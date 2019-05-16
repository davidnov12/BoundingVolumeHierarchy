/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* parallelRadixSort.cs
*/


#version 450

// Algorithm phases
#define HISTOGRAM_PHASE 0
#define SCAN_PHASE 1
#define REORDER_PHASE 2

// Setup workgroup size
layout(local_size_x = 256, local_size_y = 1) in;

// Helper bucket for radix sort algorithm
layout(std430, binding = 13) buffer histBuffer{
    int bucket[];
};

// Input morton codes
layout(std430, binding = 11) buffer mortonInBuffer{
    uint inMortons[];
};

// Input indices of triangles
layout(std430, binding = 12) buffer indexInBuffer{
    uint inIndices[];
};

uniform int base;
uniform int phase;
uniform int size;
uniform int inOffset;
uniform int outOffset;
uniform int delta;
uniform int inBucket;
uniform int outBucket;

/*
* Find out, what bit is on current position and save into helper buffer
*
* index - index of processed morton code
*/
void computeHistogram(int index){
  bucket[(size * int((inMortons[inOffset + index] & base) == base)) + index] = 1;
}

/*
* Computes prefix sum of items in radix bucket
*
* index - index of element for sum
*/
void prefixSum(int index){

  if(index < delta){
	bucket[outBucket + index] = bucket[inBucket + index];
    return;
	}

  bucket[outBucket + index] = bucket[inBucket + index] + bucket[inBucket + index - delta];

}

/*
* Reorder items on appropriate positions
*
* index - index of element for reorder
*/
void reorderElements(int index){

  // Get in and out indices
  int inIndex = inOffset + index;
  int outIndex = outOffset + bucket[(size * int((inMortons[inIndex] & base) == base)) + index + outBucket] - 1;

  // Move morton code and trinagle index on appropriate position
  inMortons[outIndex] = inMortons[inIndex];
  inIndices[outIndex] = inIndices[inIndex];

  // Set values in bucket to zero
  bucket[index] = 0;
  bucket[size + index] = 0;
  bucket[2 * size + index] = 0;
  bucket[3 * size + index] = 0;
}

void main(){

  int threadID = int(gl_GlobalInvocationID.x);

  if(threadID >= size)
    return;

  if(phase == HISTOGRAM_PHASE)
    computeHistogram(threadID);

  else if(phase == SCAN_PHASE)
    prefixSum(threadID);

  else if(phase == REORDER_PHASE)
    reorderElements(threadID);

}
