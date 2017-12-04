// Kernels don't return values -> hence void
// Global -> the values are available to all cores
__kernel void vecadd(__global int *A, __global int *B, __global int *C)
{

	// Get the work item's unique ID (current GPU "thread")
	int idx = get_global_id(0);

	C[idx] = A[idx] + B[idx];
}
