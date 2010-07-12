# Check for AMD Stream SDK first, use environment variable.
set(ENV_ATISTREAMSDKROOT $ENV{ATISTREAMSDKROOT})
if(ENV_ATISTREAMSDKROOT)
  find_path(OPENCL_INCLUDE_DIR
            NAMES CL/cl.h OpenCL/cl.h
            PATHS ${ENV_ATISTREAMSDKROOT}/include
            NO_DEFAULT_PATH)

  # An alternative test here would be something like UNIX AND NOT APPLE since
  # Apple's OpenCL is entirely different. Then again, we wouldn't be searching
  # for the StreamSDK on that platform. On the other hand, the StreamSDK isn't
  # supported except on Linux anyways.
  if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
      set(OPENCL_LIB_SEARCH_PATH
	  ${OPENCL_LIB_SEARCH_PATH}
	  ${ENV_ATISTREAMSDKROOT}/lib/x86)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(OPENCL_LIB_SEARCH_PATH
          ${OPENCL_LIB_SEARCH_PATH}
          ${ENV_ATISTREAMSDKROOT}/lib/x86_64)
    else(CMAKE_SIZEOF_VOID_P EQUAL 4)
      message(FATAL_ERROR
	      "Your platform does not appear to be either 32 or 64 bits")
    endif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  else("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    message(FATAL_ERROR "The stream sdk is currently only supported on linux")
  endif("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
  find_library(OPENCL_LIBRARY
               NAMES OpenCL
               PATHS ${OPENCL_LIB_SEARCH_PATH}
	       NO_DEFAULT_PATH)
# TODO: Add CUDA toolkit support. For now, fallback to searching the usual
# suspect paths.
else(ENV_ATISTREAMSDKROOT)
  find_path(OPENCL_INCLUDE_DIR NAMES CL/cl.h OpenCL/cl.h)

  find_library(OPENCL_LIBRARY NAMES OpenCL)
endif(ENV_ATISTREAMSDKROOT)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OPENCL DEFAULT_MSG
                                  OPENCL_LIBRARY OPENCL_INCLUDE_DIR)

if(OPENCL_FOUND)
  set(OPENCL_LIBRARIES ${OPENCL_LIBRARY})
else(OPENCL_FOUND)
  set(OPENCL_LIBRARIES)
endif(OPENCL_FOUND)

mark_as_advanced(OPENCL_INCLUDE_DIR OPENCL_LIBRARY)
