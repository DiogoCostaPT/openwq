# Instructions
# 1) docker pull amd64/ubuntu (using ubuntu will allow us to use apt-get)
# 2) docker build . -t openwq_stdl_amd64_ubuntu
# 3) Run OpenWQ:
# Main development environment is Ubuntu
FROM ubuntu
WORKDIR /code
# Update ubuntu's apt list
RUN apt-get -y update && apt-get install -y
# Install g++
RUN apt-get -y install g++
# Install cmake
RUN apt-get -y install cmake
######################################
# Armadillo
#####################################
# Install armadillo and its dependencies
RUN apt-get -y install liblapack-dev
RUN apt-get -y install libblas-dev
RUN apt-get -y install libopenblas-dev
RUN apt-get -y install libboost-dev
RUN apt-get install -y libglvnd-dev
# Get Armadillo (get 10.8.2 version, but this can be changed)
# RUN apt-get -y install libarmadillo-dev # very old version in aptitude (not suitable)
RUN apt-get install wget
RUN wget http://sourceforge.net/projects/arma/files/armadillo-10.8.2.tar.xz
RUN apt install xz-utils
RUN tar -xf armadillo-10.8.2.tar.xz
WORKDIR /code/armadillo-10.8.2
RUN cmake .
RUN make install
######################################
# VTK
#####################################
# Download and install VTK dependencies
RUN apt-get -y install python3
RUN apt-get install -y libosmesa6-dev
RUN apt-get -y install build-essential
RUN apt-get -y install mesa-common-dev
RUN apt-get -y install mesa-utils
RUN apt-get -y install freeglut3-dev
RUN apt-get -y install ninja-build
RUN apt-get -y install qt5-default
# Download and install VTK
RUN apt-get -y install git
WORKDIR /code
RUN git clone --recursive https://gitlab.kitware.com/vtk/vtk.git
RUN mkdir -p vtk/build
WORKDIR /code/vtk/build
# Ninja (instead of make) for speedier build (important for VTK)
RUN cmake -GNinja ..
RUN ninja install
######################################
# Copy local folders to image
#####################################
# Copy the OpenWQ files to the container
WORKDIR /code
COPY . /code
RUN mkdir -p /code/build/

## Install Dependencies for SUMMA
RUN apt-get update && \
    DEBIAN_FRONTEND="noninteractive" apt-get install -y software-properties-common \
    libnetcdf-dev \
    libnetcdff-dev \
    liblapack-dev \
    gfortran
# Compile OpenWQ
