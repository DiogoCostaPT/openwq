
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
void initiate(Prj_GenInfo& Prj_GenInfo, Prj_Dim& Prj_Dim,Prj_StateVar& Prj_StateVar)
{

        int numcmp = Prj_Dim.num_compartments;
        int numspec = Prj_Dim.num_chemical_species;

        std::unique_ptr<unsigned int[]> n_xyz(new unsigned int[3]); // pointer to nx, ny, nx information
        n_xyz[0] = 0;
        n_xyz[1] = 0;
        n_xyz[2] = 0;

        // Allocate memory and set to zero: domain

        // Assign and  allocate memory: wflux,wmassk,chemass
        for (int i=0;i<numcmp;i++){
                for (int j=0;j<numspec;j++){
                        
                        read_json_comptdim(Prj_GenInfo,n_xyz,numcmp,numspec);

                        arma::Cube<double> domain_xyz(n_xyz[1],n_xyz[2],n_xyz[3]);

                        (*Prj_StateVar.wflux)(i,j) = domain_xyz;
                        (*Prj_StateVar.wmass)(i,j) = domain_xyz;
                        (*Prj_StateVar.chemass)(i,j) = domain_xyz;
                }
        }
}