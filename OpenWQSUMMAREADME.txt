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