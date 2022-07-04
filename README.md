# Compiling SUMMA-OpenWQ
The best way to compile SUMMA-OpenWQ is with the docker-compose.yml file.
Steps:
 - Make the directory that contains docker-compose.yml your current working directory.
 - run `docker compose up`.
 - Once the container is running, connect to the container.
 - Inside the container enable HDF5 in armadillo by uncommenting the line that contains `#define ARMA_USE_HDF5` in the file `/usr/include/armadillo_bits/config.hpp`.
 - Then if a build directory does not exist in the directory that contains CMakeList.txt, create one with `mkdir build` and cd into it.
 - Run `cmake ..`
 - Run `make`
 - The above should create an executable `summa_openwq` in the directory /apply/

# Running SUMMA-OpenWQ
The Celia test case is included for testing SUMMA-OpenWQ. It is located in the `test_cases` folder. The file structure should be set up for running inside the docker container and require no modification.

To Run:
 - ./summa_openwq -g 1 1 -m /code/test_cases/celia1990/SUMMA/summa_fileManager_celia1990.txt




