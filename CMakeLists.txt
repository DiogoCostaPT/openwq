
#################################################
# How to use this CMakeLists
# 1) Change the compilation mode below: set(COMPILE_MODE XXXX)
# 2) Open terminal and go to the folder openwq folder that contains CMakeLists.txt, Dockerfile, and docker-compose.yml
# 3) Create a docker container: docker compose up -d
# 4) check the container id: docker ps
# 5) Open a shell inside that container: docker exec -it <container id> /bin/bash
# 6) Armadillo now disables default hdf5 support for save/load, so inside the container you need to go to “/usr/include/armadillo_bits/config.hpp” and uncomment #define ARMA_USE_HDF5
# 7) Inside the contained, go to the openwq folder
# 8) Compile with: rm CMakeCache.txt; rm -r CMakeFiles; cmake . ; make -B

# That's all !!!
#################################################

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
# FLAGS_NOAH = -O3 -ffree-form -ffree-line-length-none -fmax-errors=0 $(FLAGS_OMP)
# FLAGS_COMM = -O3 -ffree-line-length-none -fmax-errors=0 $(FLAGS_OMP)
# FLAGS_SUMMA = -O3 -ffree-line-length-none -fmax-errors=0 $(FLAGS_OMP)

# Debug runs
#FLAGS_NOAH = -p -g -ffree-form -ffree-line-length-none -fmax-errors=0 -fbacktrace -Wno-unused -Wno-unused-dummy-argument
#FLAGS_COMM = -p -g -Wall -ffree-line-length-none -fmax-errors=0 -fbacktrace -fcheck=bounds
#FLAGS_SUMMA = -p -g -Wall -ffree-line-length-none -fmax-errors=0 -fbacktrace -fcheck=bounds


#################################################
# Select the compilation mode
# That's all that needs to be changed
#################################################
# 1) openwq -> compile standalone using main
# 2) crhm_openwq -> compile with crhm using hydrolink
# 3) summa_openwq -> compile with summa using hydrolink
# 4) mesh_openwq -> compile with mesh using hydrolink
set(COMPILE_MODE summa_openwq)

# Choose where to create the executable
set(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(EXEC_DIR ${PARENT_DIR}/apply)

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Don't change beyond this point

#################################################
# if mode: Standalone
#################################################
if("${COMPILE_MODE}" STREQUAL "openwq")
        
        project("${COMPILE_MODE}")

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

                #find_package(VTK 
                #        COMPONENTS 
                #        CommonCore
                #        CommonDataModel
                #        FiltersGeneral
                #        IOXML
                #        InteractionStyle
                #        RenderingCore
                #        RenderingFreeType
                #        RenderingOpenGL2 

                #        REQUIRED)

                find_package(Armadillo REQUIRED) # don't forget to activate HDF5 support when installing Armadillo
                find_package(HDF5 REQUIRED)
                find_package(OpenMP REQUIRED)

                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

                set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR})

                add_executable(
                        ${exec_name}
                        ${SOURCES})

                target_include_directories(
                        ${exec_name} PUBLIC 
                        ${ARMADILLO_INCLUDE_DIR}
                        ${HDF5_INCLUDE_DIRS}
                        #${VTK_INCLUDE_DIR}
                        )

                target_link_libraries(
                        ${exec_name} PUBLIC
                        ${ARMADILLO_LIBRARIES}
                        ${HDF5_C_LIBRARY_hdf5}
                        #${VTK_LIBRARIES}
                        )

        ELSE()
                MESSAGE(STATUS "current platform : Windows")
                project(${exec_name})

                set(CMAKE_CXX_STANDARD 17)
                #set(CMAKE_CXX_FLAGS "-O3")
                set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic -fopenmp")
                #set(CMAKE_CXX_FLAGS "-O2 -g ")
                #set(CMAKE_BUILD_TYPE RelWithDebInfo)

                #find_package(VTK 
                #        COMPONENTS 
                #        CommonCore
                #        CommonDataModel
                #        FiltersGeneral
                #        IOXML
                #        InteractionStyle
                #        RenderingCore
                #        RenderingFreeType
                #        RenderingOpenGL2 

                #        REQUIRED)

                find_package(OpenMP REQUIRED)

                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

                set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR})

                add_executable(
                        ${exec_name}
                        ${SOURCES})

                #please set the directory to the "Armadillo" and "vtk" manually here
                target_include_directories(
                        ${exec_name} PUBLIC 
                        "I:/FLUXOS_cpp/Armadillo/include"
                        ${HDF5_INCLUDE_DIRS}
                        #${VTK_INCLUDE_DIR}
                        )

                target_link_libraries(
                        ${exec_name}
                        ${HDF5_C_LIBRARY_hdf5}
                        #${VTK_LIBRARIES}
                        )

        endif()

#################################################
# if mode: coupled (CRHM)
#################################################
elseif("${COMPILE_MODE}" STREQUAL "crhm_openwq")

        project("${COMPILE_MODE}")

        set(exec_name crhm_openwq)

        set(CRHM_DIR ${PARENT_DIR}/crhm)
        set(OPENWQ_DIR ${PARENT_DIR}/openwq)
        set(HYDROLINK_DIR ${PARENT_DIR})

        file(GLOB SOURCES
                ${CRHM_DIR}/vcl.cpp
                ${CRHM_DIR}/GlobalDll.cpp
                ${CRHM_DIR}/Common.cpp
                ${CRHM_DIR}/ClassCRHM.cpp
                ${CRHM_DIR}/ClassModule.cpp
                ${CRHM_DIR}/NewModules.cpp
                ${CRHM_DIR}/CRHM_parse.cpp
                ${CRHM_DIR}/MacroUnit.cpp
                ${CRHM_DIR}/WQ_Soil_NPCcycling.cpp
                ${CRHM_DIR}/WQ_CRHM.cpp
                ${CRHM_DIR}/About.cpp
                ${CRHM_DIR}/Bld.cpp
                ${CRHM_DIR}/Log.cpp
                ${CRHM_DIR}/UpdateForm.cpp
                ${CRHM_DIR}/CRHMmain.cpp
                ${OPENWQ_DIR}/src/*.cpp
                ${HYDROLINK_DIR}/OpenWQ_hydrolink.cpp)
        #include_directories("../")
        list(REMOVE_ITEM SOURCES ${OPENWQ_DIR}/main.cpp)

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

                #find_package(VTK 
                #        COMPONENTS 
                #        CommonCore
                #        CommonDataModel
                #        FiltersGeneral
                #        IOXML
                #        InteractionStyle
                #        RenderingCore
                #        RenderingFreeType
                #        RenderingOpenGL2 

                #        REQUIRED)

                find_package(Armadillo REQUIRED) # don't forget to activate HDF5 support when installing Armadillo
                find_package(HDF5 REQUIRED)
                find_package(OpenMP REQUIRED)
                #find_package(boost_filesystem 1.71.0 REQUIRED CONFIG)
                #find_package(Boost 1.72.0 EXACT REQUIRED PATHS "boost_1_72_0/")
                find_package(Boost)

                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

                set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR})

                # Create the executable with openWQ coupled to CRHM   
                add_executable(
                        ${exec_name}
                        ${SOURCES})

                target_include_directories(
                        ${exec_name} PUBLIC 
                        ${Boost_INCLUDE_DIRS}
                        ${ARMADILLO_INCLUDE_DIR}
                        ${HDF5_INCLUDE_DIRS}
                        #${VTK_INCLUDE_DIR}
                        )

                target_link_libraries(
                        ${exec_name} PUBLIC
                        ${Boost_LIBRARIES}
                        ${ARMADILLO_LIBRARIES}
                        ${HDF5_C_LIBRARY_hdf5}
                        #${VTK_LIBRARIES}
                        )

        ELSE()
                MESSAGE(STATUS "current platform : Windows")
                MESSAGE(STATUS "Not supported yet!")

        endif()

#################################################
# if mode: coupled (SUMMA)
#################################################
elseif("${COMPILE_MODE}" STREQUAL "summa_openwq")
        
        # FLAGS_NOAH = -O3 -ffree-form -ffree-line-length-none -fmax-errors=0 $(FLAGS_OMP)
        # FLAGS_COMM = -O3 -ffree-line-length-none -fmax-errors=0 $(FLAGS_OMP)
        # FLAGS_SUMMA = -O3 -ffree-line-length-none -fmax-errors=0 $(FLAGS_OMP)

        # Debug runs
        #FLAGS_NOAH = -p -g -ffree-form -ffree-line-length-none -fmax-errors=0 -fbacktrace -Wno-unused -Wno-unused-dummy-argument
        #FLAGS_COMM = -p -g -Wall -ffree-line-length-none -fmax-errors=0 -fbacktrace -fcheck=bounds
        #FLAGS_SUMMA = -p -g -Wall -ffree-line-length-none -fmax-errors=0 -fbacktrace -fcheck=bounds

        set(exec_name summa_openwq)

        IF(UNIX)
                IF(UNIX AND NOT APPLE)
                        MESSAGE(STATUS "current platform : Linux")
                ELSEIF(APPLE)
                        MESSAGE(STATUS "current platform : MacOS")
                ENDIF()
                # Declare use of multiple Languages
                project(openwq_summa LANGUAGES CXX Fortran)
                # C needs to be enables
                enable_language(C)
                # Verify language compilers can talk to each other
                include(FortranCInterface)
                FortranCInterface_VERIFY(CXX)

                # Get paths to source code
                set(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
                message(STATUS "PARENT DIR = ${PARENT_DIR}")
                set(SUMMA_DIR ${PARENT_DIR}/summa)
                message(STATUS "SUMMA_DIR = ${SUMMA_DIR}")
                set(OPENWQ_DIR ${PARENT_DIR})

                ############################################################################
                ## File definitions for subcomponents - For OpenWQ
                ############################################################################
                file(GLOB OPENWQ_SOURCES 
                        ${OPENWQ_DIR}/src/*.cpp 
                        ${PARENT_DIR}/OpenWQ_hydrolink.cpp)

                ############################################################################
                ## File definitions for subcomponents - For Summa
                ############################################################################
                set(DRIVER_DIR ${SUMMA_DIR}/build/source/driver)
                set(HOOKUP_DIR ${SUMMA_DIR}/build/source/hookup)
                set(NETCDF_DIR ${SUMMA_DIR}/build/source/netcdf)
                set(DSHARE_DIR ${SUMMA_DIR}/build/source/dshare)
                set(NOAHMP_DIR ${SUMMA_DIR}/build/source/noah-mp)
                set(ENGINE_DIR ${SUMMA_DIR}/build/source/engine)
                set(INTERFACE_DIR ${SUMMA_DIR}/build/source/interface)

                #  utilities
                set(NRUTIL
                ${ENGINE_DIR}/nrtype.f90
                ${ENGINE_DIR}/f2008funcs.f90
                ${ENGINE_DIR}/nr_utility.f90)

                # Numerical recipes procedures
                # NOTE: all numerical recipes procedures are now replaced with free versions
                set(NRPROC
                ${ENGINE_DIR}/expIntegral.f90
                ${ENGINE_DIR}/spline_int.f90)

                # Hook-up modules (set files and directory paths)
                SET(HOOKUP
                ${HOOKUP_DIR}/ascii_util.f90
                ${HOOKUP_DIR}/summaFileManager.f90)

                # Data modules
                set(DATAMS
                ${INTERFACE_DIR}/openWQ.f90 # needs to be compiled before globalData
                ${DSHARE_DIR}/multiconst.f90
                ${DSHARE_DIR}/var_lookup.f90
                ${DSHARE_DIR}/data_types.f90
                ${DSHARE_DIR}/globalData.f90
                ${DSHARE_DIR}/flxMapping.f90
                ${DSHARE_DIR}/get_ixname.f90
                ${DSHARE_DIR}/popMetadat.f90
                ${DSHARE_DIR}/outpt_stat.f90)

                # utility modules
                set(UTILMS
                ${ENGINE_DIR}/time_utils.f90
                ${ENGINE_DIR}/mDecisions.f90
                ${ENGINE_DIR}/snow_utils.f90
                ${ENGINE_DIR}/soil_utils.f90
                ${ENGINE_DIR}/updatState.f90
                ${ENGINE_DIR}/matrixOper.f90)

                # Solver
                set(SOLVER
                ${ENGINE_DIR}/vegPhenlgy.f90
                ${ENGINE_DIR}/diagn_evar.f90
                ${ENGINE_DIR}/stomResist.f90
                ${ENGINE_DIR}/groundwatr.f90
                ${ENGINE_DIR}/vegSWavRad.f90
                ${ENGINE_DIR}/vegNrgFlux.f90
                ${ENGINE_DIR}/ssdNrgFlux.f90
                ${ENGINE_DIR}/vegLiqFlux.f90
                ${ENGINE_DIR}/snowLiqFlx.f90
                ${ENGINE_DIR}/soilLiqFlx.f90
                ${ENGINE_DIR}/bigAquifer.f90
                ${ENGINE_DIR}/computFlux.f90
                ${ENGINE_DIR}/computResid.f90
                ${ENGINE_DIR}/computJacob.f90
                ${ENGINE_DIR}/eval8summa.f90
                ${ENGINE_DIR}/summaSolve.f90
                ${ENGINE_DIR}/systemSolv.f90
                ${ENGINE_DIR}/varSubstep.f90
                ${ENGINE_DIR}/opSplittin.f90
                ${ENGINE_DIR}/coupled_em.f90
                ${ENGINE_DIR}/run_oneGRU.f90
                ${ENGINE_DIR}/run_oneHRU.f90)

                # Define routines for SUMMA preliminaries
                set(PRELIM
                ${ENGINE_DIR}/conv_funcs.f90
                ${ENGINE_DIR}/sunGeomtry.f90
                ${ENGINE_DIR}/convE2Temp.f90
                ${ENGINE_DIR}/allocspace.f90
                ${ENGINE_DIR}/checkStruc.f90
                ${ENGINE_DIR}/childStruc.f90
                ${ENGINE_DIR}/ffile_info.f90
                ${ENGINE_DIR}/read_attrb.f90
                ${ENGINE_DIR}/read_pinit.f90
                ${ENGINE_DIR}/pOverwrite.f90
                ${ENGINE_DIR}/read_param.f90
                ${ENGINE_DIR}/paramCheck.f90
                ${ENGINE_DIR}/check_icond.f90)

                # Define routines for the SUMMA model runs
                set(MODRUN
                ${ENGINE_DIR}/indexState.f90
                ${ENGINE_DIR}/getVectorz.f90
                ${ENGINE_DIR}/updateVars.f90
                ${ENGINE_DIR}/var_derive.f90
                ${ENGINE_DIR}/read_force.f90
                ${ENGINE_DIR}/derivforce.f90
                ${ENGINE_DIR}/snowAlbedo.f90
                ${ENGINE_DIR}/canopySnow.f90
                ${ENGINE_DIR}/tempAdjust.f90
                ${ENGINE_DIR}/snwCompact.f90
                ${ENGINE_DIR}/layerMerge.f90
                ${ENGINE_DIR}/layerDivide.f90
                ${ENGINE_DIR}/volicePack.f90
                ${ENGINE_DIR}/qTimeDelay.f90)

                set(NETCDF
                ${NETCDF_DIR}/netcdf_util.f90
                ${NETCDF_DIR}/def_output.f90
                ${NETCDF_DIR}/modelwrite.f90
                ${NETCDF_DIR}/read_icond.f90)

                # ... stitch together common programs
                set(COMM_ALL
                ${NRPROC}
                ${HOOKUP}
                ${DATAMS}
                ${UTILMS})

                # ... stitch together SUMMA programs
                set(SUMMA_ALL
                ${NETCDF}
                ${PRELIM}
                ${MODRUN}
                ${SOLVER})

                # Define the driver routine
                set(DRIVER
                ${DRIVER_DIR}/summa_type.f90
                ${DRIVER_DIR}/summa_util.f90
                ${DRIVER_DIR}/summa_alarms.f90
                ${DRIVER_DIR}/summa_globalData.f90
                ${DRIVER_DIR}/summa_defineOutput.f90
                ${DRIVER_DIR}/summa_init.f90
                ${DRIVER_DIR}/summa_setup.f90
                ${DRIVER_DIR}/summa_restart.f90
                ${DRIVER_DIR}/summa_forcing.f90
                ${DRIVER_DIR}/summa_modelRun.f90
                ${DRIVER_DIR}/summa_openWQ.f90
                ${DRIVER_DIR}/summa_writeOutput.f90
                ${DRIVER_DIR}/summa_driver.f90)

                set(CMAKE_CXX_STANDARD 17)
                #set(CMAKE_CXX_FLAGS "-O3")
                set(CMAKE_CXX_FLAGS "-g -O0 -Wall -pedantic")
                IF(APPLE)
                        MESSAGE(STATUS "add flags")
                        set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic -Wno-c++11-narrowing")
                ENDIF()
                #set(CMAKE_CXX_FLAGS "-O2 -g ")
                #set(CMAKE_BUILD_TYPE RelWithDebInfo)

                #find_package(VTK 
                #        COMPONENTS 
                #        CommonCore
                #        CommonDataModel
                #        FiltersGeneral
                #        IOXML
                #        InteractionStyle
                #        RenderingCore
                #        RenderingFreeType
                #        RenderingOpenGL2 

                #        REQUIRED)

                find_package(Armadillo REQUIRED) # don't forget to activate HDF5 support when installing Armadillo
                find_package(HDF5 REQUIRED)
                find_package(OpenMP REQUIRED)
                find_package(netCDF REQUIRED)
                find_package(LAPACK REQUIRED)

                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

                set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR})

                # compile openWQ into an object file
                add_library(openWQ OBJECT 
                ${OPENWQ_SOURCES})
                target_compile_options(openWQ PRIVATE
                        -g3 -Wall -pedantic)
                target_include_directories(openWQ PRIVATE
                        ${ARMADILLO_INCLUDE_DIR}
                        #${VTK_INCLUDE_DIR}
                        ${HDF5_INCLUDE_DIRS}
                        ${OpenMP_INCLUDE_DIR})
                target_link_libraries(openWQ PUBLIC
                        ${ARMADILLO_LIBRARIES}
                        ${HDF5_C_LIBRARY_hdf5}
                        #${VTK_LIBRARIES}
                        -fopenmp)

                # Compile the NOAHMP into object/.mod files
                add_library(SUMMA_NOAHMP OBJECT
                        ${NOAHMP_DIR}/module_model_constants.F
                        ${NOAHMP_DIR}/module_sf_noahutl.F
                        ${NOAHMP_DIR}/module_sf_noahlsm.F
                        ${NOAHMP_DIR}/module_sf_noahmplsm.F
                        ${NRUTIL})
                        target_compile_options(SUMMA_NOAHMP PRIVATE
                                -ffree-form -ffree-line-length-none -fmax-errors=0)
                        set(CMAKE_Fortran_FLAGS "-g -O0 -ffree-line-length-none -fmax-errors=0 -lstdc++")

                # Compile Summa_Comm into object/.mod files
                add_library(SUMMA_COMM OBJECT
                        ${COMM_ALL})
                        target_compile_options(SUMMA_COMM PRIVATE
                        -ffree-line-length-none -fmax-errors=0 -lstdc++)
                        target_include_directories(SUMMA_COMM PRIVATE
                                "/usr/include/"
                                "/code/build/"
                                "/opt/local/"
                                "/opt/homebrew/Cellar/"
                                "/opt/homebrew/include/")
                        target_link_libraries(SUMMA_COMM PUBLIC
                                "/usr/lib/x86_64-linux-gnu/libnetcdff.a"
                                SUMMA_NOAHMP)

                # Create the executable with openWQ coupled to Summa                 
                add_executable(${exec_name}
                        ${SUMMA_ALL}
                        ${DRIVER})
                        set_property(TARGET ${exec_name} PROPERTY LINKER_LANGUAGE Fortran)
                        target_include_directories(
                                ${exec_name} PUBLIC
                                ${LAPACK_INCLUDES}
                                ${netCDF_INCLUDES}
                                "/usr/include/"
                                "/code/build/"
                                "/opt/local/"
                                "/opt/homebrew/Cellar/"
                                "/opt/homebrew/include/")
                        target_link_libraries(
                                ${exec_name} PUBLIC
                                "/usr/lib/x86_64-linux-gnu/libnetcdff.a"
                                ${LAPACK_LIBRARIES}
                                ${netCDF_LIBRARIES}
                                SUMMA_COMM
                                SUMMA_NOAHMP
                                openWQ)

        ELSE()
        
                MESSAGE(STATUS "current platform : Windows")
                MESSAGE(STATUS "*not supported yet!")

        ENDIF()


#################################################
# if mode: coupled (MESH)
#################################################
elseif("${COMPILE_MODE}" STREQUAL "mesh_openwq")

        MESSAGE(STATUS "Compilation mode: mesh-openwq")
        MESSAGE(STATUS "*not supported yet!")
        #set(exec_name mesh_openwq)

endif()