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

#pragma once

#include <armadillo>
#include <memory>
#include "exprtk.hpp"
#include <string>



/* #################################################
 General information for openWQ about the project
################################################# */
class OpenWQ_wqconfig
{


    // #################################################
    // Compiling and re-structuring of input data for quicker access during runtime
    // Source and source forcing (SinkSource_FORC)
    // AND
    // External fluxes (ExtFlux_FORC_jsonAscii)
    private:

        // Master file location
        std::string OpenWQ_masterjson;


        size_t num_coldata_jsonAscii = 20;
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
        size_t num_coldata_h5 = 4;
        // 1 - ix
        // 2 - iy
        // 3 - iz
        // 4 - value (already converted to mg/l (concentration) or g(mass))


    public:
        // Constructor
        OpenWQ_wqconfig();

        // Destructor
        ~OpenWQ_wqconfig();

        /**
         * OpenWQ_masterjson
        */
        std::string get_OpenWQ_masterjson();
        void set_OpenWQ_masterjson(std::string OpenWQ_masterjson);

        // /**
        //  * time_previous methods
        // */
        // double get_time_previous();
        // void set_time_previous(double time_previous);
        


    // #################################################
    // General

    // TODO: These variables probably do not need to be here. They seem like they should be part of the
    // JSON classes or output class
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