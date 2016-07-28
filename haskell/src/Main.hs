module Main where

import           Control.Parallel.OpenCL
import           Foreign                 (castPtr, nullPtr, sizeOf)
import           Foreign.Marshal.Array   (newArray, peekArray)

-- vector addition, OpenCL edition.
addVecSrc :: String
addVecSrc = unlines [ "__kernel void addVec(__global int *a,"
                    , "                     __global int *b,"
                    , "                     __global int *c)"
                    , "{                                    "
                    , "        unsigned int n = get_global_id(0);"
                    , "        c[n] = a[n] + b[n];               "
                    , "}"
                    ]

main :: IO ()
main = do
    (platform:_) <- clGetPlatformIDs
    (device:_) <- clGetDeviceIDs platform CL_DEVICE_TYPE_ALL
    context <- clCreateContext [CL_CONTEXT_PLATFORM platform] [device] print
    q <- clCreateCommandQueue context device []

    program <- clCreateProgramWithSource context addVecSrc
    clBuildProgram program [device] ""
    kernel <- clCreateKernel program "addVec"

    let len = 1024 :: Int
        vSz = sizeOf (0 :: Int) * len

    -- Kernel arguments could be [CFloat], but printing results are
    -- bonkers. Printing integers are more... reassuring.
    aVec <- newArray ([0..(len-1)] :: [Int])
    bVec <- newArray ([0..(len-1)] :: [Int])
    oVec <- newArray ([0..(len-1)] :: [Int])

    inVec1 <- clCreateBuffer context
              [CL_MEM_READ_ONLY, CL_MEM_COPY_HOST_PTR] (vSz, castPtr aVec)
    inVec2 <- clCreateBuffer context
              [CL_MEM_READ_ONLY, CL_MEM_COPY_HOST_PTR] (vSz, castPtr bVec)
    outVec <- clCreateBuffer context
              [CL_MEM_WRITE_ONLY] (vSz, nullPtr)

    clSetKernelArgSto kernel 0 inVec1
    clSetKernelArgSto kernel 1 inVec2
    clSetKernelArgSto kernel 2 outVec

    exec <- clEnqueueNDRangeKernel q kernel [vSz] [] []

    _ <- clEnqueueReadBuffer q outVec True 0 vSz (castPtr oVec) [exec]

    result <- peekArray len oVec

    print result
