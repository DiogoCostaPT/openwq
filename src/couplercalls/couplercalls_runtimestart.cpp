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
// Calls all functions required inside time loop
// But before space loop is initiated
// ################################################################
void OpenWQ_couplercalls::RunTimeLoopStart(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_json& OpenWQ_json,                       // create OpenWQ_json object
    OpenWQ_wqconfig& OpenWQ_wqconfig,               // create OpenWQ_wqconfig object
    OpenWQ_units& OpenWQ_units,                     // functions for unit conversion
    OpenWQ_utils& OpenWQ_utils,                       // utility methods/functions
    OpenWQ_readjson& OpenWQ_readjson,               // read json files
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_initiate& OpenWQ_initiate,               // initiate modules
    OpenWQ_watertransp& OpenWQ_watertransp,         // transport modules
    OpenWQ_chem& OpenWQ_chem,                       // biochemistry modules
    OpenWQ_extwatflux_ss& OpenWQ_extwatflux_ss,     // sink and source modules)
    OpenWQ_solver& OpenWQ_solver,
    OpenWQ_output& OpenWQ_output,
    time_t simtime){                                // simulation time in seconds since seconds 
                                                    // since 00:00 hours, Jan 1, 1970 UTC


    // Local variables
    std::string msg_string; // error/warning message
    
    // ########################################
    // Reset Derivatives 
    // Needed before start of new time iteraction
    // ########################################
    OpenWQ_solver.Reset_Deriv(
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_vars,
        true,       // reset inst derivatives
        false);     // reset cumulative derivatives

    // ########################################
    // Reset EWF conc 
    // Specially needed for discrete conc requests
    // ########################################
    OpenWQ_solver.Reset_EWFconc(
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_vars);


    // ########################################
    // Update all time vars
    // ########################################
    OpenWQ_initiate.setTimeVars(
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        simtime);           // simulation time in seconds since seconds since 00:00 hours, Jan 1, 1970 UTC


    // ################################
    // Read and Set Initial Conditions
    // ################################
    if (OpenWQ_hostModelconfig.is_first_interaction_step()){

        // (icmp, chemi, ix, ix and iz loop is inside the function)
        OpenWQ_initiate.readSet(
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            OpenWQ_utils,
            OpenWQ_units,
            OpenWQ_output);

    }


    // ########################################
    // Sources and Sinks 
    // (doesn't need to be inside the space loop 
    // because space is dealt with inside the function)
    // ########################################         
    struct tm *tm_simtime = localtime(&simtime);

    // Get Year, Month, Day, Hour and Min of simulation time
    int year_sim_now = tm_simtime->tm_year;
    year_sim_now += 1900;   // Because localtime() returns: The number of years since 1900
    int month_sim_now = tm_simtime->tm_mon; 
    month_sim_now++;        // Because localtime() returns: months since January - [0, 11]
    int day_sim_now = tm_simtime->tm_mday;
    int hour_sim_now = tm_simtime->tm_hour;
    int min_sim_now = tm_simtime->tm_min;
    int sec_sim_now = tm_simtime->tm_sec;

    // SS (find and apply) - JSON or ASCII input
    OpenWQ_extwatflux_ss.CheckApply_EWFandSS_jsonAscii(
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_utils,
        OpenWQ_units,
        OpenWQ_output,
        year_sim_now, month_sim_now, day_sim_now,
        hour_sim_now, min_sim_now, sec_sim_now,
        (std::string) "ss",
        OpenWQ_wqconfig.SinkSource_FORC);

    // EWF (find and update ewf_conc for use with EWF) - JSON or ASCII input
    OpenWQ_extwatflux_ss.CheckApply_EWFandSS_jsonAscii(
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_utils,
        OpenWQ_units,
        OpenWQ_output,
        year_sim_now, month_sim_now, day_sim_now,
        hour_sim_now, min_sim_now, sec_sim_now,
        (std::string) "ewf",
        OpenWQ_wqconfig.ExtFlux_FORC_jsonAscii);

    // EWF  (find and update ewf_conc for use with EWF) - H5
    OpenWQ_extwatflux_ss.CheckApply_EWF_h5(
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_utils,
        OpenWQ_units,
        OpenWQ_output,
        year_sim_now, month_sim_now, day_sim_now,
        hour_sim_now, min_sim_now, sec_sim_now); 

    // Flag to note that 1st time step has been completed
    OpenWQ_wqconfig.set_tstep1_flag_false();

    // #################################################
    // MODULES

    // ########################################
    // Biogeochemistry (doesn't need space loop => it's inside the function)
    // ########################################
    
    // NATIVE Bioogeochemical model
    if ((OpenWQ_wqconfig.BGC_module).compare("OPENWQ_NATIVE_BGC") == 0){

        OpenWQ_chem.Run(
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_wqconfig,
            OpenWQ_hostModelconfig,
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