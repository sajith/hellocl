/*
 * OpenCL hello world
 *
 * Should compile and run on Linux, if OpenCL ICD loader, the actual
 * client driver, etc are correctly installed.
 */

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <CL/opencl.h>

#include "query.h"
#include "print.h"

const char *addVecSrc[] = {
        "__kernel void addVec(__global int *a, __global int *b, __global int *c)",
        "{",
        "        unsigned int n = get_global_id(0);",
        "        c[n] = a[n] + b[n];",
        "}"
};

int g_verbose_flag = 0;

int main(int argc, char **argv)
{
        int ch;
        int vecsize = 10;
        while ((ch = getopt (argc, argv, "vs:")) != -1) {
                switch (ch) {
                case 'v':
                        g_verbose_flag = 1;
                        break;
                case 's':
                        vecsize = strtol(optarg, NULL, 10);
                        if (vecsize < 1) {
                                fprintf(stderr, "%d is the wrong size", vecsize);
                                exit(1);
                        }
                        break;
             break;

                default:
                        abort ();
                }
        }
        
        int *a = malloc(vecsize * sizeof(int));
        int *b = malloc(vecsize * sizeof(int));
        int *c = malloc(vecsize * sizeof(int));

        if (a == NULL || b == NULL || c == NULL) {
                perror("malloc");
                exit(1);
        }

        /* make up a bunch of numbers. */
        for (int i = 0; i < vecsize; i++) {
                a[i] = rand() % (RAND_MAX / 2);
                b[i] = rand() % (RAND_MAX / 2);
        }

        cl_platform_id clPlatform;
        if (clGetPlatformIDs(1, &clPlatform, NULL) != CL_SUCCESS) {
                dexit("clGetPlatformIDs");
        }

        cl_device_id clDevice;
        if (clGetDeviceIDs(clPlatform, CL_DEVICE_TYPE_GPU, 1, &clDevice, NULL) != CL_SUCCESS) {
                dexit("clGetDeviceIDs");
        }

        /* queryDevice(&clDevice); */

        clock_t startTime = clock();
        
        cl_int err;

        cl_context clContext = clCreateContextFromType(0, CL_DEVICE_TYPE_GPU, NULL, NULL, &err);
        errorCheck("clCreateContextFromType", err);

        /* TODO: deprecated; use clCreateCommandQueueWithProperties */
        cl_command_queue clCommandQueue = clCreateCommandQueue(clContext, clDevice, 0, &err);
        errorCheck("clCreateCommandQueue", err);

        /* TODO: Why does this program segfault at `clCreateCommandQueueWithProperties`? */
        /* cl_queue_properties prop[] = { CL_QUEUE_PROPERTIES, 0, 0 }; */
        /* cl_command_queue clCommandQueue = clCreateCommandQueueWithProperties(clContext, clDevice, prop, 0); */
        
        cl_mem clVec1 = clCreateBuffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * vecsize, a, &err);
        errorCheck("clCreateBuffer", err);
        
        cl_mem clVec2 = clCreateBuffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * vecsize, b, &err);
        errorCheck("clCreateBuffer", err);
        
        cl_mem clOutVec = clCreateBuffer(clContext, CL_MEM_WRITE_ONLY, sizeof(int) * vecsize, NULL, &err);
        errorCheck("clCreateBuffer", err);

        cl_program clProgram = clCreateProgramWithSource(clContext, 5, addVecSrc, NULL, &err);
        errorCheck("clCreateProgramWithSource", err);
        
        clBuildProgram(clProgram, 0, NULL, NULL, NULL, NULL);

        cl_kernel clKernel = clCreateKernel(clProgram, "addVec", &err);
        errorCheck("clCreateKernel", err);

        clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void *) &clVec1);
        clSetKernelArg(clKernel, 1, sizeof(cl_mem), (void *) &clVec2);
        clSetKernelArg(clKernel, 2, sizeof(cl_mem), (void *) &clOutVec);

        const size_t vecSize = { vecsize };
        
        clEnqueueNDRangeKernel(clCommandQueue, clKernel, 1, NULL, &vecSize, NULL, 0, NULL, NULL);
        clEnqueueReadBuffer(clCommandQueue, clOutVec, CL_TRUE, 0, sizeof(int) * vecsize, c, 0, NULL, NULL);

        clReleaseKernel(clKernel);
        clReleaseProgram(clProgram);
        clReleaseCommandQueue(clCommandQueue);
        clReleaseContext(clContext);
        clReleaseMemObject(clVec1);
        clReleaseMemObject(clVec2);
        clReleaseMemObject(clOutVec);

        clock_t endTime = clock();

        /* TODO: The time calculated this way doesn't appear to be
         * correct.  Go back to old notes!
         */
        double cpuTime = (endTime - startTime) / CLOCKS_PER_SEC;
        printf("Added %d numbers in %f\n", vecsize * 2, cpuTime);
        
        /* Check results. */
        for (int i = 0; i < vecsize; i++) {
                if (a[i] + b[i] != c[i]) {
                        fprintf(stderr, "Result appears to be wrong.\n");
                        exit (1);
                }
        }

        printf("All good!\n");
        
        return 0;
}
