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
#include <armadillo>
#include <memory> 
#include "jnlohmann/json.h"
using json = nlohmann::json;


// Project Information
class DEMOS_OpenWQ_json
{
    public:

    json Master;
    json H2O;
    json CMPI;
    json WQ;
    json BGC;

};


// General information about the project
class DEMOS_OpenWQ_vars
{
    public:
    DEMOS_OpenWQ_vars(){

    }
    DEMOS_OpenWQ_vars(size_t numcmp, size_t numinteract){

        this-> numcmp = numcmp;
        this-> numinteract = numinteract;

        try{

            // wmass = std::unique_ptr<arma::field<arma::Cube<double>>>(new arma::field<arma::cube>(numcmp)); // 1 field: water mass
            // wflux = std::unique_ptr<arma::field<arma::field<arma::Cube<double>>>>(new arma::field<arma::field<arma::cube>>(numcmp)); // 1 field: flow within compartment
            // wchem_exch = std::unique_ptr<arma::field<arma::Mat<double>>>(new arma::field<arma::mat>(numinteract)); // 1 field: flow between compartments
            chemass = std::unique_ptr<arma::field<arma::field<arma::Cube<double>>>>(new arma::field<arma::field<arma::cube>>(numcmp));  // multiple fields: one for eacg chem
        
        }catch(int e){
            std::cout << "An exception occurred creating the domain: ERR " << std::to_string(e) << std::endl;
        }

    }
    size_t numcmp, numinteract;

    //std::unique_ptr<arma::field<arma::Cube<double>>>  wmass;
    //std::unique_ptr<arma::field<arma::Mat<double>>> wchem_exch;
    //std::unique_ptr<arma::field<arma::field<arma::Cube<double>>>> wflux
    std::unique_ptr<arma::field<arma::field<arma::Cube<double>>>> chemass; 

};

#endif