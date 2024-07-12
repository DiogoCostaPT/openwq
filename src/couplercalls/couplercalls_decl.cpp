// Copyright 2020, Diogo Costa, diogo.pinhodacosta@canada.ca
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


#include "OpenWQ_couplercalls.hpp"

// ################################################################
// Initial Configuration of OpenWQ
// ################################################################

void OpenWQ_couplercalls::InitialConfig(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_json& OpenWQ_json,                    // create OpenWQ_json object
    OpenWQ_wqconfig& OpenWQ_wqconfig,            // create OpenWQ_wqconfig object
    OpenWQ_units& OpenWQ_units,                  // functions for unit conversion
    OpenWQ_utils& OpenWQ_utils,                    // utility methods/functions
    OpenWQ_readjson& OpenWQ_readjson,            // read json files
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_initiate& OpenWQ_initiate,            // initiate modules
    OpenWQ_watertransp& OpenWQ_watertransp,      // transport modules
    OpenWQ_chem& OpenWQ_chem,                    // biochemistry modules
    OpenWQ_extwatflux_ss& OpenWQ_extwatflux_ss,  // sink and source modules)
    OpenWQ_output& OpenWQ_output){


    // Local variables
    std::string msg_string; // error message
    
    // ################################
    // Read JSON files
    // ################################
        
    OpenWQ_readjson.read_all(
        OpenWQ_json,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_units,
        OpenWQ_utils,
        OpenWQ_output);

    // ################################
    // Allocate memory: set variable sizes
    // ################################
    OpenWQ_initiate.initmemory(
        OpenWQ_json,
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_output);
        
    // ##################################
    // Parse Sink and Source Inputs AND External Water Fluxes
    // and store them in tuple and arma::mat for quick access
    // ##################################
    // SS
    OpenWQ_extwatflux_ss.Set_EWFandSS_drive(
        OpenWQ_json.SinkSource, // SS json
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_units,
        OpenWQ_utils,
        OpenWQ_output,
        (std::string) "ss");    // flag for SS

    // EWF
    OpenWQ_extwatflux_ss.Set_EWFandSS_drive(
        OpenWQ_json.ExtWatFlux, // EWF json
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_units,
        OpenWQ_utils,
        OpenWQ_output,
        (std::string) "ewf");  // flag for EWF

    // ##################################
    // MODULES
    // ##################################
   
    // ##################################
    // BIOGEOCHEMISTRY
    
    // NATIVE Bigoeochemical model
    // Parse biogeochemical expressions (and save in global)
    if ((OpenWQ_wqconfig.BGC_module).compare("OPENWQ_NATIVE_BGC") == 0)
    {
        
        OpenWQ_chem.setBGCexpressions(
            OpenWQ_json,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            OpenWQ_vars,
            OpenWQ_units,
            OpenWQ_output);

    }else{

        // Create Message
        msg_string = 
            "<OpenWQ> ERROR: No BGC_module found or unkown";

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(
            OpenWQ_wqconfig,    // for Log file name
            msg_string,         // message
            true,               // print in console
            true);              // print in log file
        
        // Abort (Fatal error)
        exit(EXIT_FAILURE);

    }

}