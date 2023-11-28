######
# STEPS TO SET UP THE APPROPRIATE Docker CONTAINER
# ====> (ONLY NEED TO SET UP ONCE!) <====
######
# 0) Start Docker
# 1) Go to the folder that contains: docker-compose.yml
# 2) clear all images and containers: docker system prune -a
# 3) check IF docker-compose.yml file is good: docker-compose config
# 4) running container: docker compose up -d 
# 5) stop container: docker compose down
# 6) see the containers running: docker ps
# 7) run a shell inside the container: docker exec -it <container name> /bin/bash

#####
# STEPS TO COMPILE THE PROGRAM WITH THIS CMakeLists
######
# 1) Change the compilation mode below: SET(COMPILE_TARGET XXXX)
# 2) Open terminal and go to the openwq folder that contains CMakeLists.txt, Dockerfile, and docker-compose.yml
# 3) Create a docker container: docker compose up -d
# 4) check the container id: docker ps
# 5) Open a shell inside that container: docker exec -it <container id> /bin/bash
# 6) Armadillo now disables default hdf5 support for save/load, so inside the container you need to go to “/usr/include/armadillo_bits/config.hpp” and uncomment #define ARMA_USE_HDF5
# 7) Inside the contained, go to the openwq folder that contains CMakeLists.txt, Dockerfile, and docker-compose.yml
# 8) Compile with: rm CMakeCache.txt; rm -r CMakeFiles; cmake . ; make -j 2; rm *.mod
# That's all !!!

CMAKE_MINIMUM_REQUIRED(VERSION 3.10 FATAL_ERROR)

#####
# USER SECTION: Select the compilation options
# That's all that needs to be changed
#####

# A) Set Compilation target:
# optA1) openwq -> compile standalone using main
# optA2) crhm_openwq -> compile with crhm using hydrolink
# optA3) summa_openwq -> compile with summa using hydrolink
# optA4) mizuroute_openwq -> compile with mizuRoute using hydrolink
# optA5) fluxos_openwq -> compile with fluxos using hydrolink
SET(COMPILE_TARGET mizuroute_openwq)

# B) Set Compilation mode:
# optB1) debug -> set the necessary flags for debugging
# optB2) fast -> set optimization flags
SET(COMPILE_MODE debug)

#####
# DON'T CHANGE BEYOND THIS POINT...............
#####

MESSAGE(STATUS "##########################################")
MESSAGE(STATUS "** OpenWQ **")
MESSAGE(STATUS "OpenWQ Compilation Setup:")
MESSAGE(STATUS "  => Compilation Target: ${COMPILE_TARGET}")
MESSAGE(STATUS "  => Compilation Mode: ${COMPILE_MODE}")
MESSAGE(STATUS "##########################################")

# ..................
# IF TARGET = openwq
# Supports linux, iOS and windows
# ..................
IF("${COMPILE_TARGET}" STREQUAL "openwq")
        
        SET(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
        SET(EXEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

        PROJECT("${COMPILE_TARGET}")
        SET(exec_name "${COMPILE_TARGET}_${COMPILE_MODE}")
        SET(CMAKE_CXX_STANDARD 17)

        FILE(GLOB SOURCES "main.cpp" "src/*.cpp")

        IF(UNIX)

                IF(UNIX AND NOT APPLE)

                        MESSAGE(STATUS "** OpenWQ **: current platform is Linux")
                        # Compilation mode
                        IF("${COMPILE_MODE}" STREQUAL "debug")
                                SET(CMAKE_CXX_FLAGS "-g -Wall -pedantic")
                        ELSEIF("${COMPILE_MODE}" STREQUAL "fast")
                                SET(CMAKE_CXX_FLAGS "-O3 -Wall -pedantic ${OpenMP_CXX_FLAGS}")
                                FIND_PACKAGE(OpenMP)
                        ENDIF()

                ELSEIF(APPLE)

                        MESSAGE(STATUS "** OpenWQ **: current platform is MacOS")
                        # Compilation mode
                        IF("${COMPILE_MODE}" STREQUAL "debug")
                                SET(CMAKE_CXX_FLAGS "-g -Wall -pedantic -Wno-c++11-narrowing")
                        ELSEIF("${COMPILE_MODE}" STREQUAL "fast")
                                SET(CMAKE_CXX_FLAGS "-O3 -Wall -pedantic -Wno-c++11-narrowing ${OpenMP_CXX_FLAGS}")
                                FIND_PACKAGE(OpenMP)
                        ENDIF()

                ENDIF()

                # Add packages
                # don't forget to activate HDF5 support when installing Armadillo (line 121)
                FIND_PACKAGE(Armadillo REQUIRED) 
                FIND_PACKAGE(HDF5 REQUIRED)

                SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR})

                ADD_EXECUTABLE(
                        ${exec_name}
                        ${SOURCES})

                TARGET_INCLUDE_DIRECTORIES(
                        ${exec_name} PUBLIC 
                        ${HDF5_INCLUDE_DIRS}
                        ${ARMADILLO_INCLUDE_DIR})

                TARGET_LINK_LIBRARIES(
                        ${exec_name} PUBLIC
                        ${HDF5_C_LIBRARY_hdf5}
                        ${ARMADILLO_LIBRARIES})

                IF(OpenMP_CXX_FOUND)
                        MESSAGE("** OpenWQ **: OpenMP enabled!")
                        TARGET_LINK_LIBRARIES(
                                ${exec_name} PUBLIC 
                                OpenMP::OpenMP_CXX)
                ENDIF()

        ELSE()

                # Windows 
                MESSAGE(STATUS "current platform : Windows")

                IF("${COMPILE_MODE}" STREQUAL "debug")
                        SET(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic")
                ELSEIF("${COMPILE_MODE}" STREQUAL "fast")
                        SET(CMAKE_CXX_FLAGS "-O3 -Wall -pedantic -Wno-c++11-narrowing ${OpenMP_CXX_FLAGS}")
                ENDIF()
 
                FIND_PACKAGE(OpenMP REQUIRED)

                SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR})

                ADD_EXECUTABLE(
                        ${exec_name}
                        ${SOURCES})

                # Please set the directory to the "Armadillo"
                TARGET_INCLUDE_DIRECTORIES(
                        ${exec_name} PUBLIC 
                        "I:/FLUXOS_cpp/Armadillo/include"
                        ${HDF5_INCLUDE_DIRS})

                TARGET_LINK_LIBRARIES(
                        ${exec_name}
                        ${HDF5_C_LIBRARY_hdf5})

        ENDIF()

# ..................
# IF TARGET = crhm_openwq
# Only supports compilation in linux
# ..................
ELSEIF("${COMPILE_TARGET}" STREQUAL "crhm_openwq")

        IF(UNIX AND NOT APPLE)

                MESSAGE(STATUS "** OpenWQ **: current platform is Linux")

                PROJECT("${COMPILE_TARGET}")
                SET(exec_name "${COMPILE_TARGET}_${COMPILE_MODE}")

                # Choose where to create the executable
                SET(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
                SET(EXEC_DIR ${PARENT_DIR}/../case_studies)

                SET(CRHM_DIR ${PARENT_DIR}/crhm)
                SET(OPENWQ_DIR ${PARENT_DIR}/openwq/openwq)
                SET(HYDROLINK_DIR ${PARENT_DIR}/openwq)

                FILE(GLOB SOURCES
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

                IF(UNIX AND NOT APPLE)
                        MESSAGE(STATUS "current platform : Linux")
                ENDIF()
                        
                SET(CMAKE_CXX_STANDARD 17)

                IF("${COMPILE_MODE}" STREQUAL "debug")
                        set(CMAKE_CXX_FLAGS "-g3 -Wall -pedantic")
                ELSEIF("${COMPILE_MODE}" STREQUAL "fast")
                        set(CMAKE_CXX_FLAGS "-O3 -Wall -pedantic ${OpenMP_CXX_FLAGS}")
                        FIND_PACKAGE(OpenMP)
                ENDIF()

                IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
                        set(CXX_fileSYSTEM_LIBRARIES "stdc++fs")
                ENDIF()

                FIND_PACKAGE(Armadillo REQUIRED) # don't forget to activate HDF5 support when installing Armadillo
                FIND_PACKAGE(HDF5 REQUIRED)
                FIND_PACKAGE(Boost)

                SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR})

                # Create the executable with openWQ coupled to CRHM   
                ADD_EXECUTABLE(
                        ${exec_name}
                        ${SOURCES})

                TARGET_INCLUDE_DIRECTORIES(
                        ${exec_name} PUBLIC 
                        ${Boost_INCLUDE_DIRS}
                        ${ARMADILLO_INCLUDE_DIR}
                        ${HDF5_INCLUDE_DIRS}
                        "/usr/include/"
                        "/code/")

                TARGET_LINK_LIBRARIES(
                        ${exec_name} PUBLIC
                        #"/usr/lib/x86_64-linux-gnu/libnetcdff.a"
                        "/usr/lib/aarch64-linux-gnu/libnetcdff.a"
                        ${Boost_LIBRARIES}
                        ${ARMADILLO_LIBRARIES}
                        ${HDF5_C_LIBRARY_hdf5})
                
                IF(OpenMP_CXX_FOUND)
                        MESSAGE("** OpenWQ **: OpenMP enabled!")
                        TARGET_LINK_LIBRARIES(
                                ${exec_name} PUBLIC 
                                OpenMP::OpenMP_CXX)
                ENDIF()

        ELSE()
                # Apple and Windows not supported
                IF(UNIX AND APPLE)
                        MESSAGE(STATUS "** OpenWQ **: current platform is MacOS")
                ELSE()
                        MESSAGE(STATUS "** OpenWQ **: current platform is Windows")
                ENDIF()
                MESSAGE(STATUS "Not supported yet!")
                MESSAGE(STATUS "Please compile the code inside the Docker container!")
                MESSAGE(STATUS "See intructions in the CMakeLists.txt.")

        ENDIF()

# ..................
# IF TARGET = summa_openwq
# Only supports compilation in linux
# ..................
ELSEIF("${COMPILE_TARGET}" STREQUAL "summa_openwq")
        
        # FLAGS_NOAH = -O3 -ffree-form -ffree-line-length-none -fmax-errors=0 $(FLAGS_OMP)
        # FLAGS_COMM = -O3 -ffree-line-length-none -fmax-errors=0 $(FLAGS_OMP)
        # FLAGS_SUMMA = -O3 -ffree-line-length-none -fmax-errors=0 $(FLAGS_OMP)

        # Debug runs
        #FLAGS_NOAH = -p -g -ffree-form -ffree-line-length-none -fmax-errors=0 -fbacktrace -Wno-unused -Wno-unused-dummy-argument
        #FLAGS_COMM = -p -g -Wall -ffree-line-length-none -fmax-errors=0 -fbacktrace -fcheck=bounds
        #FLAGS_SUMMA = -p -g -Wall -ffree-line-length-none -fmax-errors=0 -fbacktrace -fcheck=bounds

        SET(exec_name "${COMPILE_TARGET}_${COMPILE_MODE}")

        IF(UNIX AND NOT APPLE)

                MESSAGE(STATUS "** OpenWQ **: current platform is Linux")
   
                # Declare use of multiple Languages
                PROJECT(openwq_summa LANGUAGES CXX Fortran)
                # C needs to be enables
                ENABLE_LANGUAGE(C)
                # Verify language compilers can talk to each other
                INCLUDE(FortranCInterface)
                FortranCInterface_VERIFY(CXX)

                # Get paths to source code
                SET(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
                SET(EXEC_DIR ${PARENT_DIR}/../../../../case_studies)

                SET(SUMMA_DIR ${PARENT_DIR}../../../../../../)
                SET(OPENWQ_DIR ${PARENT_DIR}/openwq)

                # File definitions for subcomponents - For OpenWQ
                FILE(GLOB OPENWQ_SOURCES 
                        ${OPENWQ_DIR}/src/*.cpp 
                        ${OPENWQ_DIR}/src/global/*.cpp
                        ${OPENWQ_DIR}/src/extwatflux_ss/*.cpp
                        ${OPENWQ_DIR}/src/initiate/*.cpp
                        ${OPENWQ_DIR}/src/chem/*.cpp
                        ${OPENWQ_DIR}/src/watertransp/*.cpp
                        ${OPENWQ_DIR}/src/readjson/*.cpp
                        ${OPENWQ_DIR}/src/couplercalls/*.cpp
                        ${OPENWQ_DIR}/src/output/*.cpp
                        ${OPENWQ_DIR}/src/solver/*.cpp
                        ${OPENWQ_DIR}/src/units/*.cpp
                        ${OPENWQ_DIR}/src/utils/*.cpp
                        ${PARENT_DIR}/OpenWQ_hydrolink.cpp
                        ${PARENT_DIR}/OpenWQ_interface.cpp)

                SET(OPENWQ_INCLUDES
                        ${OPENWQ_DIR}/src/global/
                        ${OPENWQ_DIR}/src/jnlohmann/
                        ${PARENT_DIR})



                # File definitions for subcomponents - For Summa
                SET(DRIVER_DIR ${SUMMA_DIR}/build/source/driver)
                SET(HOOKUP_DIR ${SUMMA_DIR}/build/source/hookup)
                SET(NETCDF_DIR ${SUMMA_DIR}/build/source/netcdf)
                SET(DSHARE_DIR ${SUMMA_DIR}/build/source/dshare)
                SET(NOAHMP_DIR ${SUMMA_DIR}/build/source/noah-mp)
                SET(ENGINE_DIR ${SUMMA_DIR}/build/source/engine)
                SET(OPENWQ_INTERFACE_DIR ${SUMMA_DIR}/build/source/openwq)

                #  utilities
                SET(NRUTIL
                        ${ENGINE_DIR}/nrtype.f90
                        ${ENGINE_DIR}/f2008funcs.f90
                        ${ENGINE_DIR}/nr_utility.f90)

                # Numerical recipes procedures
                # NOTE: all numerical recipes procedures are now replaced with free versions
                SET(NRPROC
                        ${ENGINE_DIR}/expIntegral.f90
                        ${ENGINE_DIR}/spline_int.f90)

                # Hook-up modules (set files and directory paths)
                SET(HOOKUP
                        ${HOOKUP_DIR}/ascii_util.f90
                        ${HOOKUP_DIR}/summaFileManager.f90)

                # Data modules
                SET(DATAMS
                        ${OPENWQ_INTERFACE_DIR}/openWQ.f90 # needs to be compiled before globalData
                        ${DSHARE_DIR}/multiconst.f90
                        ${DSHARE_DIR}/var_lookup.f90
                        ${DSHARE_DIR}/data_types.f90
                        ${DSHARE_DIR}/globalData.f90
                        ${DSHARE_DIR}/flxMapping.f90
                        ${DSHARE_DIR}/get_ixname.f90
                        ${DSHARE_DIR}/popMetadat.f90
                        ${DSHARE_DIR}/outpt_stat.f90)

                # utility modules
                SET(UTILMS
                        ${ENGINE_DIR}/time_utils.f90
                        ${ENGINE_DIR}/mDecisions.f90
                        ${ENGINE_DIR}/snow_utils.f90
                        ${ENGINE_DIR}/soil_utils.f90
                        ${ENGINE_DIR}/updatState.f90
                        ${ENGINE_DIR}/matrixOper.f90)

                # Solver
                SET(SOLVER
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
                SET(PRELIM
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
                        ${ENGINE_DIR}/check_icond.f90
                        ${OPENWQ_INTERFACE_DIR}/summa_openWQ_allocspace.f90)

                # Define routines for the SUMMA model runs
                SET(MODRUN
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

                SET(NETCDF
                        ${NETCDF_DIR}/netcdf_util.f90
                        ${NETCDF_DIR}/def_output.f90
                        ${NETCDF_DIR}/modelwrite.f90
                        ${NETCDF_DIR}/read_icond.f90)

                # Stitch together common programs
                SET(COMM_ALL
                        ${NRPROC}
                        ${HOOKUP}
                        ${DATAMS}
                        ${UTILMS})

                # Stitch together SUMMA programs
                SET(SUMMA_ALL
                        ${NETCDF}
                        ${PRELIM}
                        ${MODRUN}
                        ${SOLVER})

                # Define the driver routine
                SET(DRIVER
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
                        ${DRIVER_DIR}/summa_writeOutput.f90
                        ${PARENT_DIR}/summa_openWQ.f90
                        ${DRIVER_DIR}/summa_driver.f90)

                SET(CMAKE_CXX_STANDARD 17)

                # Compilation mode
                IF("${COMPILE_MODE}" STREQUAL "debug")
                        SET(CMAKE_CXX_FLAGS "-g -O0 -Wall -pedantic")
                        SET(CMAKE_Fortran_FLAGS "-p -g -Wall -ffree-line-length-none -fmax-errors=0 -fbacktrace -fcheck=bounds")
                ELSEIF("${COMPILE_MODE}" STREQUAL "fast")
                        SET(CMAKE_CXX_FLAGS "-O3 ${OpenMP_CXX_FLAGS}")
                        SET(CMAKE_Fortran_FLAGS "-O3 -ffree-line-length-none -fmax-errors=0 -lstdc++")
                        FIND_PACKAGE(OpenMP)
                ENDIF()

                # Add packages
                # don't forget to activate HDF5 support when installing Armadillo
                FIND_PACKAGE(Armadillo REQUIRED) 
                FIND_PACKAGE(HDF5 REQUIRED)
                
                FIND_PACKAGE(netCDF REQUIRED)
                FIND_PACKAGE(LAPACK REQUIRED)

                SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR})

                # compile openWQ into an object file
                ADD_LIBRARY(openWQ OBJECT 
                        ${OPENWQ_SOURCES})
                TARGET_INCLUDE_DIRECTORIES(openWQ PRIVATE
                        ${ARMADILLO_INCLUDE_DIR}
                        ${HDF5_INCLUDE_DIRS}
                        ${OPENWQ_DIR}/src/)
                TARGET_LINK_LIBRARIES(openWQ PUBLIC
                        ${ARMADILLO_LIBRARIES}
                        ${HDF5_C_LIBRARY_hdf5})
                
                IF(OpenMP_CXX_FOUND)
                        MESSAGE("** OpenWQ **: OpenMP enabled!")
                        TARGET_LINK_LIBRARIES(
                                openWQ PUBLIC 
                                OpenMP::OpenMP_CXX)
                ENDIF()

                # Compile the NOAHMP into object/.mod files
                ADD_LIBRARY(SUMMA_NOAHMP OBJECT
                        ${NOAHMP_DIR}/module_model_constants.F
                        ${NOAHMP_DIR}/module_sf_noahutl.F
                        ${NOAHMP_DIR}/module_sf_noahlsm.F
                        ${NOAHMP_DIR}/module_sf_noahmplsm.F
                        ${NRUTIL})
                IF("${COMPILE_MODE}" STREQUAL "debug")
                        target_compile_options(SUMMA_NOAHMP PRIVATE -p -g -ffree-form -ffree-line-length-none -fmax-errors=0 -fbacktrace -Wno-unused -Wno-unused-dummy-argument)
                ELSEIF("${COMPILE_MODE}" STREQUAL "fast")
                        target_compile_options(SUMMA_NOAHMP PRIVATE -O3 -ffree-form -ffree-line-length-none -fmax-errors=0 -lstdc++ -fopenmp)
                ENDIF()

                # Compile Summa_Comm into object/.mod files
                ADD_LIBRARY(SUMMA_COMM OBJECT
                        ${COMM_ALL})
                IF("${COMPILE_MODE}" STREQUAL "debug")
                        target_compile_options(SUMMA_COMM PRIVATE -p -g -Wall -ffree-line-length-none -fmax-errors=0 -fbacktrace -fcheck=bounds)
                ELSEIF("${COMPILE_MODE}" STREQUAL "fast")
                        target_compile_options(SUMMA_COMM PRIVATE -O3 -ffree-line-length-none -fmax-errors=0 -lstdc++ -fopenmp)
                ENDIF()    
                TARGET_INCLUDE_DIRECTORIES(SUMMA_COMM PRIVATE
                        "/usr/include/"
                        "/code/")
                TARGET_LINK_LIBRARIES(SUMMA_COMM PUBLIC
                        #"/usr/lib/x86_64-linux-gnu/libnetcdff.a"
                        "/usr/lib/aarch64-linux-gnu/libnetcdff.a"
                        SUMMA_NOAHMP)

                # Create the executable with openWQ coupled to Summa                 
                ADD_EXECUTABLE(${exec_name}
                        ${SUMMA_ALL}
                        ${DRIVER})
                        SET_PROPERTY(TARGET ${exec_name} PROPERTY LINKER_LANGUAGE Fortran)
                        TARGET_INCLUDE_DIRECTORIES(
                                ${exec_name} PUBLIC
                                ${LAPACK_INCLUDES}
                                ${netCDF_INCLUDES}
                                "/usr/include/"
                                "/code/")
                        TARGET_LINK_LIBRARIES(
                                ${exec_name} PUBLIC
                                #"/usr/lib/x86_64-linux-gnu/libnetcdff.a"
                                "/usr/lib/aarch64-linux-gnu/libnetcdff.a"
                                ${LAPACK_LIBRARIES}
                                ${netCDF_LIBRARIES}
                                SUMMA_COMM
                                SUMMA_NOAHMP
                                openWQ)

        ELSE()
                # Apple and Windows not supported
                IF(UNIX AND APPLE)
                        MESSAGE(STATUS "** OpenWQ **: current platform is MacOS")
                ELSE()
                        MESSAGE(STATUS "** OpenWQ **: current platform is Windows")
                ENDIF()
                MESSAGE(STATUS "Not supported yet!")
                MESSAGE(STATUS "Please compile the code inside the Docker container!")
                MESSAGE(STATUS "See intructions in the CMakeLists.txt.")

        ENDIF()

# ..................
# IF TARGET = mizuroute_openwq
# Only supports compilation in linux
# ..................
ELSEIF("${COMPILE_TARGET}" STREQUAL "mizuroute_openwq")

        SET(exec_name "${COMPILE_TARGET}_${COMPILE_MODE}")


        # Choose where to create the executable
        SET(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
        SET(EXEC_DIR ${PARENT_DIR}/../../../case_studies/testCase_cameo_v1.2/bin)

        IF(UNIX AND NOT APPLE)

                MESSAGE(STATUS "** OpenWQ ** current platform is Linux")

                # Declare use of multiple Languages
                PROJECT("${COMPILE_TARGET}" LANGUAGES CXX Fortran)

                # C needs to be enables
                ENABLE_LANGUAGE(C)
                
                # Verify language compilers can talk to each other
                INCLUDE(FortranCInterface)
                FortranCInterface_VERIFY(CXX)

                # Get paths to source code
                SET(OPENWQ_DIR ${PARENT_DIR}/openwq/openwq)
                SET(OPENWQ_INTERFACE_DIR ${OPENWQ_DIR}/..)

                # File definitions for subcomponents - For OpenWQ
                FILE(GLOB OPENWQ_SOURCES 
                        ${OPENWQ_DIR}/src/*.cpp 
                        ${OPENWQ_DIR}/src/global/*.cpp
                        ${OPENWQ_DIR}/src/extwatflux_ss/*.cpp
                        ${OPENWQ_DIR}/src/initiate/*.cpp
                        ${OPENWQ_DIR}/src/chem/*.cpp
                        ${OPENWQ_DIR}/src/watertransp/*.cpp
                        ${OPENWQ_DIR}/src/readjson/*.cpp
                        ${OPENWQ_DIR}/src/couplercalls/*.cpp
                        ${OPENWQ_DIR}/src/output/*.cpp
                        ${OPENWQ_DIR}/src/solver/*.cpp
                        ${OPENWQ_DIR}/src/units/*.cpp
                        ${OPENWQ_DIR}/src/utils/*.cpp
                        ${OPENWQ_INTERFACE_DIR}/OpenWQ_hydrolink.cpp
                        ${OPENWQ_INTERFACE_DIR}/OpenWQ_interface.cpp)
                
                SET(OPENWQ_INCLUDES
                        ${OPENWQ_DIR}/src/global/
                        ${OPENWQ_DIR}/src/jnlohmann/
                        ${PARENT_DIR})

                SET(MIZUROUTE_DIR ${PARENT_DIR}/src)
                

                # Setting mizuroute
                SET(MIZUROUTE_UTILS
                        ${MIZUROUTE_DIR}/nrtype.f90
                        ${MIZUROUTE_DIR}/nr_utility.f90
                        ${MIZUROUTE_DIR}/ascii_util.f90
                        ${MIZUROUTE_DIR}/ncio_utils.f90
                        ${MIZUROUTE_DIR}/gamma_func.f90
                )
                SET(MIZUROUTE_DATATYPES
                        ${OPENWQ_INTERFACE_DIR}/openWQ.f90 # needs to be compiled before globalData
                        ${MIZUROUTE_DIR}/public_var.f90
                        ${MIZUROUTE_DIR}/dataTypes.f90
                        ${MIZUROUTE_DIR}/var_lookup.f90
                        ${MIZUROUTE_DIR}/time_utils.f90
                        ${MIZUROUTE_DIR}/datetime_data.f90
                        ${MIZUROUTE_DIR}/globalData.f90
                        ${MIZUROUTE_DIR}/popMetadat.f90
                        ${MIZUROUTE_DIR}/allocation.f90
                )
                SET(MIZUROUTE_INIT
                        ${MIZUROUTE_DIR}/network_topo.f90
                        ${MIZUROUTE_DIR}/process_param.f90
                        ${MIZUROUTE_DIR}/process_ntopo.f90
                        ${MIZUROUTE_DIR}/pfafstetter.f90
                        ${MIZUROUTE_DIR}/domain_decomposition.f90
                )
                SET(MIZUROUTE_IO
                        ${MIZUROUTE_DIR}/remap.f90
                        ${MIZUROUTE_DIR}/read_control.f90
                        ${MIZUROUTE_DIR}/read_param.f90
                        ${MIZUROUTE_DIR}/read_streamSeg.f90
                        ${MIZUROUTE_DIR}/write_streamSeg.f90
                        ${MIZUROUTE_DIR}/read_runoff.f90
                        ${MIZUROUTE_DIR}/get_basin_runoff.f90
                        ${MIZUROUTE_DIR}/read_remap.f90
                        ${MIZUROUTE_DIR}/read_restart.f90
                        ${MIZUROUTE_DIR}/write_restart.f90
                        ${MIZUROUTE_DIR}/write_simoutput.f90
                )
                SET(MIZUROUTE_CORE
                        ${MIZUROUTE_DIR}/model_finalize.f90
                        ${MIZUROUTE_DIR}/accum_runoff.f90
                        ${MIZUROUTE_DIR}/basinUH.f90
                        ${MIZUROUTE_DIR}/irf_route.f90
                        ${MIZUROUTE_DIR}/kwt_route.f90
                        ${MIZUROUTE_DIR}/dfw_route.f90
                        ${MIZUROUTE_DIR}/kw_route.f90
                        ${MIZUROUTE_DIR}/mc_route.f90
                        ${MIZUROUTE_DIR}/main_route.f90
                        ${MIZUROUTE_DIR}/model_setup.f90
                        ${OPENWQ_INTERFACE_DIR}/mizuroute_openwq.f90
                )
                SET(MIZUROUTE_DRIVER 
                        ${MIZUROUTE_DIR}/route_runoff.f90
                )
                # Stitch together MIZUROUTE programs
                SET(MIZUROUTE_MOD_ALL
                        ${MIZUROUTE_UTILS}
                        ${MIZUROUTE_DATATYPES}
                        ${MIZUROUTE_INIT}
                        ${MIZUROUTE_IO}
                        ${MIZUROUTE_CORE})

                SET(CMAKE_CXX_STANDARD 17)

                IF("${COMPILE_MODE}" STREQUAL "debug")
                        SET(CMAKE_Fortran_FLAGS "-g3 -Wall -fmax-errors=0 -fbacktrace -fcheck=all -ffpe-trap=zero -ffree-line-length-none -lstdc++")
                        SET(CMAKE_CXX_FLAGS "-g3 -O0 -Wall -pedantic")
                ELSEIF("${COMPILE_MODE}" STREQUAL "fast")
                        SET(CMAKE_Fortran_FLAGS "-O3 -fmax-errors=0 -ffree-line-length-none -lstdc++ -fopenmp")
                        SET(CMAKE_CXX_FLAGS "-O3 ${OpenMP_CXX_FLAGS}")
                        FIND_PACKAGE(OpenMP)
                ENDIF()

                FIND_PACKAGE(Armadillo REQUIRED) # don't forget to activate HDF5 support when installing Armadillo
                FIND_PACKAGE(HDF5 REQUIRED)
                FIND_PACKAGE(netCDF REQUIRED)
                FIND_PACKAGE(LAPACK REQUIRED)

                SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR})

                # compile openWQ into an object file
                ADD_LIBRARY(OPENWQ_OBJ OBJECT 
                        ${OPENWQ_SOURCES})
                TARGET_INCLUDE_DIRECTORIES(OPENWQ_OBJ PRIVATE
                        ${ARMADILLO_INCLUDE_DIR}
                        ${HDF5_INCLUDE_DIRS}
                        ${OpenMP_INCLUDE_DIR}
                        ${OPENWQ_DIR}/src/)
                TARGET_LINK_LIBRARIES(OPENWQ_OBJ PUBLIC
                        ${ARMADILLO_LIBRARIES}
                        ${HDF5_C_LIBRARY_hdf5})

                IF(OpenMP_CXX_FOUND)
                        MESSAGE("** OpenWQ **: OpenMP enabled!")
                        TARGET_LINK_LIBRARIES(
                                OPENWQ_OBJ PUBLIC 
                                OpenMP::OpenMP_CXX)
                ENDIF()

                # Create the executable with openWQ coupled to Mizuroute                 
                ADD_EXECUTABLE(${exec_name}
                        ${MIZUROUTE_MOD_ALL}
                        ${MIZUROUTE_DRIVER})
                SET_PROPERTY(TARGET ${exec_name} PROPERTY LINKER_LANGUAGE Fortran)
                TARGET_INCLUDE_DIRECTORIES(
                        ${exec_name} PUBLIC
                        ${LAPACK_INCLUDES}
                        ${netCDF_INCLUDES}
                        "/usr/include/"
                        "/code/")
                TARGET_LINK_LIBRARIES(
                        ${exec_name} PUBLIC
                        # "/usr/lib/x86_64-linux-gnu/libnetcdff.a"
                        "/usr/lib/aarch64-linux-gnu/libnetcdff.a"
                        ${LAPACK_LIBRARIES}
                        ${netCDF_LIBRARIES}
                        OPENWQ_OBJ)

        ELSE()

                # Apple and Windows not supported
                IF(UNIX AND APPLE)
                        MESSAGE(STATUS "** OpenWQ: current platform is MacOS")
                ELSE()
                        MESSAGE(STATUS "** OpenWQ: current platform is Windows")
                ENDIF()
                MESSAGE(STATUS "Not supported yet!")
                MESSAGE(STATUS "Please compile the code inside the Docker container!")
                MESSAGE(STATUS "See intructions in the CMakeLists.txt.")

        ENDIF()

# ..................
# IF TARGET = fluxos_openwq
# Only supports compilation in linux
# ..................
ELSEIF("${COMPILE_TARGET}" STREQUAL "fluxos_openwq")
SET(exec_name "${COMPILE_TARGET}_${COMPILE_MODE}")

        # Choose where to create the executable
        SET(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
        SET(EXEC_DIR ${PARENT_DIR}/bin/)

        IF(UNIX AND NOT APPLE)

                MESSAGE(STATUS "** OpenWQ ** current platform is Linux")

                # Declare use of multiple Languages
                PROJECT("${COMPILE_TARGET}")

                # Get paths to source code
                SET(FLUXOS_DIR ${PARENT_DIR}/src/fluxos)
                SET(OPENWQ_DIR ${PARENT_DIR}/src/openwq/openwq/src)
                SET(OPENWQ_INTERFACE_DIR ${PARENT_DIR}/src/openwq)

                # Setting MizuRoute sources
                FILE(GLOB SOURCES
                        ${FLUXOS_DIR}/*.cpp
                        ${OPENWQ_DIR}/*.cpp
                        ${OPENWQ_INTERFACE_DIR}/OpenWQ_hydrolink.cpp)

                LIST(REMOVE_ITEM SOURCES ${OPENWQ_INTERFACE_DIR}/main*.cpp)

                SET(CMAKE_CXX_STANDARD 17)

                IF("${COMPILE_MODE}" STREQUAL "debug")
                        SET(CMAKE_CXX_FLAGS "-g3 -O0 -Wall -pedantic")
                ELSEIF("${COMPILE_MODE}" STREQUAL "fast")
                        SET(CMAKE_CXX_FLAGS "-O3 ${OpenMP_CXX_FLAGS}")
                        FIND_PACKAGE(OpenMP)
                ENDIF()

                FIND_PACKAGE(Armadillo REQUIRED) # don't forget to activate HDF5 support when installing Armadillo
                FIND_PACKAGE(HDF5 REQUIRED)
                #FIND_PACKAGE(netCDF REQUIRED)
                #FIND_PACKAGE(LAPACK REQUIRED)

                SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR})

                # Create the executable with openWQ coupled to CRHM   
                ADD_EXECUTABLE(
                        ${exec_name}
                        ${SOURCES})

                TARGET_INCLUDE_DIRECTORIES(
                        ${exec_name} PUBLIC 
                        ${ARMADILLO_INCLUDE_DIR}
                        ${HDF5_INCLUDE_DIRS}
                        "/usr/include/"
                        "/code/")

                TARGET_LINK_LIBRARIES(
                        ${exec_name} PUBLIC
                        #"/usr/lib/x86_64-linux-gnu/libnetcdff.a"
                        "/usr/lib/aarch64-linux-gnu/libnetcdff.a"
                        ${ARMADILLO_LIBRARIES}
                        ${HDF5_C_LIBRARY_hdf5}
                        )
                
                IF(OpenMP_CXX_FOUND)
                        MESSAGE("** OpenWQ **: OpenMP enabled!")
                        TARGET_LINK_LIBRARIES(
                                ${exec_name} PUBLIC 
                                OpenMP::OpenMP_CXX)
                ENDIF()


        ELSE()

                # Apple and Windows not supported
                IF(UNIX AND APPLE)
                        MESSAGE(STATUS "** OpenWQ: current platform is MacOS")
                ELSE()
                        MESSAGE(STATUS "** OpenWQ: current platform is Windows")
                ENDIF()
                MESSAGE(STATUS "Not supported yet!")
                MESSAGE(STATUS "Please compile the code inside the Docker container!")
                MESSAGE(STATUS "See intructions in the CMakeLists.txt.")

        ENDIF()

# ..................
# IF TARGET = unknown
# ..................
ELSE()

        MESSAGE(STATUS "Compilation mode: Unkown")
        MESSAGE(STATUS "Double check the mode tag requested!")

ENDIF()
