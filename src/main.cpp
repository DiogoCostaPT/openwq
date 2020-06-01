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

#include "globalgeneral.h"
#include "globalspecific.h"
#include "process_json.h"
#include "utility.h"

int main(int argc, char* argv[]) 
{   
    ClassGenProj ClassGP; 

    // Configuration file
    std::string configjson_file (argv[1]); 

    // Get general info and size of the project
    read_json_infosize(ClassGP,configjson_file);

    // Assign OnOFF array
    //OnOFF OnOFF(ClassGP.size.number_of_compartments,ClassGP.size.number_of_chemical_species); // create class
    //OnOFF_assign(ClassGP,OnOFF); // assign values

    // Assign the main variable
    ClassDetProj ClassDP(ClassGP.size.number_of_compartments,
        ClassGP.size.number_of_chemical_species,
        ClassGP.size.dim_nx,
        ClassGP.size.dim_ny,
        ClassGP.size.dim_nz);

    // Initialize major arma::field variables
    initiate(ClassGP,ClassDP);

    // Initiate (IC and BC)
    //initiate();

}

