
#include <stdio.h>

#include "names.h"
#include "print.h"
#include "query.h"

cl_device_info clDevInfo[] = {
        CL_DEVICE_TYPE,
        CL_DEVICE_VENDOR_ID,
        CL_DEVICE_MAX_COMPUTE_UNITS,
        CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
        CL_DEVICE_MAX_WORK_GROUP_SIZE,
        CL_DEVICE_MAX_WORK_ITEM_SIZES,
        CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR,
        CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,
        CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,
        CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG,
        CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,
        CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE,
        CL_DEVICE_MAX_CLOCK_FREQUENCY,
        CL_DEVICE_ADDRESS_BITS,
        CL_DEVICE_MAX_READ_IMAGE_ARGS,
        CL_DEVICE_MAX_WRITE_IMAGE_ARGS,
        CL_DEVICE_MAX_MEM_ALLOC_SIZE,
        CL_DEVICE_IMAGE2D_MAX_WIDTH,
        CL_DEVICE_IMAGE2D_MAX_HEIGHT,
        CL_DEVICE_IMAGE3D_MAX_WIDTH,
        CL_DEVICE_IMAGE3D_MAX_HEIGHT,
        CL_DEVICE_IMAGE3D_MAX_DEPTH,
        CL_DEVICE_IMAGE_SUPPORT,
        CL_DEVICE_MAX_PARAMETER_SIZE,
        CL_DEVICE_MAX_SAMPLERS,
        CL_DEVICE_MEM_BASE_ADDR_ALIGN,
        CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE,
        CL_DEVICE_SINGLE_FP_CONFIG,
        CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,
        CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,
        CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,
        CL_DEVICE_GLOBAL_MEM_SIZE,
        CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,
        CL_DEVICE_MAX_CONSTANT_ARGS,
        CL_DEVICE_LOCAL_MEM_TYPE,
        CL_DEVICE_LOCAL_MEM_SIZE,
        CL_DEVICE_ERROR_CORRECTION_SUPPORT,
        CL_DEVICE_PROFILING_TIMER_RESOLUTION,
        CL_DEVICE_ENDIAN_LITTLE,
        CL_DEVICE_AVAILABLE,
        CL_DEVICE_COMPILER_AVAILABLE,
        CL_DEVICE_EXECUTION_CAPABILITIES,
        CL_DEVICE_QUEUE_ON_HOST_PROPERTIES,
        CL_DEVICE_NAME,
        CL_DEVICE_VENDOR,
        CL_DRIVER_VERSION,
        CL_DEVICE_PROFILE,
        CL_DEVICE_VERSION,
        CL_DEVICE_EXTENSIONS,
        CL_DEVICE_PLATFORM,
        CL_DEVICE_DOUBLE_FP_CONFIG,
        CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_INT,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE,
        CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF,
        CL_DEVICE_OPENCL_C_VERSION,
        CL_DEVICE_LINKER_AVAILABLE,
        CL_DEVICE_BUILT_IN_KERNELS,
        CL_DEVICE_IMAGE_MAX_BUFFER_SIZE,
        CL_DEVICE_IMAGE_MAX_ARRAY_SIZE,
        CL_DEVICE_PARENT_DEVICE,
        CL_DEVICE_PARTITION_MAX_SUB_DEVICES,
        CL_DEVICE_PARTITION_PROPERTIES,
        CL_DEVICE_PARTITION_AFFINITY_DOMAIN,
        CL_DEVICE_PARTITION_TYPE,
        CL_DEVICE_REFERENCE_COUNT,
        CL_DEVICE_PREFERRED_INTEROP_USER_SYNC,
        CL_DEVICE_PRINTF_BUFFER_SIZE,
        CL_DEVICE_IMAGE_PITCH_ALIGNMENT,
        CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT,
        CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS,
        CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE,
        CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES,
        CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE,
        CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE,
        CL_DEVICE_MAX_ON_DEVICE_QUEUES,
        CL_DEVICE_MAX_ON_DEVICE_EVENTS,
        CL_DEVICE_SVM_CAPABILITIES,
        CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE,
        CL_DEVICE_MAX_PIPE_ARGS,
        CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS,
        CL_DEVICE_PIPE_MAX_PACKET_SIZE,
        CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT,
        CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT,
        CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT
};

void queryDevice(cl_device_id *clDevice)
{
        int n = sizeof(clDevInfo) / sizeof(clDevInfo[0]);

        for (int i = 0; i < n; i++) {
                cl_device_info info = clDevInfo[i];
                const char *name    = clDeviceInfoString(info);
                char buffer[1024]   = {0};

                int ret = clGetDeviceInfo(*clDevice, info, sizeof(buffer), &buffer, NULL);
                
                if (ret != CL_SUCCESS) {
                        const char *errstr = clErrorString(ret);
                        derror("Error on clGetDeviceInfo (info: %s, error: %s)\n", name, errstr);
                } else {
                        printf("%s: %s\n", name, buffer);
                }
        }
}