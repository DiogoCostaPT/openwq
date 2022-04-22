
# Build with
# rm CMakeCache.txt; cmake . -DCMAKE_C_COMPILER=gcc-11 -DCMAKE_CXX_COMPILER=g++-11; make -B

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# compile_OpenWQ_mode options:
# 1) stdl -> compile standalone using main
# 1) crhm -> compile with crhm using hydrolink
# 1) summa -> compile with summa using hydrolink
set(compile_OpenWQ_mode crhm)

if("${compile_OpenWQ_mode}" STREQUAL "standalone")
        file(GLOB SOURCES "main.cpp" "src/*.cpp")

elseif("${compile_OpenWQ_mode}" STREQUAL "crhm")
        file(GLOB SOURCES
                "../vcl.cpp"
                "../GlobalDll.cpp"
                "../Common.cpp"
                "../ClassCRHM.cpp"
                "../ClassModule.cpp"
                "../NewModules.cpp" 
                "../CRHM_parse.cpp"
                "../MacroUnit.cpp"
                "../WQ_Soil_NPCcycling.cpp"
                "../WQ_CRHM.cpp"
                "../About.cpp"
                "../Bld.cpp"
                "../Log.cpp"
                "../UpdateForm.cpp"
                "../CRHMmain.cpp"
                "../OpenWQ_hydrolink.cpp"
                "src/*.cpp")
        #include_directories("../")
        list(REMOVE_ITEM SOURCES "src/main.cpp")

elseif("${compile_OpenWQ_mode}" STREQUAL "summa")
        #file(GLOB SOURCES "main.cpp" "src/*.cpp")
endif()

IF(UNIX)
        IF(UNIX AND NOT APPLE)
                MESSAGE(STATUS "current platform : Linux")
        ELSEIF(APPLE)
                MESSAGE(STATUS "current platform : MacOS")
        ENDIF()
        project(openwq)
        
        set(CMAKE_CXX_STANDARD 17)
        #set(CMAKE_CXX_FLAGS "-O3")
        set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic")
        
        IF(APPLE)
                MESSAGE(STATUS "add flags")
                set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic -Wno-c++11-narrowing")
                
                #include(CheckCCompilerFlag)
                #set(CMAKE_REQUIRED_LINK_OPTIONS "-arch;x86_64")
                #check_c_compiler_flag("-arch x86_64" x86_64Supported)
                #message("x86_64Supported=${x86_64Supported}")
                
                # Compiling for x86
                #set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE INTERNAL "" FORCE)
                #set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE INTERNAL "" FORCE)

        ENDIF()

        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
                set(CXX_FILESYSTEM_LIBRARIES "stdc++fs")
        endif()
        #set(CMAKE_CXX_FLAGS "-O2 -g ")
        #set(CMAKE_BUILD_TYPE RelWithDebInfo)

        #set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR} "/usr/src/openmp_stdl/vtk/build")

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

        if("${compile_OpenWQ_mode}" STREQUAL "standalone")
                target_include_directories(
                        openwq PUBLIC 
                        ${ARMADILLO_INCLUDE_DIR}
                        ${VTK_INCLUDE_DIR})

                target_link_libraries(
                        openwq PUBLIC
                        ${ARMADILLO_LIBRARIES}
                        ${VTK_LIBRARIES})

        elseif("${compile_OpenWQ_mode}" STREQUAL "crhm")
                find_package(boost_filesystem 1.71.0 REQUIRED CONFIG)
                #find_package(Boost 1.72.0 EXACT REQUIRED PATHS "boost_1_72_0/")

                target_include_directories(
                        openwq PUBLIC 
                        ${Boost_INCLUDE_DIRS}
                        ${ARMADILLO_INCLUDE_DIR}
                        ${VTK_INCLUDE_DIR})

                target_link_libraries(
                        openwq PUBLIC
                        ${Boost_LIBRARIES}
                        ${ARMADILLO_LIBRARIES}
                        ${VTK_LIBRARIES})

        elseif("${compile_OpenWQ_mode}" STREQUAL "summa")

        endif()


ELSE()
        MESSAGE(STATUS "current platform : Windows")
        project(openwq)

        set(CMAKE_CXX_STANDARD 17)
        #set(CMAKE_CXX_FLAGS "-O3")
        set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic -fopenmp")
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
