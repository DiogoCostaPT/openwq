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
#include <tuple>
#include <vector>
#include "exprtk.hpp"


#include "jnlohmann/json.h"
using json = nlohmann::json;

/* #################################################
// Project JSON files
################################################# */
class OpenWQ_json
{
    public:

    json Master;
    json Config;
    json BGCcycling;
    json SinkSource;

};

/* #################################################
// Link: openWQ to Host Hydrological Model
################################################# */
class OpenWQ_hostModelconfig
{

    typedef std::tuple<int,std::string,int, int, int> hydroTuple;
    // Add host_hydrological_model compartment:
    // (1) int => index in openWQ 
    // (2) std::string => reference name in JSON file
    // (3) int => number of cells in x-direction
    // (4) int => number of cells in y-direction
    // (5) int => number of cells in z-direction
    
    public: 
    
    std::vector<hydroTuple> HydroComp;
    
    // Number of hydrological compartments (that can store and transport water)
    unsigned int num_HydroComp;

};

/* #################################################
// General information for openWQ about the project
################################################# */
class OpenWQ_wqconfig
{
    public:
    OpenWQ_wqconfig(){

    }
    OpenWQ_wqconfig(size_t num_coldata){

        this -> num_coldata = num_coldata;

        // #################################################
        // num_coldata is, the moment, equal to 11
        // 0 - chemical
        // 1 - compartment
        // 2 - source(=0) or sink(=1)
        // 3 - YYYY
        // 4 - MM
        // 5 - DD
        // 6 - HH
        // 7 - ix
        // 8 - iy
        // 9 - iz
        // 10 - value (already converted to mg/l (concentration) or g(mass)) 

        SinkSource_FORC = 
            std::unique_ptr<
                arma::Mat<double>>
            ( new  arma::mat(1,num_coldata));
        
            
    }

    size_t num_coldata;

    // #################################################
    // General set up
    double timetep_out;             // time step
    std::string timestep_out_unit;  // time step unit
    double nexttime_out = 0.0f;     // iteractive next printing time
    
    // #################################################
    // Chemistry
    unsigned int num_chem;                  //Number of chemical species  
    std::vector
        <std::string> chem_species_list;    // Chemical species list

    // BGC kinetic formulas (tuple with all the info needed)
    // It includes also the formulas parsed and ready to be used
    // for each BGC cyle provided by the user
    typedef exprtk::expression<double> expression_t;
    std::vector<
        std::tuple<
            std::string,                // Biogeochemical cycle name
            std::string,                // Transformation name
            std::string,                // kinetic equation provided
            unsigned int,               // index of consumed species       
            unsigned int,               // index of produced species
            std::vector<unsigned int>   // index of chemical in transformation equation (needs to be here for loop reset)
        >> BGCexpressions_info;
    
    std::vector<
        exprtk::expression<double>     // Expression (exprtk) parsed
        >BGCexpressions_eq;            // BGC kinetic formulas for all biogeochemical cycles
    
    std::vector<double> chemass_InTransfEq; // chemical mass involved in transformation (needs to be here for loop reset)

    // #################################################
    // Sink anhd Source
    std::unique_ptr<
        arma::Mat<double>
        > SinkSource_FORC;

};

/* #################################################
// Main openWQ data structures
################################################# */
class OpenWQ_vars
{
    public:
    OpenWQ_vars(){

    }
    OpenWQ_vars(size_t num_HydroComp){

        this-> num_HydroComp = num_HydroComp;

        try{

            // Units of chemass are in: g (grams)
            // Thus, concentrations are in mg/l (or g/m3) and volume in m3
            chemass = std::unique_ptr<
                arma::field< // Compartments
                arma::field< // Chemical Species
                arma::Cube<  // Dimensions: nx, ny, nz
                double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp)); 

        }catch(const std::exception& e){
            std::cout << 
                "ERROR: An exception occured during memory allocation (openWQ_global.h)" 
                << std::endl;
            exit (EXIT_FAILURE);
        }

    }
    size_t num_HydroComp;

    std::unique_ptr<arma::field<arma::field<arma::Cube<double>>>> chemass; 
    
};



#endif