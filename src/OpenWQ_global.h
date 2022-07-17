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
#include <omp.h>


#include "jnlohmann/json.hpp"
using json = nlohmann::json;

/* #################################################
// Project JSON files
################################################# */
class OpenWQ_json
{
    public:

    json Master;
    json Config;
    json SinkSource;
    json TE_module;     // Transport and Erosion
    json BGC_module;    // Biogeochemistry

};

/* #################################################
// Link: openWQ to Host Hydrological Model
################################################# */
class OpenWQ_hostModelconfig
{
    public:
    
    // Host model characterization via tuple
    typedef std::tuple<int,std::string,int, int, int> hydroTuple;
    // Add host_hydrological_model compartment:
    // (1) int => index in openWQ 
    // (2) std::string => reference name in JSON file
    // (3) int => number of cells in x-direction
    // (4) int => number of cells in y-direction
    // (5) int => number of cells in z-direction

    OpenWQ_hostModelconfig(){

        waterVol_hydromodel = std::unique_ptr<
            std::vector<
            arma::Cube<
            double>>>(new std::vector<arma::cube>); 

        SM = std::unique_ptr<
            arma::Cube<
            double>>(new arma::cube); 

        Tair = std::unique_ptr<
            arma::Cube<
            double>>(new arma::cube); 

        Tsoil = std::unique_ptr<
            arma::Cube<
            double>>(new arma::cube); 
    }

    std::vector<hydroTuple> HydroComp;

    // Host model iteraction step (dynamic value)
    long interaction_step = 0;

    // Host model time step (in seconds)
    long time_step;

    // Number of hydrological compartments (that can store and transport water)
    unsigned int num_HydroComp;

    // Stores water fluxes when concentration are requested for outputs
    std::unique_ptr<std::vector<arma::Cube<double>>> waterVol_hydromodel;
    
    // Water volume minimum limit (critical for concentration calculations)
    // to avoid concentration instabilities and numerical blowup
    // uses native units: m3
    const double watervol_minlim = 0.01;

    // Add dependencies for BGC calculations
    std::unique_ptr<arma::Cube<double>> SM;    // Saves all SM data from hostmodel
    std::unique_ptr<arma::Cube<double>> Tair;  // Saves all Tair data from hostmodel
    std::unique_ptr<arma::Cube<double>> Tsoil;  // Saves all Tsoil data from hostmodel
    double SM_txyz;                                                  // Used as iteractive variable to use with exprtk
    double Tair_txyz;                                                // Used as iteractive variable to use with exprtk
    double Tsoil_txyz;                                                // Used as iteractive variable to use with exprtk
    
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
        // Since and source forcing
        
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
        // 11 - flag to tell if already used (1) or not (0) because models can run 
                // timesteps smaller than the specified load and that would cause the 
                // load to be added multiple times
 
        SinkSource_FORC = 
            std::unique_ptr<
                arma::Mat<double>>
            ( new  arma::mat(0,num_coldata));

    }

    size_t num_coldata;

    // Master file location
    std::string OpenWQ_masterjson;

    // #################################################
    // General
    double time_previous;           // previous time (in seconds) for calculation of dynamic
                                    // timesteps that critical for calculation of transformation rates
    std::string LogFile_name = "Log_OpenWQ.txt";       // name of log file

    // #################################################
    // Computation settings
    int num_threads_system;       // number of threads in the system
    int num_threads_requested;    // number of threads requested by user

    // #################################################
    // Sink anhd Source
    std::unique_ptr<
        arma::Mat<double>
        > SinkSource_FORC;

    // #################################################
    // Output 
    
    // Time
    double timetep_out;             // time step (in seconds)
    std::string timestep_out_unit;  // time step unit
    double nexttime_out = 0.0f;     // iteractive next printing time (in seconds)
    // output format
    unsigned long output_type;      // 1) CSV, 2) VTK, 3) HDF5
    bool debug_mode = false;        // set to true if debug mode is requested
    std::tuple<
        std::string,            // output units as provided by the user
        double,                 // numerator multiplier (determined by Convert_Units)
        double,                 // denominator multiplier (determined by Convert_Units)
        bool                    // flad if requested concentration (needs to be divided by water volume)
        > output_units;         // Tuple with info about output units
    // chemicals, compartments and cells/elements to export
    std::vector<int> chem2print;
    std::vector<int> compt2print;
    std::vector<bool> cells2print_bool;
    std::vector<arma::mat> cells2print_vec;
    
    // Output folder
    std::string output_dir;

    // No water concentration (as a marker/flag)
    int noWaterConc = -9999; // setting a default value

    // Flag for printing coordinates once
    bool print_oneStep = true;

    // Error message flags
    bool readSet_print_errmsg = true;
    bool BGC_Transform_print_errmsg = true;
    bool invalid_bgc_entry_errmsg = true;

    // ########################################
    // MODULES
    // ########################################

    // ##########################
    // 1) Transport / Erosion (TE)
    // ##########################

    // ##########################
    // General info 
    // (needed for all TE modules, native and not native)
    std::string TE_module;  // Get module name

    // ##########################
    // OpenWQ native module: OPENWQ_NATIVE_TE
    std::vector<double> OpenWQ_TE_native_IntMob_Erodib_K;

    std::vector
        <std::tuple
            <unsigned int,unsigned int,unsigned int,double>> 
            OpenWQ_TE_native_BoundMix_info;

    // ##########################
    // 1) Biogeochemistry
    // ##########################
    
    // ##########################
    // General info 
    // (needed for all BGC modules, native and not native)
    std::string BGC_module;     // Get module name

    unsigned int BGC_general_num_chem;                  //Number of chemical species  
    std::vector
        <std::string> BGC_general_chem_species_list;    // Chemical species list
    std::vector
        <unsigned int> BGC_general_mobile_species;    // index of mobile chem species

    // ##########################
    // OpenWQ native module: OPENWQ_NATIVE_BGC
    
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
        >> openWQ_BGCnative_BGCexpressions_info;
    
    std::vector<
        exprtk::expression<double>                      // Expression (exprtk) parsed
        >openWQ_BGCnative_BGCexpressions_eq;            // BGC kinetic formulas for all biogeochemical cycles
    
    std::vector<double> openWQ_BGCnative_chemass_InTransfEq; // chemical mass involved in transformation (needs to be here for loop reset)
    
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
            
            
            // ############################################
            // Mass changes (for solver separation and DEBUG mode activation)
            // ############################################


            // ############################################
            // Derivatives

            // Derivative (chemistry)
            d_chemass_dt_chem = std::unique_ptr<
                arma::field< // Compartments
                arma::field< // Chemical Species
                arma::Cube<  // Dimensions: nx, ny, nz
                double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));

            // Derivative (water transport)
            d_chemass_dt_transp = std::unique_ptr<
                arma::field< // Compartments
                arma::field< // Chemical Species
                arma::Cube<  // Dimensions: nx, ny, nz
                double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));


            // ############################################
            // Single time, constant or isolated changes 
            // to state-variable

            // IC (single time change at start of simulation)
            d_chemass_ic = std::unique_ptr<
                arma::field< // Compartments
                arma::field< // Chemical Species
                arma::Cube<  // Dimensions: nx, ny, nz
                double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));
    
            // Derivative (sink and source)
            d_chemass_ss = std::unique_ptr<
                arma::field< // Compartments
                arma::field< // Chemical Species
                arma::Cube<  // Dimensions: nx, ny, nz
                double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));


             // ############################################
            // Cumulative Mass changes 
            // Cumulating mass changes until next output is printed
            // Needed for Debug mode
            // ############################################


            // ############################################
            // Derivatives

            // Derivative (chemistry)
            d_chemass_dt_chem_out = std::unique_ptr<
                arma::field< // Compartments
                arma::field< // Chemical Species
                arma::Cube<  // Dimensions: nx, ny, nz
                double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));

            // Derivative (water transport)
            d_chemass_dt_transp_out = std::unique_ptr<
                arma::field< // Compartments
                arma::field< // Chemical Species
                arma::Cube<  // Dimensions: nx, ny, nz
                double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));


            // ############################################
            // Single time, constant or isolated changes 
            // to state-variable

            // Derivative (sink and source)
            d_chemass_ss_out = std::unique_ptr<
                arma::field< // Compartments
                arma::field< // Chemical Species
                arma::Cube<  // Dimensions: nx, ny, nz
                double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));
            

        }catch(const std::exception& e){

            // Write Error in Console only (the program hasn't started yet)
            std::cout << 
                "ERROR: An exception occured during memory allocation (openWQ_global.h)" 
                << std::endl;
            exit (EXIT_FAILURE);

        }

    }
    size_t num_HydroComp;

    std::unique_ptr<arma::field<arma::field<arma::Cube<double>>>> 
        chemass,                    // state-variable
        d_chemass_dt_chem,          // derivative (chemistry)
        d_chemass_dt_transp,        // derivative (transport)
        d_chemass_ss,               // derivative (isolated) SS
        d_chemass_ic,               // derivative (at start) IC
        d_chemass_dt_chem_out,      // cumulative derivative for output in ddebug model
        d_chemass_dt_transp_out,    // cumulative derivative for output in ddebug model
        d_chemass_ss_out;           // cumulative derivative for output in ddebug model

};



#endif