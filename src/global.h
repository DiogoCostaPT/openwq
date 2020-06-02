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
class JSONfiles
{
    public:

    json Master;
    json H2O;
    json BGC;

};


// Porject general info
class Prj_GenInfo
{
    public:

    std::string project_name;
    std::string geographical_location;
    std::string authors;
    std::string date;
    std::string additional_info;
    std::string water_balance_setup_file;
    std::string biogeochemistry_setup_file;

};

// Project general dimensions
class Prj_Dim
{
    public:

    int num_compartments;
    int num_chemical_species;

};

// General information about the project
class Prj_StateVar
{
    public:
    Prj_StateVar(){

    }
    Prj_StateVar(size_t numcmp,size_t numspec){

        this-> numcmp = numcmp;
        this-> numspec = numspec;

        try{
            char cmpt_names[numcmp]; // compartment names
            char spec_names[numspec]; // species names

            wflux = std::unique_ptr<arma::field<arma::Mat<double>>>(new arma::field<arma::mat>(numcmp,numspec));
            wmass = std::unique_ptr<arma::field<arma::Mat<double>>>(new arma::field<arma::mat>(numcmp,numspec));
            chemass = std::unique_ptr<arma::field<arma::Mat<double>>>(new arma::field<arma::mat>(numcmp,numspec));

        }catch(int e){
            std::cout << "An exception occurred creating the domain: ERR " << std::to_string(e) << std::endl;
        }

    }
    size_t numcmp, numspec;

    std::unique_ptr<arma::field<arma::Mat<double>>> wflux, wmass, chemass; 

};

#endif