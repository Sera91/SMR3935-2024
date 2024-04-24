/* Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------
// Transpose
//
// This file contains both device and host code for transposing a floating-point
// matrix.  It performs several transpose kernels, which incrementally improve
// performance through coalescing, removing shared memory bank conflicts, and
// eliminating partition camping.  Several of the kernels perform a copy, used
// to represent the best case performance that a transpose can achieve.
//
// Please see the whitepaper in the docs folder of the transpose project for a
// detailed description of this performance study.
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <cassert>
//#include <cooperative_groups.h>

#define checkCudaErrors( func_call ) func_call

// Each block transposes/copies a tile of TILE_DIM x TILE_DIM elements
// using TILE_DIM x BLOCK_ROWS threads, so that each thread transposes
// TILE_DIM/BLOCK_ROWS elements.  TILE_DIM must be an integral multiple of
// BLOCK_ROWS

#define TILE_DIM 16
#define BLOCK_ROWS 16

// This sample assumes that MATRIX_SIZE_X = MATRIX_SIZE_Y
int MATRIX_SIZE_X = 1024;
int MATRIX_SIZE_Y = 1024;
int MUL_FACTOR = TILE_DIM;

#define FLOOR(a, b) (a - (a % b))

// Compute the tile size necessary to illustrate performance cases for SM20+
// hardware
int MAX_TILES = (FLOOR(MATRIX_SIZE_X, 512) * FLOOR(MATRIX_SIZE_Y, 512)) /
                (TILE_DIM * TILE_DIM);

// Number of repetitions used for timing.  Two sets of repetitions are
// performed: 1) over kernel launches and 2) inside the kernel over just the
// loads and stores

#define NUM_REPS 100

// -------------------------------------------------------
// Copies
// width and height must be integral multiples of TILE_DIM
// -------------------------------------------------------

__global__ void copy(float *odata, float *idata, int width, int height) {
  int xIndex = blockIdx.x * TILE_DIM + threadIdx.x;
  int yIndex = blockIdx.y * TILE_DIM + threadIdx.y;

  int index = xIndex + width * yIndex;

  for (int i = 0; i < TILE_DIM; i += BLOCK_ROWS) {
    odata[index + i * width] = idata[index + i * width];
  }
}

__global__ void copySharedMem(float *odata, float *idata, int width,
                              int height) {
  __shared__ float tile[TILE_DIM][TILE_DIM];

  int xIndex = blockIdx.x * TILE_DIM + threadIdx.x;
  int yIndex = blockIdx.y * TILE_DIM + threadIdx.y;

  int index = xIndex + width * yIndex;

  for (int i = 0; i < TILE_DIM; i += BLOCK_ROWS) {
    if (xIndex < width && yIndex < height) {
      tile[threadIdx.y][threadIdx.x] = idata[index];
    }
  }

  __syncthreads();

  for (int i = 0; i < TILE_DIM; i += BLOCK_ROWS) {
    if (xIndex < height && yIndex < width) {
      odata[index] = tile[threadIdx.y][threadIdx.x];
    }
  }
}

// -------------------------------------------------------
// Transposes
// width and height must be integral multiples of TILE_DIM
// -------------------------------------------------------

__global__ void transposeNaive(float *odata, float *idata, int width,
                               int height) {
  int xIndex = blockIdx.x * TILE_DIM + threadIdx.x;
  int yIndex = blockIdx.y * TILE_DIM + threadIdx.y;

  int index_in = xIndex + width * yIndex;
  int index_out = yIndex + height * xIndex;

  for (int i = 0; i < TILE_DIM; i += BLOCK_ROWS) {
    odata[index_out + i] = idata[index_in + i * width];
  }
}

// coalesced transpose (with bank conflicts)

__global__ void transposeCoalesced(float *odata, float *idata, int width,
                                   int height) {
  __shared__ float tile[TILE_DIM][TILE_DIM];

  int xIndex = blockIdx.x * TILE_DIM + threadIdx.x;
  int yIndex = blockIdx.y * TILE_DIM + threadIdx.y;
  int index_in = xIndex + (yIndex)*width;

  xIndex = blockIdx.y * TILE_DIM + threadIdx.x;
  yIndex = blockIdx.x * TILE_DIM + threadIdx.y;
  int index_out = xIndex + (yIndex)*height;

  for (int i = 0; i < TILE_DIM; i += BLOCK_ROWS) {
    tile[threadIdx.y + i][threadIdx.x] = idata[index_in + i * width];
  }

  __syncthreads();

  for (int i = 0; i < TILE_DIM; i += BLOCK_ROWS) {
    odata[index_out + i * height] = tile[threadIdx.x][threadIdx.y + i];
  }
}

// Coalesced transpose with no bank conflicts

__global__ void transposeNoBankConflicts(float *odata, float *idata, int width,
                                         int height) {
  __shared__ float tile[TILE_DIM][TILE_DIM + 1];

  int xIndex = blockIdx.x * TILE_DIM + threadIdx.x;
  int yIndex = blockIdx.y * TILE_DIM + threadIdx.y;
  int index_in = xIndex + (yIndex)*width;

  xIndex = blockIdx.y * TILE_DIM + threadIdx.x;
  yIndex = blockIdx.x * TILE_DIM + threadIdx.y;
  int index_out = xIndex + (yIndex)*height;

  for (int i = 0; i < TILE_DIM; i += BLOCK_ROWS) {
    tile[threadIdx.y + i][threadIdx.x] = idata[index_in + i * width];
  }

  __syncthreads();

  for (int i = 0; i < TILE_DIM; i += BLOCK_ROWS) {
    odata[index_out + i * height] = tile[threadIdx.x][threadIdx.y + i];
  }
}

// ---------------------
// host utility routines
// ---------------------

void computeTransposeCPU(float *gold, float *idata, const int size_x,
                         const int size_y) {
  for (int y = 0; y < size_y; ++y) {
    for (int x = 0; x < size_x; ++x) {
      gold[(x * size_y) + y] = idata[(y * size_x) + x];
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
//! Compare two arrays of arbitrary type
//! @return  true if \a reference and \a data are identical, otherwise false
//! @param reference  timer_interface to the reference data / gold image
//! @param data       handle to the computed data
//! @param len        number of elements in reference and data
//! @param epsilon    epsilon to use for the comparison
//////////////////////////////////////////////////////////////////////////////

inline bool compareData(const float *reference, const float *data,
                        const unsigned int len, const float epsilon,
                        const float threshold) {
  assert(epsilon >= 0);

  bool result = true;
  unsigned int error_count = 0;

  for (unsigned int i = 0; i < len; ++i) {
    float diff = static_cast<float>(reference[i]) - static_cast<float>(data[i]);
    bool comp = (diff <= epsilon) && (diff >= -epsilon);
    result &= comp;

    error_count += !comp;
  }

  if (threshold == 0.0f) {
    return (result) ? true : false;
  } else {
    if (error_count) {
      printf("%4.2f(%%) of bytes mismatched (count=%d)\n",
             static_cast<float>(error_count) * 100 / static_cast<float>(len),
             error_count);
    }

    return (len * threshold > error_count) ? true : false;
  }
}

int main(int argc, char **argv)
{
  if (argc != 2) {
    printf("Transpose: Must specify a matrix dimension\n");
    return 1;
  }

  // Matrix dimensions
  int size_x = 512;
  size_x = atoi(argv[1]);
  int size_y = size_x;

  float total_tiles = (float)MAX_TILES;

  if (size_x % TILE_DIM != 0 || size_y % TILE_DIM != 0) {
    printf("Matrix size must be integral multiple of tile size %d\nExiting...\n\n", TILE_DIM);
    exit(EXIT_FAILURE);
  }

  // kernel pointer and descriptor
  void (*kernel)(float *, float *, int, int);
  const char *kernelName;

  // execution configuration parameters
  dim3 grid(size_x / TILE_DIM, size_y / TILE_DIM);
  dim3 threads(TILE_DIM, BLOCK_ROWS);

  if (grid.x < 1 || grid.y < 1) {
    printf("Grid size computation incorrect in test\nExiting...\n\n");
    exit(EXIT_FAILURE);
  }

  // CUDA events
  cudaEvent_t start, stop;

  // size of memory required to store the matrix
  size_t mem_size = static_cast<size_t>(sizeof(float) * size_x * size_y);

  // allocate host memory
  float *h_idata = (float *)malloc(mem_size);
  float *h_odata = (float *)malloc(mem_size);
  float *transposeGold = (float *)malloc(mem_size);
  float *gold;

  // allocate device memory
  float *d_idata, *d_odata;
  checkCudaErrors(cudaMalloc((void **)&d_idata, mem_size));
  checkCudaErrors(cudaMalloc((void **)&d_odata, mem_size));

  // initialize host data
  for (int i = 0; i < (size_x * size_y); ++i) {
    h_idata[i] = (float)i;
  }

  // copy host data to device
  checkCudaErrors(
      cudaMemcpy(d_idata, h_idata, mem_size, cudaMemcpyHostToDevice));

  // Compute reference transpose solution
  computeTransposeCPU(transposeGold, h_idata, size_x, size_y);

  // print out common data for all kernels
  printf(
      "\nMatrix size: %dx%d (%dx%d tiles), tile size: %dx%d, block size: "
      "%dx%d\n\n",
      size_x, size_y, size_x / TILE_DIM, size_y / TILE_DIM, TILE_DIM, TILE_DIM,
      TILE_DIM, BLOCK_ROWS);

  // initialize events
  checkCudaErrors(cudaEventCreate(&start));
  checkCudaErrors(cudaEventCreate(&stop));

  //
  // loop over different kernels
  //

  bool success = true;

  for (int k = 0; k < 5; k++) {
    // set kernel pointer
    switch (k) {
      case 0:
        kernel = &copy;
        kernelName = "simple copy       ";
        break;

      case 1:
        kernel = &copySharedMem;
        kernelName = "shared memory copy";
        break;

      case 2:
        kernel = &transposeNaive;
        kernelName = "naive             ";
        break;

      case 3:
        kernel = &transposeCoalesced;
        kernelName = "coalesced         ";
        break;

      case 4:
        kernel = &transposeNoBankConflicts;
        kernelName = "optimized         ";
        break;

      default:
        break;
    }

    // set reference solution
    if (kernel == &copy || kernel == &copySharedMem) {
      gold = h_idata;
    } else {
      gold = transposeGold;
    }

    // Clear error status
    checkCudaErrors(cudaGetLastError());

    // warmup to avoid timing startup
    kernel<<<grid, threads>>>(d_odata, d_idata, size_x, size_y);

    // take measurements for loop over kernel launches
    checkCudaErrors(cudaEventRecord(start, 0));

    for (int i = 0; i < NUM_REPS; i++) {
      kernel<<<grid, threads>>>(d_odata, d_idata, size_x, size_y);
      // Ensure no launch failure
      checkCudaErrors(cudaGetLastError());
    }

    checkCudaErrors(cudaEventRecord(stop, 0));
    checkCudaErrors(cudaEventSynchronize(stop));
    float kernelTime;
    checkCudaErrors(cudaEventElapsedTime(&kernelTime, start, stop));

    checkCudaErrors(
        cudaMemcpy(h_odata, d_odata, mem_size, cudaMemcpyDeviceToHost));

    bool res = compareData(gold, h_odata, size_x * size_y, 0.01f, 0.0f);

    if (res == false) {
      printf("*** %s kernel FAILED ***\n", kernelName);
      success = false;
    }

    // report effective bandwidths
    float kernelBandwidth = 2.0f * 1000.0f * mem_size / (1024 * 1024 * 1024) /
                            (kernelTime / NUM_REPS);
    printf(
        "transpose %s, Throughput = %.4f GB/s, Time = %.5f ms, Size = %u fp32 "
        "elements, NumDevsUsed = %u, Workgroup = %u\n",
        kernelName, kernelBandwidth, kernelTime / NUM_REPS, (size_x * size_y),
        1, TILE_DIM * BLOCK_ROWS);
  }

  // cleanup
  free(h_idata);
  free(h_odata);
  free(transposeGold);
  cudaFree(d_idata);
  cudaFree(d_odata);

  checkCudaErrors(cudaEventDestroy(start));
  checkCudaErrors(cudaEventDestroy(stop));

  if (!success) {
    printf("Test failed!\n");
    exit(EXIT_FAILURE);
  }

  printf("Test passed\n");
  exit(EXIT_SUCCESS);
}
