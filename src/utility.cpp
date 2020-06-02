
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


// Read grid size for each compartment and initialize
void initiate(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar)
{
        // domain/water (number of compartments and grid dimensions)
        int numcmp = JSONfiles.H2O["number_of_compartments"];
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