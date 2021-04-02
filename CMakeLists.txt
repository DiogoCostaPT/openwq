cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(openwq)

file(GLOB SOURCES "src/*.cpp")

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic")
#set(CMAKE_CXX_FLAGS "-O2 -g ")
#set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(VTK 
        COMPONENTS 
        vtkCommonCore
        vtkCommonDataModel
        vtkFiltersGeneral
        vtkIOXML
        vtkInteractionStyle
        vtkRenderingCore
        vtkRenderingFreeType
        vtkRenderingOpenGL2 

        REQUIRED)

find_package(Armadillo 10.3.0 REQUIRED) # don't forget to activate HDF5 support when installing Armadillo
find_package(OpenMP REQUIRED)
find_package(HDF5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(openwq
        ${SOURCES})

target_link_libraries(
        openwq
        ${ARMADILLO_LIBRARIES}
        ${VTK_LIBRARIES})


