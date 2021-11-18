FROM ubuntu


WORKDIR /code


# Summa Dependencies
RUN apt-get update && \
    DEBIAN_FRONTEND="noninteractive" apt-get install -y software-properties-common \
    libnetcdf-dev \
    libnetcdff-dev \
    liblapack-dev

# OpenWQ Dependencies

RUN apt update -y \
    && apt upgrade -y \
    && DEBIAN_FRONTEND="noninteractive" apt install -y \
         cmake \
         g++ \
         git \
         libssl-dev \
         make \
         gfortran \
         gdb \
    && apt-get autoclean

ADD . /code


