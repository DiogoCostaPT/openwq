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

// #include "utility.h"

#include "DEMOS_OpenWQ_global.h"
#include "DEMOS_OpenWQ_load.h"
#include "DEMOS_OpenWQ_initiate.h"
#include "DEMOS_OpenWQ_chem.h"
#include "DEMOS_OpenWQ_watertransp.h"
//#include "DEMOS_OpenWQ_print.h"

int main(int argc, char* argv[]) 
{   
    std::string vtufilename;

    // Create Object: DEMOS_OpenWQ_hostModelconfig (link to host hydrological model)
    DEMOS_OpenWQ_hostModelconfig DEMOS_OpenWQ_hostModelconfig;
    // Link openWQ data strucuture indexes to hydrological model compartments 
    typedef std::tuple<int,std::string,int, int, int> hydroTuple;
    /* hydroTuple: 
    (1) index in openWQ variable, 
    (2) name identifier in openWQ_config.json, 
    (3) number of cell in x-direction
    (4) number of cell in y-direction
    (5) number of cell in z-direction
    */
    DEMOS_OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(0,"snow",100,50,1));
    DEMOS_OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(1,"soil",100,50,1));
    DEMOS_OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(2,"groundwater",100,50,1));
    DEMOS_OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(3,"streams",100,50,1));
    // ...
    int num_HydroComp = DEMOS_OpenWQ_hostModelconfig.HydroComp.size(); // number of hydrological compartments in host model


    // Create Object: DEMOS_OpenWQ_json (Input JSON files)
    DEMOS_OpenWQ_json DEMOS_OpenWQ_json; // create object
    
    // Load input data
    DEMOS_OpenWQ_load DEMOS_OpenWQ_load; // create object: json files load modules
    DEMOS_OpenWQ_load.loadinit(
        DEMOS_OpenWQ_json);
    
    // Create Object: DEMOS_OpenWQ_vars (openWQ variables)
    DEMOS_OpenWQ_vars DEMOS_OpenWQ_vars(
        num_HydroComp);
   
    // DEMOS_OpenWQ_initiate
    DEMOS_OpenWQ_initiate DEMOS_OpenWQ_initiate; // create object: start modules e.g., initiate
    DEMOS_OpenWQ_initiate.initiate(
        DEMOS_OpenWQ_json,
        DEMOS_OpenWQ_vars,
        DEMOS_OpenWQ_hostModelconfig,
        num_HydroComp);
    
    // DEMOS_OpenWQ_watertransp
    DEMOS_OpenWQ_watertransp DEMOS_OpenWQ_watertransp;   // create object: transport modules
    DEMOS_OpenWQ_chem DEMOS_OpenWQ_chem;                 // create object: biochemistry modules
    //DEMOS_OpenWQ_print DEMOS_OpenWQ_print;             // print modules
    
    int ts_hosthydromod = DEMOS_OpenWQ_hostModelconfig.HydroComp.size(); // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    
    // Loop: Space and Time
    for (int ts=0;ts<ts_hosthydromod;ts++){ // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL

        int source = 1;                 // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        int ix_s = 1;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        int iy_s = 1;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        int iz_s = 0;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        int recipient = 2;              // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        int ix_r = 1;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        int iy_r = 1;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        int iz_r = 0;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        double wflux_s2r = 0.01;        // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        double wmass_recipient = 0.1;   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL


        /* ###################################################################################
        Transport with water fluxes
        ################################################################################### */ 

        // FUNCTION 1: Just Advection (Land Surface model)
        DEMOS_OpenWQ_watertransp.Adv(
            DEMOS_OpenWQ_json,
            DEMOS_OpenWQ_vars,
            source,
            ix_s, 
            iy_s,
            iz_s,
            recipient,
            ix_r,
            iy_r,
            iz_r,
            wflux_s2r,
            wmass_recipient
            );

        // FUNCTION 2: Advection and Dispersion (Aquatic Systems)
        //DEMOS_OpenWQ_watertransp.AdvDisp(
        //    DEMOS_OpenWQ_json,
        //    DEMOS_OpenWQ_vars);

        //
        //void DEMOS_OpenWQ_watertransp::ChemCompExchange(
        //    DEMOS_OpenWQ_json& DEMOS_OpenWQ_json, 
        //    DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, 
        //    int source, std::string kinetics, 
        //    std::vector<std::string> parameter_names, 
        //    std::vector<double> parameter_values,
        //    std::array<double,7> & linedata, 
        //    int & index_chem){


        /* ###################################################################################
        Biogeochemistry
        ################################################################################### */ 

        DEMOS_OpenWQ_chem.Run(
            DEMOS_OpenWQ_json,
            DEMOS_OpenWQ_vars,
            DEMOS_OpenWQ_hostModelconfig);

        /* ###################################################################################
        Sources and Sink
        ################################################################################### */ 

        // Need to create functions for this

        /* ###################################################################################
        Print Results
        ################################################################################### */ 
        //DEMOS_OpenWQ_print.writeVTU(DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars,num_HydroComp,0); 

    }
    

}

