cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
# FLAGS_NOAH = -O3 -ffree-form -ffree-line-length-none -fmax-errors=0 $(FLAGS_OMP)
# FLAGS_COMM = -O3 -ffree-line-length-none -fmax-errors=0 $(FLAGS_OMP)
# FLAGS_SUMMA = -O3 -ffree-line-length-none -fmax-errors=0 $(FLAGS_OMP)

# Debug runs
#FLAGS_NOAH = -p -g -ffree-form -ffree-line-length-none -fmax-errors=0 -fbacktrace -Wno-unused -Wno-unused-dummy-argument
#FLAGS_COMM = -p -g -Wall -ffree-line-length-none -fmax-errors=0 -fbacktrace -fcheck=bounds
#FLAGS_SUMMA = -p -g -Wall -ffree-line-length-none -fmax-errors=0 -fbacktrace -fcheck=bounds


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

        file(GLOB SOURCES "src/*.cpp")
        set(OPENWQ_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

        set(OPENWQ_FILES 
        ${OPENWQ_DIR}/OpenWQ_hydrolink.cpp)
        ############################################################################
        ## File definitions for subcomponents - For Summa
        ############################################################################
        set(DRIVER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/summa/build/source/driver)
        set(HOOKUP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/summa/build/source/hookup)
        set(NETCDF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/summa/build/source/netcdf)
        set(DSHARE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/summa/build/source/dshare)
        set(NOAHMP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/summa/build/source/noah-mp)
        set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/summa/build/source/engine)
        set(INTERFACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/summa/build/source/interface)

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
        find_package(netCDF REQUIRED)
        find_package(LAPACK REQUIRED)

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

          # compile openWQ into an object file
          add_library(openWQ OBJECT 
          ${SOURCES})
          target_compile_options(openWQ PRIVATE
                  -g3 -Wall -pedantic)
          target_include_directories(openWQ PRIVATE
                  ${ARMADILLO_INCLUDE_DIR}
                  ${VTK_INCLUDE_DIR}
                  ${OpenMP_INCLUDE_DIR})
          target_link_libraries(openWQ PUBLIC
                  ${ARMADILLO_LIBRARIES}
                  ${VTK_LIBRARIES}
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
                        "/u1/kck540/openWQ/openwq/build/")
                target_link_libraries(SUMMA_COMM PUBLIC
                        "/usr/lib/x86_64-linux-gnu/libnetcdff.a"
                        SUMMA_NOAHMP)

         # Create the executable with openWQ coupled to Summa                 
        add_executable(summa
                ${SUMMA_ALL}
                ${DRIVER})
                set_property(TARGET summa PROPERTY LINKER_LANGUAGE Fortran)
                target_include_directories(
                        summa PUBLIC
                        ${LAPACK_INCLUDES}
                        ${netCDF_INCLUDES}
                        "/usr/include/"
                        "/u1/kck540/openWQ/openwq/build/")
                target_link_libraries(summa PUBLIC
                        "/usr/lib/x86_64-linux-gnu/libnetcdff.a"
                        ${LAPACK_LIBRARIES}
                        ${netCDF_LIBRARIES}
                        SUMMA_COMM
                        SUMMA_NOAHMP
                        openWQ)

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
                "I:/FLUXOS_cpp/Armadillo/include" }
                ${VTK_INCLUDE_DIR})

        target_link_libraries(
                openwq
                ${VTK_LIBRARIES})

ENDIF()
