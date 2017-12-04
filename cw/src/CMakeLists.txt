project(mpi_openmp_raytracer)
cmake_minimum_required(VERSION 3.2.2)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++14)

# Find a supported openmp flag
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif(OPENMP_FOUND)
find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

set(SOURCE main.cpp)
add_executable(mpi_openmp_raytracer ${SOURCE})
target_link_libraries(mpi_openmp_raytracer ${MPI_CXX_LIBRARIES})
install(TARGETS mpi_openmp_raytracer RUNTIME DESTINATION bin)