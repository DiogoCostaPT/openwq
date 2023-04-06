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

#include "OpenWQ_hostModelconfig.hpp"


// Constructor
OpenWQ_hostModelconfig::OpenWQ_hostModelconfig() 
{
    // Water volumes from hostmodel
    waterVol_hydromodel = std::unique_ptr<
        std::vector<                            // compartments
        arma::Cube<                             // ix, iy, iz
        double>>>(new std::vector<arma::cube>); 

    // Dependencies from hostmodel 
    // (to be available for BGC)
    dependVar = std::unique_ptr<
        std::vector<                            // dependency variable
        arma::Cube<                             // ix, iy, iz
        double>>>(new std::vector<arma::cube>);

    dependVar_scalar = std::unique_ptr<
        std::vector<
        double>>(new std::vector<double>);
}

// Methods

void OpenWQ_hostModelconfig::increment_interaction_step()
{
    interaction_step++;
}

bool OpenWQ_hostModelconfig::is_first_interaction_step()
{
    return (interaction_step == 1);
}