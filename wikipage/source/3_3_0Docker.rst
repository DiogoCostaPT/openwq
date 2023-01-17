Docker
==================================

**Install Docker**

Install Docker in your machine.

**Create the Docker image and container**
(only needs to be set up once)

1. Start Docker
2. Go to the main OpenWQ folder that contains the files ``docker-compose.yml`` and ```Dockerfile``
3. If needed, clear all images and containers with ``docker system prune -a``
4. Check if ``docker-compose.yml`` file is good with ``docker-compose config``
5. Create the Docker image (set up for Ubuntu:20.04) that contains all the necessary packages with ``docker compose up -d``
6. If you need to stop the container: ``docker compose down``

**Compile your coupled OpenWQ model**

1. Go to the main OpenWQ folder that contains the ``CMakeLists.txt`` file
2. Inside the ``CMakeLists.txt`` file, set up the compilation target: ``SET(COMPILE_TARGET XXXX)``
3. Inside the ``CMakeLists.txt`` file, set up the compilation mode: ``SET(COMPILE_MODE XXXX)``
4. Open terminal and go to the OpenWQ folder that contains the ``CMakeLists.txt``
5. Check the containers that are running with ``docker ps`` (exports container IDs)
6. Get the appropriate container ID and use it in the command below
7. Run a shell inside the container with ``docker exec -it <container id> /bin/bash``
8. Armadillo now disables default hdf5 support for save/load, so inside the container you need to go to “/usr/include/armadillo_bits/config.hpp” and uncomment #define ARMA_USE_HDF5
9. Go to the main OpenWQ folder that contains the ``CMakeLists.txt`` file
10. Compile with: ``rm CMakeCache.txt; rm -r CMakeFiles; cmake . ; make -B; rm *.mod``

That's all !! :)