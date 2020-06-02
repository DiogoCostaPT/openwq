
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


#include "utility.h"


// Initialize memory of major variables: arma::field
void initmemory(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar)
{
        // domain/water (number of compartments and grid dimensions)
        int numcmp = JSONfiles.H2O["compartments"].size();
        std::unique_ptr<unsigned int[]> n_xyz(new unsigned int[3]); // pointer to nx, ny, nx information
        n_xyz[0] = 0;
        n_xyz[1] = 0;
        n_xyz[2] = 0;

        // BGC
        int numlistspec = JSONfiles.BGC["list_chemical_species"].size(); // size of list_chemical_speciess
        int numspec_j;

        // Assign and  allocate memory: wflux,wmassk,chemass
               for (int i=0;i<numcmp;i++){
                       
                       // dimensions
                        n_xyz[0] = JSONfiles.H2O[std::to_string(i+1)]["nx"];
                        n_xyz[1] = JSONfiles.H2O[std::to_string(i+1)]["ny"];
                        n_xyz[2] = JSONfiles.H2O[std::to_string(i+1)]["nz"];
                        
                        // fwater fluxes arma::cube
                        arma::Cube<double> domain_xyz(n_xyz[0],n_xyz[1],n_xyz[2]);
                        (*Prj_StateVar.wflux)(i) = domain_xyz; // fwater fluxes arma::cube
                        (*Prj_StateVar.wmass)(i) = domain_xyz; // fwater fluxes arma::cube

                        // water quality
                        numspec_j = 0;
                        for (int j=0;j<numlistspec;j++){ 

                            // number of species in compartment j
                            numspec_j = JSONfiles.BGC["compartments"][std::to_string(i+1)]["chem_species"].size(); 

                            // creating arma::field of arma:c:cube of size numspec_j-> copies of domain_xyz for each species
                            arma::field<arma::Cube<double>> domain_spec(numspec_j);
                            for (int s=0;s<numspec_j;s++){
                                domain_spec(s) = domain_xyz;
                            }
                            (*Prj_StateVar.chemass)(i) = domain_spec;
                        }
        }
}


// Read JSON file to class
void read_JSON_2class(json & jsondata,const std::string& jsonfile)
{
        try{
                std::ifstream i(jsonfile);
                i >> (jsondata);

        }catch (json::type_error){
                std::cout << "An exception occurred parsing" << jsonfile << std::endl;
                abort();
        }
}


// Initial conditions (water and chemical mass)
void IC_calc(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar){

    int numcmp = JSONfiles.H2O["compartments"].size();
        
    for (int i=0;i<numcmp;i++){
        std::string filepath = JSONfiles.H2O[std::to_string(i+1)]["water_fluxes_file"]["file_path"];
        read_file_3Dcoldata(filepath,
            JSONfiles.H2O[std::to_string(i+1)]["water_massbalance_file"]["grid_col"],
            JSONfiles.H2O[std::to_string(i+1)]["water_massbalance_file"]["var_col"],
            (*Prj_StateVar.wmass)(i)
            );

    }

}

// read 3D col data from file at assign to variable
void read_file_3Dcoldata(std::string & filepath,
    json & file_gridxyz_col,
    json & file_varxyz_col,
    arma::Cube<double> & to_cubedata
    ){

    

}