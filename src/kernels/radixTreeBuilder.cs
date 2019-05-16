/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* radixTreeBuilder.cs
*/


#version 450

// Structure of BVH node
struct node{

	vec4 _min;
	vec4 _max;

	int left;
	int right;
	int triangleA;
	int triangleB;

	int parent;
	int sibling;
	int tmp;
	int gap;
};

// Algorithm phases
#define BUILD_PHASE 0
#define AABB_PHASE 1

// Workgroup size
layout(local_size_x = 256, local_size_y = 1) in;

// Input sorted morton codes
layout(std430, binding = 11) buffer mortonInBuffer{
    uint inMortons[];
};

// Output BVH nodes
layout(std430, binding = 14) buffer nodesOutBuffer{
    node nodes[];
};

// Indices of triangles
layout(std430, binding = 12) buffer indicesBuffer{
    uint indices[];
};

// Coordinations of triangles vertices
layout(std430, binding = 10) buffer coordsBuffer{
    float coords[];
};

uniform int size;
uniform int baseOffset = 0;
uniform int phase;

int prev;

/*
* Signum function
*
* x - value to get signum
* return signum value of x
*/
int sgn(int x){

	if(x > 0) return 1;
  else if(x < 0) return -1;

	return 0;
}

/*
* Count of leading zeros in value
*
* val - input value
* return number of leading zeros in val (in binary representation)
*/
int clz(int val){

  int count = 0;
  while(val != 0){

    val >>= 1;
    count++;
  }

  return 32 - count;
}

/*
* Delta of 2 morton codes (length of longest prefix)
*
* a, b - indices of input morton codes
* return length of longest common prefix of 2 morton codes
*/
int delta(int a, int b){
	if(a < 0 || a > size || b < 0 || b > size)
  	return -1;

  int res, dres;

	if(inMortons[a] == inMortons[b]){
		res = int(inMortons[a]) ^ int(inMortons[b]);
		dres = int(indices[a]) ^ int(indices[b]);
		res = clz(res) + clz(dres);
	}
	else{
  	res = int(inMortons[a]) ^ int(inMortons[b]);
		res = clz(res);
	}

	return res;
}

/*
* Finds minimum and maximum coordinations of given geometry (two triangles)
*
* i - index of current node
* start, end - first and last primitive of current node
*/
void findMinMax(int i, int start, int end){

  vec4 _min, _max;

  _min.x = min(min(coords[9 * indices[start]], min(coords[9 * (indices[start]) + 3], coords[9 * (indices[start]) + 6])),
           min(min(coords[9 * indices[end]], coords[9 * (indices[end]) + 3]), coords[9 * (indices[end]) + 6]));
  _min.y = min(min(coords[9 * indices[start] + 1], min(coords[9 * (indices[start]) + 4], coords[9 * (indices[start]) + 7])),
           min(min(coords[9 * indices[end] + 1], coords[9 * (indices[end]) + 4]), coords[9 * (indices[end]) + 7]));
  _min.z = min(min(coords[9 * indices[start] + 2], min(coords[9 * (indices[start]) + 5], coords[9 * (indices[start]) + 8])),
           min(min(coords[9 * indices[end] + 2], coords[9 * (indices[end]) + 5]), coords[9 * (indices[end]) + 8]));

  _max.x = max(max(coords[9 * indices[start]], max(coords[9 * (indices[start]) + 3], coords[9 * (indices[start]) + 6])),
           max(max(coords[9 * indices[end]], coords[9 * (indices[end]) + 3]), coords[9 * (indices[end]) + 6]));
  _max.y = max(max(coords[9 * indices[start] + 1], max(coords[9 * (indices[start]) + 4], coords[9 * (indices[start]) + 7])),
           max(max(coords[9 * indices[end] + 1], coords[9 * (indices[end]) + 4]), coords[9 * (indices[end]) + 7]));
  _max.z = max(max(coords[9 * indices[start] + 2], max(coords[9 * (indices[start]) + 5], coords[9 * (indices[start]) + 8])),
           max(max(coords[9 * indices[end] + 2], coords[9 * (indices[end]) + 5]), coords[9 * (indices[end]) + 8]));

  nodes[i]._min = _min;
  nodes[i]._max = _max;

}

/*
* Finds minimum and maximum coordinations of given geometry (one triangle + one node)
*
* i - index of current node
* tri - index of triangle in node
* node - index of child node
*/
void findMinMaxMisc(int i, int tri, int node){

	vec4 _min, _max;

	_min.x = min(min(coords[9 * indices[tri]], min(coords[9 * (indices[tri]) + 3], coords[9 * (indices[tri]) + 6])), nodes[node]._min.x);
	_min.y = min(min(coords[9 * indices[tri] + 1], min(coords[9 * (indices[tri]) + 4], coords[9 * (indices[tri]) + 7])), nodes[node]._min.y);
	_min.z = min(min(coords[9 * indices[tri] + 2], min(coords[9 * (indices[tri]) + 5], coords[9 * (indices[tri]) + 8])), nodes[node]._min.z);

	_max.x = max(max(coords[9 * indices[tri]], max(coords[9 * (indices[tri]) + 3], coords[9 * (indices[tri]) + 6])), nodes[node]._max.x);
	_max.y = max(max(coords[9 * indices[tri] + 1], max(coords[9 * (indices[tri]) + 4], coords[9 * (indices[tri]) + 7])), nodes[node]._max.y);
	_max.z = max(max(coords[9 * indices[tri] + 2], max(coords[9 * (indices[tri]) + 5], coords[9 * (indices[tri]) + 8])), nodes[node]._max.z);

	nodes[i]._min = _min;
	nodes[i]._max = _max;

}

/*
* Finds minimum and maximum coordinations of given geometry (two nodes)
*
* i - index of current node
* a, b - child nodes indices
*/
void findMinMaxNodes(int i, int a, int b){

	vec4 _min, _max;

	_min.x = min(nodes[a]._min.x, nodes[b]._min.x);
	_min.y = min(nodes[a]._min.y, nodes[b]._min.y);
	_min.z = min(nodes[a]._min.z, nodes[b]._min.z);

	_max.x = max(nodes[a]._max.x, nodes[b]._max.x);
	_max.y = max(nodes[a]._max.y, nodes[b]._max.y);
	_max.z = max(nodes[a]._max.z, nodes[b]._max.z);

	nodes[i]._min = _min;
	nodes[i]._max = _max;

}

/*
* Building of BVH tree, founds child nodes of current node, found belonging primitives
*
* index - index of processed node
*/
void processNode(int index){

	// Node initialization
	nodes[index].left = -1;
	nodes[index].right = -1;
	nodes[index].triangleA = -1;
	nodes[index].triangleB = -1;
	nodes[index].tmp = 0;
	nodes[index]._min = vec4(1e6f);
	nodes[index]._max = vec4(-1e6f);

	// Direction of sequence
  int d = sgn(delta(index, index + 1) - delta(index, index - 1));

  int d_min = delta(index, index - d);
  int l_max = 2;

	// Maximum length of sequence
  while(delta(index, index + (l_max * d)) > d_min){
    l_max *= 2;
  }

  int l = 0;
  int divider = 2;
  for(int t = l_max / divider; t >= 1; divider *= 2){

		if(delta(index, index + (l + t) * d) > d_min){
			l = l + t;
		}

		if(t == 1)
			break;
    t = l_max / divider;

	}

  int j = index + l * d;
  int d_node = delta(index, j);

  int s = 0;
  divider = 2;
  for(int t = (l + (divider - 1)); t >= 1; divider *= 2){

		if(delta(index, index + (s + t) * d) > d_node){
      s += t;
		}

		if (t == 1)
			break;

		t = (l + (divider - 1)) / divider;

  }

  int y = index + s * d + min(d, 0);

	// Childs settings - left
  if(min(index, j) == y){
    nodes[index].triangleA = y;
  }
  else{
    nodes[index].left = y;

		if(index != size)
			nodes[y].parent = index;

		else
			nodes[y].parent = 0;
  }

	// Childs settings - right
  if(max(index, j) == (y + 1)){
    nodes[index].triangleB = y + 1;
  }
  else{
    nodes[index].right = y + 1;

		if(index != size)
			nodes[y + 1].parent = index;

		else
			nodes[y + 1].parent = 0;
  }

	if(index == 0)
		nodes[index].parent = -1;

	nodes[index].sibling = y;
	nodes[index].gap = j;

}

/*
* Computes minimal AABB for BVH node
*
* index - index of BVH node
*/
void findAABB(int index){

	if(nodes[index].triangleA == -1 || nodes[index].triangleB == -1)
		return;

	while(index != -1){

		// Node with two triangles
		if(nodes[index].left == -1 && nodes[index].right == -1){
			findMinMax(index, nodes[index].triangleA, nodes[index].triangleB);
			prev = index;
			index = nodes[index].parent;
		}

		// Node with one triangles + one child node
		else if(nodes[index].left != -1 && nodes[index].right != -1){
			nodes[index].gap = index;
			atomicAdd(nodes[index].tmp, 1);
			if(atomicCompSwap(nodes[index].tmp, 2, 3) != 2)	return;

			findMinMaxNodes(index, nodes[index].left, nodes[index].right);
			prev = index;
			index = nodes[index].parent;

		}

		// Node with two child nodes
		else{
			int tri = max(nodes[index].triangleA, nodes[index].triangleB);
			int nd = max(nodes[index].left, nodes[index].right);

			findMinMaxMisc(index, tri, nd);
			prev = index;
			index = nodes[index].parent;
		}

	}

}

void main(){

  int i = int(gl_GlobalInvocationID.x);

  if(i > size)
    return;

	if(phase == BUILD_PHASE)
	processNode(i);

	else if(phase == AABB_PHASE)
	findAABB(i);

}
