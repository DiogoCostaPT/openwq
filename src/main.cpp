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

#include "OpenWQ_global.h"
#include "OpenWQ_readjson.h"
#include "OpenWQ_initiate.h"
#include "OpenWQ_chem.h"
#include "OpenWQ_watertransp.h"
#include "OpenWQ_sinksource.h"
#include "OpenWQ_output.h"
#include "OpenWQ_units.h"


int main(int argc, char* argv[]) 
{   
       
    // Initiate output VTU file name string 
    std::string vtufilename;

    // Create Object: OpenWQ_hostModelconfig (link to host hydrological model)
    OpenWQ_hostModelconfig OpenWQ_hostModelconfig;
    // Link openWQ data strucuture indexes to hydrological model compartments 
    typedef std::tuple<int,std::string,int, int, int> hydroTuple;
    /* hydroTuple: 
    (1) index in openWQ variable, 
    (2) name identifier in openWQ_config.json, 
    (3) number of cell in x-direction
    (4) number of cell in y-direction
    (5) number of cell in z-direction
    */
    OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(0,"SNOW",100,50,1));
    OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(1,"SOIL",100,50,1));
    OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(2,"GROUNDWATER",100,50,1));
    OpenWQ_hostModelconfig.HydroComp.push_back(hydroTuple(3,"STREAMS",100,50,1));
    // (add other compartments as needed)...

    int num_HydroComp = OpenWQ_hostModelconfig.HydroComp.size(); // number of hydrological compartments in host model


    // Create Object: OpenWQ_json (Input JSON files) and wqconfig
    OpenWQ_json OpenWQ_json;                // create OpenWQ_json object
    OpenWQ_wqconfig OpenWQ_wqconfig(11);    // create OpenWQ_wqconfig object
    OpenWQ_units OpenWQ_units;              // functions for unit conversion
    
    // Read JSON file
    OpenWQ_readjson OpenWQ_readjson; // create object: json files load modules
    OpenWQ_readjson.read_all(
        OpenWQ_json,
        OpenWQ_wqconfig,
        OpenWQ_units);
    
    // Create Object: OpenWQ_vars (openWQ variables)
    OpenWQ_vars OpenWQ_vars(
        num_HydroComp);

    // OpenWQ_initiate
    OpenWQ_initiate OpenWQ_initiate; // create object: start modules e.g., initiate
    
    // Allocate memory: set variable sizes
    OpenWQ_initiate.initmemory(
        OpenWQ_json,
        OpenWQ_vars,
        OpenWQ_hostModelconfig);
    
    // Read and Set Initial Conditions 
    // Needs loop in host model because it requires grid-cell volume (mass IC) or water volume (concentration IC))
    unsigned int icmp = 0;                    // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    unsigned int ix = 0;                      // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    unsigned int iy = 0;                      // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    unsigned int iz = 0;                      // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    double igridcell_volume = 1;     // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    double iwater_volume = 0.5;      // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL

    OpenWQ_initiate.readSet(
        OpenWQ_json,
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_units,
        icmp,
        ix,
        iy,
        iz,
        igridcell_volume,   // all calculations assume unit = m3
        iwater_volume);     // all calculations assume unit = m3

    // Create objects 
    OpenWQ_watertransp OpenWQ_watertransp;      // transport modules
    OpenWQ_chem OpenWQ_chem;                    // biochemistry modules
    OpenWQ_sinksource OpenWQ_sinksource;        // sink and source modules
    OpenWQ_output OpenWQ_output;                // print modules
    
    /* #################################################
    // Parse biogeochemical expressions (and save in global)
    ################################################# */
    OpenWQ_chem.setBGCexpressions(
        OpenWQ_json,
        OpenWQ_wqconfig,
        OpenWQ_vars);

    /* #################################################
    // Parse sink and source inputs and store them in tuple and arma::mat for rapid access
    ################################################# */
    OpenWQ_sinksource.SetSinkSource(
        OpenWQ_json,
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_units);    
    
    unsigned int ts_hosthydromod = 5000; // (timesteps) TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
    
    /* #################################################
    // Loop: Time 
    // Space loop is inside OpenWQ_chem.Run function
    // No space loop for OpenWQ_watertransp.Adv: it needs to be called throughout the host model code
    ################################################# */
    for (unsigned int ts=0;ts<ts_hosthydromod;ts++){ // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL

        /* ########################################
         Sources and Sinks (doesn't need space loop => it's inside the function)
        ######################################## */ 
        unsigned int YYYY = 2001;              // current model step: year (TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        unsigned int MM = 5;                   // current model step: month (TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        unsigned int DD = 20;                  // current model step: day (TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        unsigned int HH = 12;                  // current model step: hour (TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL

        
        OpenWQ_sinksource.CheckApply(
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            YYYY,
            MM,
            DD,
            HH);
        

        /* ########################################
        Transport with water fluxes (No space loop: needs to be called for every water flux)
        ######################################## */ 

        unsigned int source = 1;                 // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        unsigned int ix_s = 1;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        unsigned int iy_s = 1;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        unsigned int iz_s = 0;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        unsigned int recipient = 2;              // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        unsigned int ix_r = 1;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        unsigned int iy_r = 1;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        unsigned int iz_r = 0;                   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        double wflux_s2r = 0.01;        // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL
        double wmass_recipient = 0.1;   // TO REMOVE/REPLACE IN HOST HYDROLOGICAL MODEL


        // FUNCTION 1: Just Advection (Land Surface model)
        OpenWQ_watertransp.Adv(
            OpenWQ_json,
            OpenWQ_vars,
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
        //OpenWQ_watertransp.AdvDisp(
        //    OpenWQ_json,
        //    OpenWQ_vars);

        //
        //void OpenWQ_watertransp::ChemCompExchange(
        //    OpenWQ_json& OpenWQ_json, 
        //    OpenWQ_vars& OpenWQ_vars, 
        //    int source, std::string kinetics, 
        //    std::vector<std::string> parameter_names, 
        //    std::vector<double> parameter_values,
        //    std::array<double,7> & linedata, 
        //    int & index_chem){


         /* ########################################
         Biogeochemistry (doesn't need space loop => it's inside the function)
        ###########################
        ############# */ 

        OpenWQ_chem.Run(
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_wqconfig,
            OpenWQ_hostModelconfig);

        /* ########################################
         Output Results
        ######################################## */
        OpenWQ_output.writeVTU(
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            ts); 

    }
    

}

