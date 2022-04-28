
#################################################
# How to use this CMakeLists
# 1) Change the compilation mode below: set(CompileMode XXXX)
# 2) Open terminal and go to the folder that contains CMakeLists.txt.
# 3) Compile with: rm CMakeCache.txt; cmake . -DCMAKE_C_COMPILER=gcc-11 -DCMAKE_CXX_COMPILER=g++-11; make -B
# That's all that needs to be changed
#################################################

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

#################################################
# Select the compilation mode
# That's all that needs to be changed
#################################################
# 1) openwq -> compile standalone using main
# 2) crhm_openwq -> compile with crhm using hydrolink
# 3) summa_openwq -> compile with summa using hydrolink
# 4) mesh_openwq -> compile with mesh using hydrolink
set(CompileMode crhm_openwq)

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Don't change beyond this point

PROJECT("${CompileMode}")

#################################################
# if mode: Standalone
#################################################
if("${CompileMode}" STREQUAL "openwq")
        
        set(exec_name openwq)

        file(GLOB SOURCES "main.cpp" "src/*.cpp")

        if(UNIX)

                if(UNIX AND NOT APPLE)
                        MESSAGE(STATUS "current platform : Linux")
                elseif(APPLE)
                        MESSAGE(STATUS "current platform : MacOS")
                endif()
                
                set(CMAKE_CXX_STANDARD 17)
                #set(CMAKE_CXX_FLAGS "-O3")
                set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic")
                #set(CMAKE_CXX_FLAGS "-O2 -g ")
                #set(CMAKE_BUILD_TYPE RelWithDebInfo)
                
                if(APPLE)
                        MESSAGE(STATUS "add flags")
                        set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic -Wno-c++11-narrowing")
                        
                        #include(CheckCCompilerFlag)
                        #set(CMAKE_REQUIRED_LINK_OPTIONS "-arch;x86_64")
                        #check_c_compiler_flag("-arch x86_64" x86_64Supported)
                        #message("x86_64Supported=${x86_64Supported}")
                        
                        # Compiling for x86
                        #set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE INTERNAL "" FORCE)
                        #set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE INTERNAL "" FORCE)

                endif()

                if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
                        set(CXX_fileSYSTEM_LIBRARIES "stdc++fs")
                endif()

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

                find_package(Armadillo 11.0.1 REQUIRED) # don't forget to activate HDF5 support when installing Armadillo
                find_package(OpenMP REQUIRED)

                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

                set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

                add_executable("${exec_name}"
                        ${SOURCES})

                target_include_directories(
                        "${exec_name}" PUBLIC 
                        ${ARMADILLO_INCLUDE_DIR}
                        ${VTK_INCLUDE_DIR})

                target_link_libraries(
                        "${exec_name}" PUBLIC
                        ${ARMADILLO_LIBRARIES}
                        ${VTK_LIBRARIES})

        ELSE()
                MESSAGE(STATUS "current platform : Windows")
                project("${exec_name}")

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

                add_executable("${exec_name}"
                        ${SOURCES})

                #please set the directory to the "Armadillo" and "vtk" manually here
                target_include_directories(
                        openwq PUBLIC 
                        "I:/FLUXOS_cpp/Armadillo/include"}
                        ${VTK_INCLUDE_DIR})

                target_link_libraries(
                        "${exec_name}"
                        ${VTK_LIBRARIES})

        endif()

#################################################
# if mode: coupled (CRHM)
#################################################
elseif("${CompileMode}" STREQUAL "crhm_openwq")

        set(exec_name crhm_openwq)

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

        if(UNIX)
                if(UNIX AND NOT APPLE)
                        MESSAGE(STATUS "current platform : Linux")
                elseif(APPLE)
                        MESSAGE(STATUS "current platform : MacOS")
                endif()
                
                
                set(CMAKE_CXX_STANDARD 17)
                #set(CMAKE_CXX_FLAGS "-O3")
                set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic")
                #set(CMAKE_CXX_FLAGS "-O2 -g ")
                #set(CMAKE_BUILD_TYPE RelWithDebInfo)
                
                if(APPLE)
                        MESSAGE(STATUS "add flags")
                        set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic -Wno-c++11-narrowing")
                        
                        #include(CheckCCompilerFlag)
                        #set(CMAKE_REQUIRED_LINK_OPTIONS "-arch;x86_64")
                        #check_c_compiler_flag("-arch x86_64" x86_64Supported)
                        #message("x86_64Supported=${x86_64Supported}")
                        
                        # Compiling for x86
                        #set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE INTERNAL "" FORCE)
                        #set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE INTERNAL "" FORCE)

                endif()

                if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
                        set(CXX_fileSYSTEM_LIBRARIES "stdc++fs")
                endif()

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

                find_package(Armadillo 11.0.1 REQUIRED) # don't forget to activate HDF5 support when installing Armadillo
                find_package(OpenMP REQUIRED)
                #find_package(boost_filesystem 1.71.0 REQUIRED CONFIG)
                #find_package(Boost 1.72.0 EXACT REQUIRED PATHS "boost_1_72_0/")
                find_package(boost)

                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

                set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

                add_executable("${exec_name}"
                        ${SOURCES})

                target_include_directories(
                        "${exec_name}" PUBLIC 
                        ${Boost_INCLUDE_DIRS}
                        ${ARMADILLO_INCLUDE_DIR}
                        ${VTK_INCLUDE_DIR})

                target_link_libraries(
                        "${exec_name}" PUBLIC
                        ${Boost_LIBRARIES}
                        ${ARMADILLO_LIBRARIES}
                        ${VTK_LIBRARIES})

        ELSE()
                MESSAGE(STATUS "current platform : Windows")
                MESSAGE(STATUS "Not supported yet!")

        endif()

#################################################
# if mode: coupled (SUMMA)
#################################################
elseif("${CompileMode}" STREQUAL "summa_openwq")

        set(exec_name summa_openwq)
        
        

#################################################
# if mode: coupled (SUMMA)
#################################################
elseif("${CompileMode}" STREQUAL "mesh_openwq")

        set(exec_name mesh_openwq)

endif()
