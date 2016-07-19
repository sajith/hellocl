/*
 * OpenCL hello world
 *
 * Should compile and run on Linux: assuming that OpenCL ICD loader,
 * the actual client driver, etc are correctly installed.
 */

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <CL/opencl.h>

#include "names.h"
#include "query.h"
#include "print.h"

const char *addVecSrc[] = {
        "__kernel void addVec(__global int *a, ",
        "                     __global int *b, ",
        "                     __global int *c)" ,
        "{",
        "        unsigned int n = get_global_id(0);",
        "        c[n] = a[n] + b[n];",
        "}"
};

int g_verbose_flag = 0;

int main(int argc, char **argv)
{
        int ch;
        int vecSize = 10;
        
        while ((ch = getopt (argc, argv, "vs:")) != -1) {
                switch (ch) {
                case 'v':
                        g_verbose_flag = 1;
                        break;
                case 's':
                        vecSize = strtol(optarg, NULL, 10);
                        if (vecSize < 1) {
                                fprintf(stderr, "%d is the wrong size",
                                        vecSize);
                                exit(1);
                        }
                        break;
             break;

                default:
                        abort ();
                }
        }
        
        int *aVec   = malloc(vecSize * sizeof(int));
        int *bVec   = malloc(vecSize * sizeof(int));
        int *resVec = malloc(vecSize * sizeof(int));

        if (aVec == NULL || bVec == NULL || resVec == NULL) {
                perror("malloc");
                exit(1);
        }

        /* make up a bunch of numbers. */
        for (int i = 0; i < vecSize; i++) {
                aVec[i] = rand() % (RAND_MAX / 2);
                bVec[i] = rand() % (RAND_MAX / 2);
        }

        cl_platform_id clPlatform;
        if (clGetPlatformIDs(1, &clPlatform, NULL) != CL_SUCCESS) {
                dexit("clGetPlatformIDs");
        }

        cl_device_id clDevice;
        if (clGetDeviceIDs(clPlatform,
                           CL_DEVICE_TYPE_GPU,
                           1, &clDevice, NULL) != CL_SUCCESS) {
                dexit("clGetDeviceIDs");
        }

        /* queryDevice(&clDevice); */

        clock_t startTime = clock();
        
        cl_int err;

        cl_context clContext =
                clCreateContextFromType(0,
                                        CL_DEVICE_TYPE_GPU,
                                        NULL,
                                        NULL,
                                        &err);
        errorCheck("clCreateContextFromType", err);

        /* TODO: `clCreateCommandQueue` is deprecated; use
         * `clCreateCommandQueueWithProperties` instead.  But the
         * latter segfaults on Mesa/Intel beignet 1.1.2; what's up
         * with that?
         */
        cl_command_queue clCommandQueue =
                clCreateCommandQueue(clContext,
                                     clDevice,
                                     0,
                                     &err);
        errorCheck("clCreateCommandQueue", err);

#if 0        
        /* TODO: Why does this program segfault at
         * `clCreateCommandQueueWithProperties`? */
        cl_queue_properties prop[] = { CL_QUEUE_PROPERTIES, 0, 0 };
        cl_command_queue clCommandQueue =
                clCreateCommandQueueWithProperties(clContext,
                                                   clDevice,
                                                   prop,
                                                   0);
#endif
        
        cl_mem clVec1 =
                clCreateBuffer(clContext,
                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                               sizeof(int) * vecSize,
                               aVec,
                               &err);
        errorCheck("clCreateBuffer", err);
        
        cl_mem clVec2 =
                clCreateBuffer(clContext,
                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                               sizeof(int) * vecSize,
                               bVec,
                               &err);
        errorCheck("clCreateBuffer", err);
        
        cl_mem clOutVec = clCreateBuffer(clContext,
                                         CL_MEM_WRITE_ONLY,
                                         sizeof(int) * vecSize,
                                         NULL,
                                         &err);
        errorCheck("clCreateBuffer", err);

        cl_program clProgram = clCreateProgramWithSource(clContext,
                                                         7,
                                                         addVecSrc,
                                                         NULL,
                                                         &err);
        errorCheck("clCreateProgramWithSource", err);
        
        clBuildProgram(clProgram, 0, NULL, NULL, NULL, NULL);

        cl_kernel clKernel = clCreateKernel(clProgram,
                                            "addVec",
                                            &err);
        errorCheck("clCreateKernel", err);

        clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void *) &clVec1);
        clSetKernelArg(clKernel, 1, sizeof(cl_mem), (void *) &clVec2);
        clSetKernelArg(clKernel, 2, sizeof(cl_mem), (void *) &clOutVec);

        const size_t vecSz = { vecSize };
        
        clEnqueueNDRangeKernel(clCommandQueue,
                               clKernel,
                               1,
                               NULL,
                               &vecSz,
                               NULL,
                               0,
                               NULL,
                               NULL);
        
        clEnqueueReadBuffer(clCommandQueue,
                            clOutVec,
                            CL_TRUE,
                            0,
                            sizeof(int) * vecSize,
                            resVec,
                            0,
                            NULL,
                            NULL);

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
        printf("Added %d numbers in %f\n", vecSize * 2, cpuTime);
        
        /* Check results. */
        for (int i = 0; i < vecSize; i++) {
                if (aVec[i] + bVec[i] != resVec[i]) {
                        fprintf(stderr, "Result appears to be wrong.\n");
                        exit (1);
                }
        }

        printf("All good!\n");

        free(aVec);
        free(bVec);
        free(resVec);
        
        return 0;
}
