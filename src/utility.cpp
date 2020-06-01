
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


void initiate(ClassGenProj& ClassGP, ClassDetProj& ClassDP){

    int numcmp = ClassGP.size.number_of_compartments;
    int numspec = ClassGP.size.number_of_chemical_species;

     // Allocate memory and set to zero: domain
    (*ClassDP.domain_xyz).zeros();

    // Assign and  allocate memory: wflux,wmassk,chemass
    for (int i=0;i<numcmp;i++){
        for (int j=0;j<numspec;j++){
            (*ClassDP.wflux)(i,j) = (*ClassDP.domain_xyz);
            (*ClassDP.wmass)(i,j) = (*ClassDP.domain_xyz);
            (*ClassDP.chemass)(i,j) = (*ClassDP.domain_xyz);
        }
    }


}