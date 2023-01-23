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

    // SS (find and apply)
    OpenWQ_extwatflux_ss.CheckApply_EWFandSS(
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_utils,
        OpenWQ_units,
        OpenWQ_output,
        year_sim_now,
        month_sim_now,
        day_sim_now,
        hour_sim_now,
        min_sim_now,
        sec_sim_now,
        (std::string) "ss",
        OpenWQ_wqconfig.SinkSource_FORC);

    // EWF (find and update ewf_conc for use with EWF)
    OpenWQ_extwatflux_ss.CheckApply_EWFandSS(
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_utils,
        OpenWQ_units,
        OpenWQ_output,
        year_sim_now,
        month_sim_now,
        day_sim_now,
        hour_sim_now,
        min_sim_now,
        sec_sim_now,
        (std::string) "ewf",
        OpenWQ_wqconfig.ExtFlux_FORC); 

    // Flag to note that 1st time step has been completed
    OpenWQ_wqconfig.tstep1_flag = false;

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


// ################################################################
// Calls inside space loop
// Called for each grid cell
// Applicable to:
// 1) Fluxes between compartments and compartment-domain elements (ix,iy,iz)
// 2) Out-Fluxes (fluxes that exit the system - loss from control volume)
// ################################################################
void OpenWQ_couplercalls::RunSpaceStep(
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
    time_t simtime,                                 // simulation time in seconds since seconds since 00:00 hours, Jan 1, 1970 UTC
    const int source, const int ix_s, const int iy_s, const int iz_s,
    const int recipient, const int ix_r, const int iy_r, const int iz_r,
    const double wflux_s2r, const double wmass_source){

    
    // Local variables
    std::string msg_string;

    // Return if flux or source_volume is zero
    if (wflux_s2r == 0.0f || wmass_source <= 0.0f)
        return;

    // #################################################
    // MODULES

    // #################################################
    // TE module
    // Mass transport with water (mobile material only)
    // #################################################

    // NATIVE TE model
    if ((OpenWQ_wqconfig.TE_module).compare("OPENWQ_NATIVE_TE_ADVDISP") == 0)
    {

        // Advection and dispersion
        OpenWQ_watertransp.AdvDisp(
            OpenWQ_vars, OpenWQ_wqconfig,
            source, ix_s, iy_s, iz_s,
            recipient, ix_r, iy_r, iz_r,
            wflux_s2r, wmass_source);


        // Internal mobilization of immobile pools
        // Erosion and weathering
        OpenWQ_watertransp.IntMob(
            OpenWQ_vars, OpenWQ_wqconfig,
            source, ix_s, iy_s, iz_s,
            recipient, ix_r, iy_r, iz_r,
            wflux_s2r, wmass_source);
   

        // Boundary Mixing due to velocity gradients
        // due to turbulence and cross-boarder eddies
        // only apply if fluxe between cells in same compartment          
        OpenWQ_watertransp.BoundMix(
            OpenWQ_hostModelconfig, OpenWQ_vars, OpenWQ_wqconfig,
            source, ix_s, iy_s, iz_s,
            recipient, ix_r, iy_r, iz_r,
            wflux_s2r, wmass_source);
    
    }else if ((OpenWQ_wqconfig.TE_module).compare("OPENWQ_NATIVE_TE_ADVP") == 0)
    {

        // Advection and dispersion
        OpenWQ_watertransp.Adv(
            OpenWQ_vars, OpenWQ_wqconfig,
            source, ix_s, iy_s, iz_s,
            recipient, ix_r, iy_r, iz_r,
            wflux_s2r, wmass_source);


        // Internal mobilization of immobile pools
        // Erosion and weathering
        OpenWQ_watertransp.IntMob(
            OpenWQ_vars, OpenWQ_wqconfig,
            source, ix_s, iy_s, iz_s,
            recipient, ix_r, iy_r, iz_r,
            wflux_s2r, wmass_source);
   

        // Boundary Mixing due to velocity gradients
        // due to turbulence and cross-boarder eddies
        // only apply if fluxe between cells in same compartment          
        OpenWQ_watertransp.BoundMix(
            OpenWQ_hostModelconfig, OpenWQ_vars, OpenWQ_wqconfig,
            source, ix_s, iy_s, iz_s,
            recipient, ix_r, iy_r, iz_r,
            wflux_s2r, wmass_source);


    }else if ((OpenWQ_wqconfig.TE_module).compare("OPENWQ_NATIVE_TE_NO_ADVDISP") == 0)
    {
        
        // Internal mobilization of immobile pools
        // Erosion and weathering
        OpenWQ_watertransp.IntMob(
            OpenWQ_vars, OpenWQ_wqconfig,
            source, ix_s, iy_s, iz_s,
            recipient, ix_r, iy_r, iz_r,
            wflux_s2r, wmass_source);
   
        // Boundary Mixing due to velocity gradients
        // due to turbulence and cross-boarder eddies
        OpenWQ_watertransp.BoundMix(
            OpenWQ_hostModelconfig, OpenWQ_vars, OpenWQ_wqconfig,
            source, ix_s, iy_s, iz_s,
            recipient, ix_r, iy_r, iz_r,
            wflux_s2r, wmass_source);

    }else{

        // Create Message
        msg_string = 
            "<OpenWQ> ERROR: No TE_module found or unkown";

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

// ################################################################
// Calls inside space loop
// Called for each grid cell
// Applicable to:
// 1) IN-fluxes (external water fluxes)
// ################################################################
void OpenWQ_couplercalls::RunSpaceStep_IN(
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
    time_t simtime, // simulation time in seconds since seconds since 00:00 hours, Jan 1, 1970 UTC
    std::string source_EWF_name,                    // name defined in HydroExtFlux (in couplecalls)
    const int recipient, const int ix_r, const int iy_r, const int iz_r,
    const double wflux_s2r){

    // Dummy variable
    double ewf_conc_chemi;  // concentraton of chemical at EWF at current time step
    unsigned int iewf;      // index of EWF

    // Find specific EWF index in EWF names
    iewf = OpenWQ_utils.FindStrIndexInVectStr(
        OpenWQ_hostModelconfig.ewf_names,
        source_EWF_name);

    // Loop for all chemical species listed in the BGQ file
    // both mobile and immobile because these are external inputs
    for (unsigned int chemi=0;chemi<OpenWQ_wqconfig.BGC_general_num_chem;chemi++){

        // Get appropriate chemi concentraton for this ewf and domain ix, iy, iz
        ewf_conc_chemi = (*OpenWQ_vars.ewf_conc)
            (iewf)
            (chemi)
            (ix_r,iy_r,iz_r);

        // Advection and dispersion
        OpenWQ_watertransp.Adv_IN(
            OpenWQ_vars, OpenWQ_wqconfig,
            recipient, ix_r, iy_r, iz_r,
            wflux_s2r,                      // external water flux
            chemi,                          // chemical id (from BGQ file and used in state-varibble data structures)
            ewf_conc_chemi);                // concentration taken from EWF json file

    }

}

// ################################################################
// Calls all functions required inside time loop
// But AFTER space loop has been finalized
// ################################################################
void OpenWQ_couplercalls::RunTimeLoopEnd(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_json& OpenWQ_json,                    // create OpenWQ_json object
    OpenWQ_wqconfig& OpenWQ_wqconfig,            // create OpenWQ_wqconfig object
    OpenWQ_units& OpenWQ_units,                  // functions for unit conversion
    OpenWQ_utils& OpenWQ_utils,                    // utility methods/functions
    OpenWQ_readjson& OpenWQ_readjson,            // read json files
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_initiate& OpenWQ_initiate,            // initiate modules
    OpenWQ_watertransp& OpenWQ_watertransp,      // transport modules
    OpenWQ_chem& OpenWQ_chem,                   // biochemistry modules
    OpenWQ_extwatflux_ss& OpenWQ_extwatflux_ss,        // sink and source modules)
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

    // ########################################
    // Output Results
    // ###########################################

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