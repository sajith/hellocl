
#ifndef __names_h__
#define __names_h__

#include <CL/opencl.h>

void errorCheck(const char *name, cl_int err);
const char *clErrorString(int err);
const char *clDeviceInfoString(cl_device_info info);

#endif /* __names_h__ */
