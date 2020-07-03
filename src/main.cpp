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

#include <iostream>
#include <fstream>
#include <armadillo>
#include <string>

#include "global.h"
#include "utility.h"
#include "main_solver.h"

int main(int argc, char* argv[]) 
{   
    std::string vtufilename;
    JSONfiles JSONfiles;
    
    // Configuration file
    std::string configjson_file (argv[1]); 

    // Read JSON files 
    read_JSON_2class(JSONfiles.Master,configjson_file); // master file
    read_JSON_2class(JSONfiles.H2O,JSONfiles.Master["water_balance_setup_file"]); // H2O file
    read_JSON_2class(JSONfiles.CMPI,JSONfiles.Master["cmp_interaction_file"]); // CMP file
    read_JSON_2class(JSONfiles.WQ,JSONfiles.Master["wq_balance_setup_file"]); // WQ file
    read_JSON_2class(JSONfiles.BGC,JSONfiles.Master["BGC_cycling_setup_file"]); // BGC file
   
    // Assign the main arma::field variables
    int numcmp = JSONfiles.H2O["compartments"].size(); // number of compartments
    int numinteract = JSONfiles.CMPI["interactions"].size(); // number of interactions between compartments
    Prj_StateVar Prj_StateVar(numcmp,numinteract);

    // Initialize memmory for major arma::field variables
    initmemory(JSONfiles,Prj_StateVar);

    // IC (water and chemical mass)
    readSetIC(JSONfiles,Prj_StateVar);

    // Transport solver
    main_solver(JSONfiles,Prj_StateVar);

}

