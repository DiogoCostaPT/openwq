Compiling With Docker:
 - The docker file should be good as is. The docker-compose.yml file will need some slight modification.
 - The bottom two volumes will need to be removed if you are just compiling. They are needed to attach the data for SUMMA when running.
 - Once this is done starting the dockerfile is done :
    docker compose up
    older versions of docker may require docker-compose up

- Once the docker container is running you have to connect to it. Once connected make the directory build/ and cd into it.
- Once in the build directory perform the following commands:
    - cmake ..
    - make

The interface for openWQ functions are located in the interface folder in 
the summa source code. There are two files openWQ.f90 and openWQInterface.f90.

The location of calls to openWQ are located in the folllowing:
    - OpenWQ::decl is called from summa_init.f90 (near the bottom of the file)
    - OpenWQ::run_time_start is called in summa_driver.f90 before summa_runPhysics
    around lines 95-100.
    - OpenWQ::run_space will be called within the model simulation, TBD
    - OpenWQ::run_time_start will be called in summa_driver.f90 after 
    summa_runPhysics, around lines 95-105;


Notes on the interface:
    - Creating seperate function calls on the fortran side is not possible 
    due to the fact that each call takes exaclty the same parameters. Therefore 
    the interface function for running openWQ takes an integer argument to denote
    which OpenWQ method to call. 


Interface Functions:
    - These functions are located in summa/interface



Program Trace:
  - Program starts in Summa with the first call to openWQ coming in summa_init.f90 (NEAR END OF FILE)
    - Here is where we initalize the openWQ object in fortran
        - This is a c++ object that Fortran holds a reference for.
    - Summa then calls decl()



Derivatives of Summa are calculated in updateVars.f90 which is called by eval8summa.
After updateVars we call computFlux, which is where the fluxes get computed.
updateProg() is when the state variables are updated.


Main Solver:
    CoupledEm -> opSplittin -> varSubstep -> systemSolv -> updateProg
                                              -> eval8Summa(compute the flux and the residual vector for given state vector)
                                              -> summaSolve (can call eval8Summa for updated state vector)

    eval8Summa -> updateVars(Derivatives) -> computeFlux