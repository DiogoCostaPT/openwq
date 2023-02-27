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
    json ExtWatFlux;
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
    
    // ########################
    // Host model COMPARTMENT or EXTERNAL FLUX characterization via tuple
    typedef std::tuple<int,std::string,int, int, int> hydroTuple;
    // Add host_hydrological_model compartment:
    // (1) int => index in openWQ 
    // (2) std::string => reference name in JSON file
    // (3) int => number of cells in x-direction
    // (4) int => number of cells in y-direction
    // (5) int => number of cells in z-direction
    // ########################

    OpenWQ_hostModelconfig(){

        // Water volumes from hostmodel
        waterVol_hydromodel = std::unique_ptr<
            std::vector<                            // compartments
            arma::Cube<                             // ix, iy, iz
            double>>>(new std::vector<arma::cube>); 

        // Dependencies from hostmodel 
        // (to be available for BGC)
        dependVar = std::unique_ptr<
            std::vector<                            // dependency variable
            arma::Cube<                             // ix, iy, iz
            double>>>(new std::vector<arma::cube>);

        dependVar_scalar = std::unique_ptr<
            std::vector<
            double>>(new std::vector<double>);

    }

    // Vectors with characterization of the different 
    // model compartments and external fluxes
    std::vector<hydroTuple> HydroComp;
    std::vector<hydroTuple> HydroExtFlux;
    std::vector<hydroTuple> HydroDepend;

    // Compartment and external water fluxes names
    std::vector<std::string> cmpt_names;    // names of comparments fluxes
    std::vector<std::string> ewf_names;     // names of external water fluxes
    std::vector<std::string> depend_names;  // names of external water fluxes

    // Number of hydrological compartments and EWF
    // (that can store and transport water)
    unsigned int num_HydroComp;
    unsigned int num_EWF;
    unsigned int num_Depend;

    // Host model iteraction step (dynamic value)
    long interaction_step = 0;

    // Host model time step (in seconds)
    long time_step;

    // Stores water fluxes when concentration are requested for outputs
    std::unique_ptr<std::vector<arma::Cube<double>>> waterVol_hydromodel;

    // To store all dependency variables 
    // to be available for BGC calculations and expressionss
    std::unique_ptr<std::vector<arma::Cube<double>>> dependVar; // global data
    std::unique_ptr<std::vector<double>> dependVar_scalar;      // data updated for use with exprtk

    
    // Water volume minimum limit (critical for concentration calculations)
    // to avoid concentration instabilities and numerical blowup
    // uses native units: m3
    const double watervol_minlim = 1;

};

/* #################################################
// General information for openWQ about the project
################################################# */
class OpenWQ_wqconfig
{
    public:
    OpenWQ_wqconfig(){

        

        // #################################################
        // Compiling and re-structuring of input data for quicker access during runtime
        // Source and source forcing (SinkSource_FORC)
        // AND
        // External fluxes (ExtFlux_FORC_jsonAscii)

        this -> num_coldata_jsonAscii = 20;
        // num_coldata_jsonAscii is, the moment, equal to 20
        // 0 - chemical
        // 1 - compartment id (from HydroComp) / external flux id (from HydroExtFlux)
        // 2 - source(=0) or sink(=1)
        // 3 - YYYY
        // 4 - MM
        // 5 - DD
        // 6 - HH
        // 7 - MIN
        // 8 - SEC
        // 9 - ix
        // 10 - iy
        // 11 - iz
        // 12 - value (already converted to mg/l (concentration) or g(mass))
        // 13 - load scheme (0) not applicable, (1) discrete or (2) continuous
        // 14, 15 ,16, 17, 18, 19 - flag to deal with "ALL" entries in YYYY, MM, DD, HH, MIN, SEC
            // if there are no "all"s, then it's to use one time only and 
            // and it is set to -1, which after use becomes -2 for not use again
            // otherwise, it gets updated everytime the load is added
            // and it provides the time increment for the next load

        this -> num_coldata_h5 = 4;
        // 1 - ix
        // 2 - iy
        // 3 - iz
        // 4 - value (already converted to mg/l (concentration) or g(mass))

        // Sink and source forcing
        SinkSource_FORC = 
            std::unique_ptr<
            arma::Mat<double>>
            (new  arma::mat(0,num_coldata_jsonAscii));

        // External fluxes forcing (JSON or ASCII datatypes)
        ExtFlux_FORC_jsonAscii = 
            std::unique_ptr<
            arma::Mat<double>>
            (new  arma::mat(0,num_coldata_jsonAscii));
        
        // External fluxes forcing (HDF5) 
        // Storing timestamps as time_t
        ExtFlux_FORC_HDF5vec_time =
            std::unique_ptr<        // EWF-h5 json block/request
            std::vector<            
            std::vector<time_t>>>
            (new  std::vector<std::vector<time_t>>);
        // Storing external_flux id
        ExtFlux_FORC_HDF5vec_ewfCompID =
            std::unique_ptr<
            std::vector<unsigned int>>
            (new  std::vector<unsigned int>);
        // Saving 1 timestep
        ExtFlux_FORC_data_tStep = 
            std::unique_ptr<
            arma::Cube<double>>
            (new  arma::cube);
        // Storing all timesteps
        ExtFlux_FORC_HDF5vec_data = 
            std::unique_ptr<
            std::vector<           // EWF-h5 json block/request
            std::vector<           // ChemSpecies 
            std::vector<           // timestamps
            arma::Cube<double>>>>>
            (new  std::vector<std::vector<std::vector<arma::cube>>>);

    }

    size_t num_coldata_jsonAscii;
    size_t num_coldata_h5;

    // General JSON key null error
    std::string jsonKeyNull_msg_start_abort = "<OpenWQ> Execution ABORTED!\nExpected json value for key=";
    std::string jsonKeyNull_msg_end_abort = " but not found! Revise the JSON files.";
    std::string jsonKeyNull_msg_start_NOabort = "<OpenWQ> WARNING: Expected json value for key=";
    std::string jsonKeyNull_msg_end_NOabort = " but not found! The entry has been zeroed. Make sure this was intended!";
    
    // Master file location
    std::string OpenWQ_masterjson;

    // #################################################
    // General
    double time_previous;           // previous time (in seconds) for calculation of dynamic
                                    // timesteps that critical for calculation of transformation rates
    std::string LogFile_name = "Log_OpenWQ.txt";       // name of log file
    std::string LogFile_name_fullpath;
    // #################################################
    // Computation settings
    int num_threads_system;         // number of threads in the system
    int num_threads_requested;      // number of threads requested by user
    int num_threads_default = 4;    // if requested num threads is invalid, defaults to this value 

    // #################################################
    // Sink and Source AND External fluxes
    std::unique_ptr<            
        arma::Mat<double>
        > SinkSource_FORC;              // SS
    std::unique_ptr<            
        arma::Mat<double>
        > ExtFlux_FORC_jsonAscii;       // External fluxes (JSON and ASCII)
    std::unique_ptr<
        std::vector<       
        std::vector<time_t>
        >> ExtFlux_FORC_HDF5vec_time;   // External fluxes HDF5 vector (timestamps as time_t)
    std::unique_ptr<                    // EWF compartment id
        std::vector<unsigned int>
        > ExtFlux_FORC_HDF5vec_ewfCompID;
    std::unique_ptr<            
        arma::Cube<double>
        > ExtFlux_FORC_data_tStep;      // External fluxes HDF5 vector (one timestep)
    std::unique_ptr<
        std::vector<                    // JSON-h5-EWF request (blocks)   
        std::vector<                    // Chemical species
        std::vector<                    // Time steps
        arma::Cube<double>
        >>>> ExtFlux_FORC_HDF5vec_data;   // External fluxes HDF5 vector (data)

    std::string h5EWF_interpMethod;     // interpolation method for h5 EWF 
    int allSS_flag = -1;                // number to replace in SinkSource_FORC to denote "all"
    bool tstep1_flag = true;            // flag to note that it's the first time step, so need to exclude loads prior to that

    // #################################################

    // Time
    double timetep_out;             // time step (in seconds)
    std::string timestep_out_unit;  // time step unit
    double nexttime_out = 0.0f;     // iteractive next printing time (in seconds)
    // output format
    unsigned long output_type;      // 1) CSV, 2) HDF5
    bool debug_mode = false;        // set to true if debug mode is requested
    std::tuple<
        std::string,                // output units as provided by the user
        double,                     // numerator multiplier (determined by Convert_Units)
        double,                     // denominator multiplier (determined by Convert_Units)
        bool                        // flad if requested concentration (needs to be divided by water volume)
        > output_units;             // Tuple with info about output units
    // chemicals, compartments and cells/elements to export
    std::vector<int> chem2print;
    std::vector<int> compt2print;
    std::vector<bool> cells2print_bool;
    std::vector<arma::mat> cells2print_vec;
    // Since the unix time epoch is 1970, which is used as a reference for timegm,
    // the seconds become negative for years below 1970, 
    // which will mess up time management.
    // Thus, the number of seconds since 00:00 1 Jan 1970 GMT, 
    // which is 2,208,988,800, is added 
    // (which is saved in OpenWQ_vars.secSinceUnixTimeEpoch).
    unsigned long long secFrom1900toUnixTimeEpoch1970 = 2208988800;
    
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
        <unsigned int> BGC_general_mobile_species;      // index of mobile chem species

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
    
            // Derivative (ss: sink or load)
            d_chemass_ss = std::unique_ptr<
                arma::field< // Compartments
                arma::field< // Chemical Species
                arma::Cube<  // Dimensions: nx, ny, nz
                double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));

            // Derivative (ewf: external water flux)
            d_chemass_ewf = std::unique_ptr<
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

            // Derivative (ewf: external water flux)
            d_chemass_ewf_out = std::unique_ptr<
                arma::field< // Compartments
                arma::field< // Chemical Species
                arma::Cube<  // Dimensions: nx, ny, nz
                double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));
                
            // ############################################
            // Single time, constant or isolated changes 
            // to state-variable

            // Derivative (ss: sink and source)
            d_chemass_ss_out = std::unique_ptr<
                arma::field< // Compartments
                arma::field< // Chemical Species
                arma::Cube<  // Dimensions: nx, ny, nz
                double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));

            // ############################################
            // External IN-fluxes
            // ############################################
            ewf_conc = std::unique_ptr<
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
        d_chemass_ewf,              // derivative (isolated) EWF
        d_chemass_ic,               // derivative (at start) IC
        d_chemass_dt_chem_out,      // cumulative derivative for output in debug model
        d_chemass_dt_transp_out,    // cumulative derivative for output in debug model
        d_chemass_ss_out,           // cumulative derivative for output in debug model
        d_chemass_ewf_out,          // cumulative derivative for output in debug model
        ewf_conc;                   // concentration of external water fluxes

};



#endif