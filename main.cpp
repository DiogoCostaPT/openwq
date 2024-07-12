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

#include <iostream>
#include <fstream>
#include <armadillo>
#include <string>
#include <time.h> 

// #include "utility.h"

// ########################################################
// openWQ
#include "couplercalls/OpenWQ_couplercalls.hpp"
#include "global/OpenWQ_hostModelconfig.hpp"
#include "global/OpenWQ_json.hpp"
#include "global/OpenWQ_wqconfig.hpp"
#include "global/OpenWQ_vars.hpp"
#include "readjson/OpenWQ_readjson.hpp"
#include "initiate/OpenWQ_initiate.hpp"
#include "chem/OpenWQ_chem.hpp"
#include "watertransp/OpenWQ_watertransp.hpp"
#include "extwatflux_ss/OpenWQ_extwatflux_ss.hpp"
#include "units/OpenWQ_units.hpp"
#include "solver/OpenWQ_solver.hpp"
#include "output/OpenWQ_output.hpp"


int main(int argc, char* argv[]) 
{   
    
    // #######################################
    // Create class objects
    // #######################################
    OpenWQ_couplercalls OpenWQ_couplercalls;
    OpenWQ_hostModelconfig OpenWQ_hostModelconfig;
    OpenWQ_json OpenWQ_json;                    // create OpenWQ_json object
    OpenWQ_wqconfig OpenWQ_wqconfig;        // create OpenWQ_wqconfig object
    OpenWQ_units OpenWQ_units;                  // functions for unit conversion
    OpenWQ_readjson OpenWQ_readjson;            // read json files
    int num_HydroComp = 8; // number of compartments to link openWQ (see details in OpenWQ_hydrolink.cpp) 
    OpenWQ_vars OpenWQ_vars(num_HydroComp);
    OpenWQ_initiate OpenWQ_initiate;            // initiate modules
    OpenWQ_watertransp OpenWQ_watertransp;      // transport modules
    OpenWQ_chem OpenWQ_chem;                    // biochemistry modules
    OpenWQ_extwatflux_ss OpenWQ_extwatflux_ss;        // sink and source modules
    OpenWQ_solver OpenWQ_solver;                // solver module
    OpenWQ_output OpenWQ_output;                // output module
    OpenWQ_utils OpenWQ_utils;
    

    // #####################################################################################################################
    // #####################################################################################################################
    // 1) CONFIGURATION -> Call OpenWQ_couplercalls.InitialConfig (first coupler call)
    // #####################################################################################################################
    // #####################################################################################################################

    // #######################################
    // 1.1) Pass Location of Masterfile
    // #######################################
    OpenWQ_wqconfig.OpenWQ_masterjson = "/Users/diogocosta/Library/CloudStorage/OneDrive-impactblue-scientific.com/6_Projects/1_GWF/2_WIP/code/code_crhm/build/Case_Studies/big_creek_openwq/openWQ_master.json";

    // #######################################
    // 1.2) COUPLER CODE 
    // Characterize the host model domains
    // #######################################

    // Characterize the Host model domain
    // Host model specific
    /* hydroTuple: 
    (1) index in openWQ variable, 
    (2) name identifier in openWQ_config.json, 
    (3) number of cell in x-direction
    (4) number of cell in y-direction
    (5) number of cell in z-direction
    */
    // #######################################
    unsigned int nx_num = 40;
    unsigned int ny_num = 1;
    unsigned int nz_num = 1; 
    if (OpenWQ_hostModelconfig.HydroComp.size()==0){
        OpenWQ_hostModelconfig.HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(0,"SWE",nx_num,ny_num,nz_num));
        OpenWQ_hostModelconfig.HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(1,"RUNOFF",nx_num,ny_num,nz_num));
        OpenWQ_hostModelconfig.HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(2,"SSR",nx_num,ny_num,nz_num));
        OpenWQ_hostModelconfig.HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(3,"SD",nx_num,ny_num,nz_num));
        OpenWQ_hostModelconfig.HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(4,"SOIL_RECHR",nx_num,ny_num,nz_num));
        OpenWQ_hostModelconfig.HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(5,"SOIL_LOWER",nx_num,ny_num,nz_num));
        OpenWQ_hostModelconfig.HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(6,"SURFSOIL",nx_num,ny_num,nz_num));
        OpenWQ_hostModelconfig.HydroComp.push_back(OpenWQ_hostModelconfig::hydroTuple(7,"GW",nx_num,ny_num,nz_num));
    // (add other compartments as needed)...
    }

    // #######################################
    // 1.3( COUPLER CALL 
    // (DO NOT CHANGE THIS CALL!!!)
    // #######################################

    OpenWQ_couplercalls.InitialConfig(
            OpenWQ_hostModelconfig,
            OpenWQ_json,                    // create OpenWQ_json object
            OpenWQ_wqconfig,            // create OpenWQ_wqconfig object
            OpenWQ_units,                  // functions for unit conversion
            OpenWQ_utils,                    // utility methods/functions
            OpenWQ_readjson,            // read json files
            OpenWQ_vars,
            OpenWQ_initiate,            // initiate modules
            OpenWQ_watertransp,      // transport modules
            OpenWQ_chem,                   // biochemistry modules
            OpenWQ_extwatflux_ss,        // sink and source modules)
            OpenWQ_output);

    time_t simtime;

    // #####################################################################################################################
    // START of Time LOOP
    for (unsigned int nx=0;nx<nx_num;nx++){

        // #####################################################################################################################
        // #####################################################################################################################
        // 2) INSIDE_TIME_LOOP_BUT_BEFORE_SPACE_LOOP -> Call OpenWQ_couplercalls.RunTimeLoopStart (2nd coupler call)
        // #####################################################################################################################
        // #####################################################################################################################

        // (Only needed for mock coupler) Update simtime (based on hostmodel)
        // START ---------------
        simtime = time(0);
        // END ---------------

        // #######################################
        // 2.1) COUPLER CODE 
        // Update chemistry dependencies
        // #######################################

        //#pragma omp parallel for num_threads(OpenWQ_wqconfig.num_threads_requested)
        //for (unsigned int nx=0;nx<nx_num;nx++){
        //    for (unsigned int ny=0;ny<ny_num;ny++){
        //        for (unsigned int nz=0;nz<nz_num;nz++){
        //            (*OpenWQ_hostModelconfig.SM)(nx,ny,nz) = 0,3;  // loop needed - Save all SM data from hostmodel at time t
        //            (*OpenWQ_hostModelconfig.Tair)(nx,ny,nz) = 20;  // loop needed - Save all Taair data from hostmodel at time t      
        //            (*OpenWQ_hostModelconfig.Tsoil)(nx,ny,nz) = 15;   // keeping the same as Tair for now
        //        }
        //    }
        //}

        // #######################################
        // 2.2) COUPLER CODE 
        // Update compartment/Control Volume Water Volume
        // #######################################

        #pragma omp parallel for num_threads(OpenWQ_wqconfig.num_threads_requested)
        for (unsigned int nx=0;nx<nx_num;nx++){
            for (unsigned int ny=0;ny<ny_num;ny++){
                for (unsigned int nz=0;nz<nz_num;nz++){

                    (*OpenWQ_hostModelconfig.waterVol_hydromodel)[0](nx,ny,nz) = 2;
                    (*OpenWQ_hostModelconfig.waterVol_hydromodel)[1](nx,ny,nz) = 2;
                    (*OpenWQ_hostModelconfig.waterVol_hydromodel)[2](nx,ny,nz) = 2;
                    (*OpenWQ_hostModelconfig.waterVol_hydromodel)[3](nx,ny,nz) = 2;
                    (*OpenWQ_hostModelconfig.waterVol_hydromodel)[4](nx,ny,nz) = 2;
                    (*OpenWQ_hostModelconfig.waterVol_hydromodel)[5](nx,ny,nz) = 2;
                    (*OpenWQ_hostModelconfig.waterVol_hydromodel)[6](nx,ny,nz) = 2;
                    (*OpenWQ_hostModelconfig.waterVol_hydromodel)[7](nx,ny,nz) = 2;

                }
            }
        }

        // #######################################
        // 2.3) COUPLER CALL 
        // (DO NOT CHANGE THIS CALL!!!)
        // #######################################

        OpenWQ_couplercalls.RunTimeLoopStart(
            OpenWQ_hostModelconfig,
            OpenWQ_json,                    // create OpenWQ_json object
            OpenWQ_wqconfig,            // create OpenWQ_wqconfig object
            OpenWQ_units,                  // functions for unit conversion
            OpenWQ_utils,
            OpenWQ_readjson,            // read json files
            OpenWQ_vars,
            OpenWQ_initiate,            // initiate modules
            OpenWQ_watertransp,      // transport modules
            OpenWQ_chem,                   // biochemistry modules
            OpenWQ_extwatflux_ss,        // sink and source modules)
            OpenWQ_solver,
            OpenWQ_output,
            simtime);

        // #####################################################################################################################
        // #####################################################################################################################
        // 3) INSIDE_SPACE_LOOP -> Call OpenWQ_couplercalls.RunSpaceStep (3rd coupler call)
        // #####################################################################################################################
        // #####################################################################################################################

        // (Only needed for mock coupler)
        // START ---------------
        int source;
        int ix_s;
        int iy_s;
        int iz_s;
        int recipient;
        int ix_r;
        int iy_r;
        int iz_r;
        double wflux_s2r;
        double wmass_source;
         // END ---------------

        for (unsigned int nx=0;nx<nx_num;nx++){
            for (unsigned int ny=0;ny<ny_num;ny++){
                for (unsigned int nz=0;nz<nz_num;nz++){

                    // (Only needed for mock coupler)
                    // START ---------------
                    source = 0;
                    ix_s = nx;
                    iy_s = ny;
                    iz_s = nz;
                    recipient = 0;  
                    ix_r = std::min(nx,nx_num-1);
                    iy_r = std::min(ny,ny_num-1);
                    iz_r = std::min(nz,nz_num-1);
                    wflux_s2r = 0.1 * (*OpenWQ_hostModelconfig.waterVol_hydromodel)[source](ix_s,iy_s,iz_s);
                    wmass_source = (*OpenWQ_hostModelconfig.waterVol_hydromodel)[source](ix_s,iy_s,iz_s);
                    // END ---------------

                    OpenWQ_couplercalls.RunSpaceStep(
                        OpenWQ_hostModelconfig,
                        OpenWQ_json,                    // create OpenWQ_json object
                        OpenWQ_wqconfig,            // create OpenWQ_wqconfig object
                        OpenWQ_units,                  // functions for unit conversion
                        OpenWQ_utils,
                        OpenWQ_readjson,            // read json files
                        OpenWQ_vars,
                        OpenWQ_initiate,            // initiate modules
                        OpenWQ_watertransp,      // transport modules
                        OpenWQ_chem,                   // biochemistry modules
                        OpenWQ_extwatflux_ss,        // sink and source modules)
                        OpenWQ_solver,
                        OpenWQ_output,
                        simtime,                            // simulation time in seconds since seconds since 00:00 hours, Jan 1, 1970 UTC
                        source, ix_s, iy_s, iz_s,
                        recipient, ix_r, iy_r, iz_r,
                        wflux_s2r, wmass_source);

                }
            }
        }

        // #####################################################################################################################
        // #####################################################################################################################
        // 4) INSIDE_TIME_LOOP_BUT_AFTER_SPACE_LOOP -> Call OpenWQ_couplercalls.RunTimeLoopEnd (4th coupler call)
        // #####################################################################################################################
        // #####################################################################################################################

        OpenWQ_couplercalls.RunTimeLoopEnd(
            OpenWQ_hostModelconfig,
            OpenWQ_json,                    // create OpenWQ_json object
            OpenWQ_wqconfig,            // create OpenWQ_wqconfig object
            OpenWQ_units,                  // functions for unit conversion
            OpenWQ_utils,
            OpenWQ_readjson,            // read json files
            OpenWQ_vars,
            OpenWQ_initiate,            // initiate modules
            OpenWQ_watertransp,      // transport modules
            OpenWQ_chem,                   // biochemistry modules
            OpenWQ_extwatflux_ss,        // sink and source modules)
            OpenWQ_solver,
            OpenWQ_output,
            simtime);

    }  

}

