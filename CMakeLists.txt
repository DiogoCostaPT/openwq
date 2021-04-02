cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

IF(UNIX)
        IF(UNIX AND NOT APPLE)
                MESSAGE(STATUS "current platform : Linux")
        ELSEIF(APPLE)
                MESSAGE(STATUS "current platform : MacOS")
        ENDIF()
        project(openwq)

        file(GLOB SOURCES "src/*.cpp")

        set(CMAKE_CXX_STANDARD 17)
        #set(CMAKE_CXX_FLAGS "-O3")
        set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic")
        IF(APPLE)
                MESSAGE(STATUS "add flags")
                set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic -Wno-c++11-narrowing")
        ENDIF()
        #set(CMAKE_CXX_FLAGS "-O2 -g ")
        #set(CMAKE_BUILD_TYPE RelWithDebInfo)

        find_package(VTK 
                COMPONENTS 
                CommonCore
                CommonDataModel
                FiltersGeneral
                IOXML
                InteractionStyle
                RenderingCore
                RenderingFreeType
                RenderingOpenGL2 

                REQUIRED)

        find_package(Armadillo 10.3.0 REQUIRED) # don't forget to activate HDF5 support when installing Armadillo
        find_package(OpenMP REQUIRED)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

        add_executable(openwq
                ${SOURCES})

        target_include_directories(
                openwq PUBLIC 
                ${ARMADILLO_INCLUDE_DIR}
                ${VTK_INCLUDE_DIR})

        target_link_libraries(
                openwq
                ${ARMADILLO_LIBRARIES}
                ${VTK_LIBRARIES})

ELSE()
        MESSAGE(STATUS "current platform : Windows")
        project(openwq)

        file(GLOB SOURCES "src/*.cpp")

        set(CMAKE_CXX_STANDARD 17)
        #set(CMAKE_CXX_FLAGS "-O3")
        set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic")
        #set(CMAKE_CXX_FLAGS "-O2 -g ")
        #set(CMAKE_BUILD_TYPE RelWithDebInfo)

        find_package(VTK 
                COMPONENTS 
                CommonCore
                CommonDataModel
                FiltersGeneral
                IOXML
                InteractionStyle
                RenderingCore
                RenderingFreeType
                RenderingOpenGL2 

                REQUIRED)

        find_package(OpenMP REQUIRED)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

        add_executable(openwq
                ${SOURCES})

        #please set the directory to the "Armadillo" and "vtk" manually here
        target_include_directories(
                openwq PUBLIC 
                "I:/FLUXOS_cpp/Armadillo/include"}
                ${VTK_INCLUDE_DIR})

        target_link_libraries(
                openwq
                ${VTK_LIBRARIES})

ENDIF()
