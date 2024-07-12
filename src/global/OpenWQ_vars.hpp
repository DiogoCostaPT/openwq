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

#pragma once

#include <armadillo>
#include <memory> 


/* #################################################
 Main openWQ data structures
################################################# */
class OpenWQ_vars
{
    public:

        size_t num_HydroComp;
        size_t num_EWF;

        std::unique_ptr<arma::field<arma::field<arma::Cube<double>>>> 
            chemass,                    // state-variable
            d_chemass_dt_chem,          // derivative (chemistry)
            d_chemass_dt_transp,        // derivative (transport)
            d_chemass_ss,               // derivative (isolated) SS
            d_chemass_ewf,              // derivative (isolated) EWF
            d_chemass_ic,               // derivative (at start) IC
            d_chemass_dt_chem_out,      // cumulative derivative for output in debug model
            d_chemass_dt_transp_out,    // cumulative derivative for output in debug model
            d_chemass_ss_out,           // cumulative derivative for output in debug model
            d_chemass_ewf_out,          // cumulative derivative for output in debug model
            ewf_conc;                   // concentration of external water fluxes

        // Constructor
        OpenWQ_vars(size_t num_HydroComp, size_t num_EWF);


};