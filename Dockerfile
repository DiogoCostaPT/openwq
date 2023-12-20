
FROM ubuntu:20.04
WORKDIR /code

# Update package manager
RUN apt-get -y update && apt-get upgrade -y

# Install Dependencies and Develepment Tools         
RUN DEBIAN_FRONTEND="noninteractive" \
             apt-get -y install g++ \
                                gdb \
                                gdbserver \
                                cmake \
                                wget \
                                xz-utils \
                                liblapack-dev \
                                libblas-dev \
                                libopenblas-dev \
                                libboost-all-dev \
                                hdf5-tools \
                                software-properties-common \
                                libnetcdf-dev \
                                libnetcdff-dev \
                                liblapack-dev \
                                gfortran

# Install Armadillo in conventional linux directory (/opt)
WORKDIR /opt
RUN wget http://sourceforge.net/projects/arma/files/armadillo-10.3.0.tar.xz
RUN tar -xf armadillo-10.3.0.tar.xz
WORKDIR /opt/armadillo-10.3.0
RUN cmake . -D DETECT_HDF5=true -DCMAKE_C_FLAGS="-DH5_USE_110_API"
RUN make install
# Enable HDF5 Support
RUN sed -i '121s/^\/\/ #define ARMA_USE_HDF5/#define ARMA_USE_HDF5/' /usr/include/armadillo_bits/config.hpp
# Set Entry Point
WORKDIR /code
COPY . /code
