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

#include "utility.h"
#include "DEMOS_OpenWQ_run.h"

#include "DEMOS_OpenWQ_global.h"
#include "DEMOS_OpenWQ_start.h"
#include "DEMOS_OpenWQ_load.h"

int main(int argc, char* argv[]) 
{   
    std::string vtufilename;

    // Configuration file (from argv)
    std::string DEMOS_OpenWQ_configjson (argv[1]); 
    
    // Create Object: DEMOS_OpenWQ_json (Input JSON files)
    DEMOS_OpenWQ_json DEMOS_OpenWQ_json; // create object
    
    // Load input data
    int numcmp;
    int numinteract;
    DEMOS_OpenWQ_load DEMOS_OpenWQ_load; // create object: json files load modules
    DEMOS_OpenWQ_load.loadinit(DEMOS_OpenWQ_json,
        DEMOS_OpenWQ_configjson,
        numcmp,
        numinteract);
    
    // Create Object: DEMOS_OpenWQ_vars (opernWQ variables)
    DEMOS_OpenWQ_vars DEMOS_OpenWQ_vars(numcmp,numinteract);

    // DEMOS_OpenWQ_start
    DEMOS_OpenWQ_start DEMOS_OpenWQ_start; // create object: start modules e.g., initiate
    DEMOS_OpenWQ_start.initiate(
        DEMOS_OpenWQ_json,
        DEMOS_OpenWQ_configjson,
        DEMOS_OpenWQ_vars);
    
    // DEMOS_OpenWQ_run
    DEMOS_OpenWQ_run DEMOS_OpenWQ_run;      // create object: transport modules
    DEMOS_OpenWQ_chem DEMOS_OpenWQ_chem;    // create object: biochemistry modules
    DEMOS_OpenWQ_print DEMOS_OpenWQ_print;  // print modules
    DEMOS_OpenWQ_run.callrun(DEMOS_OpenWQ_json,
        DEMOS_OpenWQ_vars,
        DEMOS_OpenWQ_start,
        DEMOS_OpenWQ_chem,
        DEMOS_OpenWQ_print);

}

