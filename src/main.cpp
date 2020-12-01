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

int main(int argc, char* argv[]) 
{   
    std::string vtufilename;

    // Configuration file (from argv)
    std::string DEMOS_OpenWQ_configjson (argv[1]); 
    
    // Input JSON files
    DEMOS_OpenWQ_json DEMOS_OpenWQ_json; // create object
    
    // DEMOS_OpenWQ_start
    DEMOS_OpenWQ_start DEMOS_OpenWQ_start; // create object
    DEMOS_OpenWQ_start.initiate(DEMOS_OpenWQ_json,
            DEMOS_OpenWQ_configjson);
    
    // DEMOS_OpenWQ_run
    DEMOS_OpenWQ_run DEMOS_OpenWQ_run; // create object
    DEMOS_OpenWQ_run.callrun(DEMOS_OpenWQ_json);

}

