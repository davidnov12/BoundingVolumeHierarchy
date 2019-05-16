/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* mortonCodeComputation.cs
*/

#version 450

// Setup size of workgroup
layout(local_size_x = 256, local_size_y = 1) in;

// Input coordinations of vertices
layout(std430, binding = 10) buffer inCoordsBuff {
	float inCoords[];
};

// Outup morton codes
layout(std430, binding = 11) buffer outBuff {
	uint outMorton[];
};

uniform vec3 minCoord, maxCoord;	// minimum and maximum coordinations of geometry
uniform int numberOfTriangles;		// number of input triangles


/*
* Compute coordinations of triangle centroid
*
* a, b, c - triangle vertices
* return coordinations of triangle centroid
*/
vec3 computeCentroid(vec3 a, vec3 b, vec3 c) {
	return (a + b + c) / 3.0f;
}

/*
* Normalization of input coordinates
*
* coord - input coordinates
* return - nomalized input coordinates
*/
vec3 normalizeCoord(vec3 coord) {

	vec3 intLength = maxCoord - minCoord;
	vec3 normalizedPosition = coord - minCoord;
	normalizedPosition /= intLength;

	return normalizedPosition;
}

/*
* Expansion of bits
*
* x - value to expand
* return expanded value
*/
uint expandBits(uint x) {

	x = (x * 0x00010001u) & 0xFF0000FFu;
	x = (x * 0x00000101u) & 0x0F00F00Fu;
	x = (x * 0x00000011u) & 0xC30C30C3u;
	x = (x * 0x00000005u) & 0x49249249u;

	return x;
}

/*
* Computation of moton code
*
* coord - input coordinate (triangle centroid)
* return morton code of point coord
*/
uint mortonCode(vec3 coord) {

	coord.x = min(max(coord.x * 1024.0f, 0.0f), 1023.0f);
	coord.y = min(max(coord.y * 1024.0f, 0.0f), 1023.0f);
	coord.z = min(max(coord.z * 1024.0f, 0.0f), 1023.0f);

	uint x = expandBits(uint(coord.x));
	uint y = expandBits(uint(coord.y));
	uint z = expandBits(uint(coord.z));

	return x * 4 + y * 2 + z;
}

void main() {

	uint threadID = gl_GlobalInvocationID.x;

	// Maximum of active threads = numberOfTriangles
	if (threadID >= numberOfTriangles)
		return;

	// Get trianlge vertices
	vec3 a = vec3(inCoords[9 * threadID], inCoords[9 * threadID + 1], inCoords[9 * threadID + 2]);
	vec3 b = vec3(inCoords[9 * threadID + 3], inCoords[9 * threadID + 4], inCoords[9 * threadID + 5]);
	vec3 c = vec3(inCoords[9 * threadID + 6], inCoords[9 * threadID + 7], inCoords[9 * threadID + 8]);

	// Get center and compute morton code
	vec3 center = normalizeCoord(computeCentroid(a, b, c));
	uint mortonCode = mortonCode(center);

	// Save morton code into input buffer (2 copies)
	outMorton[threadID] = mortonCode;
	outMorton[threadID + numberOfTriangles] = mortonCode;

}
