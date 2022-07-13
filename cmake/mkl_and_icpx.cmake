find_program(ICPX_EXECUTABLE NAMES icpx
	HINTS $ENV{CMPLR_ROOT}/linux/bin $ENV{ONEAPI_ROOT}/compiler/latest/linux/bin
	PATHS /opt/intel/oneapi/compiler/latest/linux/bin)

# the ENABLE_OMP_OFFLOAD is only required to unhide MKL::sycl in intel's MKLConfig.cmake file
set(ENABLE_OMP_OFFLOAD ON)
set(MKL_THREADING sequential)

find_package(MKL CONFIG
	HINTS $ENV{MKLROOT}/lib/cmake/mkl  $ENV{ONEAPI_ROOT}/mkl/latest/lib/cmake/mkl
	PATHS /opt/intel/oneapi/mkl/latest/lib/cmake/mkl)

# message(STATUS "ICPX : ${ICPX_EXECUTABLE} MKL: ${MKL_FOUND} ")

if(ICPX_EXECUTABLE AND MKL_FOUND)
  message(STATUS "Found both MLK and ICPX")
  set(SYCL_AVAILABLE ON)
else()
  set(SYCL_AVAILABLE OFF)
endif()
