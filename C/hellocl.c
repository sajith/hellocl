/*
 * OpenCL hello world
 *
 * Should compile and run on Linux, but does not work as expected.
 */

#include <stdio.h>
#include <string.h>
#include <CL/opencl.h>

#include "query.h"
#include "print.h"

const char *addVec[] = {
        "__kernel void addVec(__global int *a, __global int *b, __global int *c)",
        "{",
        "        unsigned int n = get_global_id(0);",
        "        c[n] = a[n] + b[n];",
        "}"
};

#define VECSZ 10

int main(int argc, char **argv)
{        
        int a[VECSZ] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        int b[VECSZ] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        int c[VECSZ] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

        cl_platform_id clPlatform;
        if (clGetPlatformIDs(1, &clPlatform, NULL) != CL_SUCCESS) {
                dexit("clGetPlatformIDs");
        }

        cl_device_id clDevice;
        if (clGetDeviceIDs(clPlatform, CL_DEVICE_TYPE_GPU, 1, &clDevice, NULL) != CL_SUCCESS) {
                dexit("clGetDeviceIDs");
        }

        /* queryDevice(&clDevice); */

        cl_context clContext = clCreateContextFromType(0, CL_DEVICE_TYPE_GPU, NULL, NULL, NULL);

        /* TODO: deprecated; use clCreateCommandQueueWithProperties */
        cl_command_queue clCommandQueue = clCreateCommandQueue(clContext, clDevice, 0, NULL);

        /* TODO: Why does this program segfault at `clCreateCommandQueueWithProperties`? */
        /* cl_queue_properties prop[] = { CL_QUEUE_PROPERTIES, 0, 0 }; */
        /* cl_command_queue clCommandQueue = clCreateCommandQueueWithProperties(clContext, clDevice, prop, 0); */
        
        cl_mem clVec1   = clCreateBuffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * VECSZ, a, NULL);
        cl_mem clVec2   = clCreateBuffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * VECSZ, b, NULL);
        cl_mem clOutVec = clCreateBuffer(clContext, CL_MEM_WRITE_ONLY, sizeof(int) * VECSZ, NULL, NULL);

        cl_program clProgram = clCreateProgramWithSource(clContext, 7, addVec, NULL, NULL);
        clBuildProgram(clProgram, 0, NULL, NULL, NULL, NULL);

        cl_kernel clKernel = clCreateKernel(clProgram, "addVec", NULL);

        clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void *) &clVec1);
        clSetKernelArg(clKernel, 1, sizeof(cl_mem), (void *) &clVec2);
        clSetKernelArg(clKernel, 2, sizeof(cl_mem), (void *) &clOutVec);

        const size_t vecSize = { VECSZ };
        
        clEnqueueNDRangeKernel(clCommandQueue, clKernel, 1, NULL, &vecSize, NULL, 0, NULL, NULL);
        clEnqueueReadBuffer(clCommandQueue, clOutVec, CL_TRUE, 0, sizeof(int) * VECSZ, c, 0, NULL, NULL);

        clReleaseKernel(clKernel);
        clReleaseProgram(clProgram);
        clReleaseCommandQueue(clCommandQueue);
        clReleaseContext(clContext);
        clReleaseMemObject(clVec1);
        clReleaseMemObject(clVec2);
        clReleaseMemObject(clOutVec);
        
        for (int i = 0; i < VECSZ; i++) {
                printf ("%3d ", a[i]);
        }

        printf(" +\n");
        
        for (int i = 0; i < VECSZ; i++) {
                printf ("%3d ", b[i]);
        }

        putchar('\n');
        for (int i = 0; i < VECSZ; i++) {
                putchar('-'); putchar('-'); putchar('-'); putchar('-');
        }
        putchar('\n');
        
        for (int i = 0; i < VECSZ; i++) {
                printf ("%3d ", c[i]);
        }
        putchar('\n');
        
        return 0;
}
