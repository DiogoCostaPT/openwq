// Copyright 2020, Diogo Costa
// This file is part of OpenWQ model.

// This program, openWQ, is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) aNCOLS later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GLOBALH_INCLUDED
#define GLOBALH_INCLUDED

#include <string>
#include<armadillo>
#include<memory> 

// General information about the project
class ClassGenProj
{
   

    public:

    struct geninfo {
        std::string project_name;
        std::string geographical_location;
        std::string authors;
        std::string date;
        std::string additional_info;
    } geninfo;

    struct size {
        int number_of_compartments;
        int number_of_chemical_species;
        int dim_nx;
        int dim_ny;
        int dim_nz;
    } size;

};


// General information about the project
class ClassDetProj
{
    

    public:
    ClassDetProj(){

    }
    ClassDetProj(size_t numcmp,size_t numspec, size_t nx, size_t ny, size_t nz){

        this-> numcmp = numcmp;
        this-> numspec = numspec;
        this-> nx = nx;
        this-> ny = ny;
        this-> nz = nz;

        try{
            char cmpt_names[numcmp]; // compartment names
            char spec_names[numspec]; // species names

            domain_xyz = std::unique_ptr<arma::Cube<double>>(new arma::cube(nx,ny,nz));

            wflux = std::unique_ptr<arma::field<arma::Mat<double>>>(new arma::field<arma::mat>(numcmp,numspec));
            wmass = std::unique_ptr<arma::field<arma::Mat<double>>>(new arma::field<arma::mat>(numcmp,numspec));
            chemass = std::unique_ptr<arma::field<arma::Mat<double>>>(new arma::field<arma::mat>(numcmp,numspec));

        }catch(int e){
            std::cout << "An exception occurred creating the domain: ERR " << std::to_string(e) << std::endl;
        }

    }
    size_t numcmp, numspec, nx, ny, nz;

    std::unique_ptr<arma::Cube<double>> domain_xyz;
    std::unique_ptr<arma::field<arma::Mat<double>>> wflux, wmass, chemass; 

};

#endif