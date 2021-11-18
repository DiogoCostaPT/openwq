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


#include "OpenWQ_couplercalls.h"

// ################################
// Initial Configuration of OpenWQ
// ################################

void OpenWQ_couplercalls::InitialConfig(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_json& OpenWQ_json,                    // create OpenWQ_json object
    OpenWQ_wqconfig& OpenWQ_wqconfig,            // create OpenWQ_wqconfig object
    OpenWQ_units& OpenWQ_units,                  // functions for unit conversion
    OpenWQ_readjson& OpenWQ_readjson,            // read json files
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_initiate& OpenWQ_initiate,            // initiate modules
    OpenWQ_watertransp& OpenWQ_watertransp,      // transport modules
    OpenWQ_chem& OpenWQ_chem,                   // biochemistry modules
    OpenWQ_sinksource& OpenWQ_sinksource,        // sink and source modules)
    OpenWQ_output& OpenWQ_output){


    // ################################
    // Read JSON file
    // ################################
        
    OpenWQ_readjson.read_all(
        OpenWQ_json,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_units,
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
        
    /* #################################################
    // Parse biogeochemical expressions (and save in global)
    ################################################# */
    OpenWQ_chem.setBGCexpressions(
        OpenWQ_json,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_vars,
        OpenWQ_output);

    /* #################################################
    // Parse sink and source inputs and store them in tuple and arma::mat for rapid access
    ################################################# */
    OpenWQ_sinksource.SetSinkSource(
        OpenWQ_json,
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_units,
        OpenWQ_output);  

}

// #######################
// Calls all functions required inside time loop
// But before space loop is initiated
// #######################
void OpenWQ_couplercalls::RunTimeLoopStart(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_json& OpenWQ_json,                    // create OpenWQ_json object
    OpenWQ_wqconfig& OpenWQ_wqconfig,            // create OpenWQ_wqconfig object
    OpenWQ_units& OpenWQ_units,                  // functions for unit conversion
    OpenWQ_readjson& OpenWQ_readjson,            // read json files
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_initiate& OpenWQ_initiate,            // initiate modules
    OpenWQ_watertransp& OpenWQ_watertransp,      // transport modules
    OpenWQ_chem& OpenWQ_chem,                   // biochemistry modules
    OpenWQ_sinksource& OpenWQ_sinksource,        // sink and source modules)
    OpenWQ_solver& OpenWQ_solver,
    OpenWQ_output& OpenWQ_output,
    time_t simtime){                     // simulation time in seconds since seconds since 00:00 hours, Jan 1, 1970 UTC


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
    // Update all time vars
    // ########################################
    OpenWQ_initiate.setTimeVars(
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        simtime);           // simulation time in seconds since seconds since 00:00 hours, Jan 1, 1970 UTC


    // ################################
    // Read and Set Initial Conditions
    // ################################
    if (OpenWQ_hostModelconfig.interaction_step == 1){

        // (icmp, chemi, ix, ix and iz loop is inside the function)
        OpenWQ_initiate.readSet(
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            OpenWQ_units,
            OpenWQ_output);

    }


    /* ########################################
    Sources and Sinks (doesn't need space loop => it's inside the function)
    ######################################## */            

    struct tm *tm_simtime = localtime(&simtime);

    // Get Year, Month, Day, Hour and Min of simulation time
    int year_sim_now = tm_simtime->tm_year;
    int month_sim_now = tm_simtime->tm_mon;
    int day_sim_now = tm_simtime->tm_mday;
    int hour_sim_now = tm_simtime->tm_hour;
    int min_sim_now = tm_simtime->tm_min;

    OpenWQ_sinksource.CheckApply(
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_output,
        year_sim_now,
        month_sim_now,
        day_sim_now,
        hour_sim_now,
        min_sim_now); // minutes are not available in CRHM (it seems)


    /* ########################################
    Biogeochemistry (doesn't need space loop => it's inside the function)
    ######################################## */ 
    
    OpenWQ_chem.Run(
        OpenWQ_json,
        OpenWQ_vars,
        OpenWQ_wqconfig,
        OpenWQ_hostModelconfig,
        OpenWQ_output);
 }


// #######################
    // Calls all functions required inside time loop
    // But AFTER space loop has been finalized
    // #######################
void OpenWQ_couplercalls::RunTimeLoopEnd(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_json& OpenWQ_json,                    // create OpenWQ_json object
    OpenWQ_wqconfig& OpenWQ_wqconfig,            // create OpenWQ_wqconfig object
    OpenWQ_units& OpenWQ_units,                  // functions for unit conversion
    OpenWQ_readjson& OpenWQ_readjson,            // read json files
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_initiate& OpenWQ_initiate,            // initiate modules
    OpenWQ_watertransp& OpenWQ_watertransp,      // transport modules
    OpenWQ_chem& OpenWQ_chem,                   // biochemistry modules
    OpenWQ_sinksource& OpenWQ_sinksource,        // sink and source modules)
    OpenWQ_solver& OpenWQ_solver,
    OpenWQ_output& OpenWQ_output,
    time_t simtime){


    /* ########################################
    Solver 
    (needs to be inside the time loop, but outside the space loop
    Only place where the state-variables are changed
    ######################################## */ 

    OpenWQ_solver.Numerical_Solver(
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_vars);

    /* ########################################
        Output Results
    ######################################## */

    // Only print if time to print -> Needs to be adapted to host model time conventions
    // Note that OpenWQ_wqconfig.timetep_out converted to seconds in OpenWQ_readjson    

    // Print/Save results
    // Return if not time to print
    if (simtime >= OpenWQ_wqconfig.nexttime_out){

        // Get compartment volumes for calculation of concentration (if requested)
        // neet to convert from mm (CRHM native units) to m3 (OpenWQ native units)

        // Call main printing function
        OpenWQ_output.writeResults(
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            OpenWQ_solver,
            simtime);  // needs to be in seconds since 00:00 hours, Jan 1, 1970 UTC

    }

}