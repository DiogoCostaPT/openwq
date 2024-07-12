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


#include "OpenWQ_vars.hpp"


OpenWQ_vars::OpenWQ_vars(size_t num_HydroComp, size_t num_EWF){

    this-> num_HydroComp = num_HydroComp;
    this-> num_EWF = num_EWF;

    try{

        // Units of chemass are in: g (grams)
        // Thus, concentrations are in mg/l (or g/m3) and volume in m3
        chemass = std::unique_ptr<
            arma::field< // Compartments
            arma::field< // Chemical Species
            arma::Cube<  // Dimensions: nx, ny, nz
            double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));
        
        // ############################################
        // Mass changes (for solver separation and DEBUG mode activation)
        // ############################################

        // ############################################
        // Derivatives

        // Derivative (chemistry)
        d_chemass_dt_chem = std::unique_ptr<
            arma::field< // Compartments
            arma::field< // Chemical Species
            arma::Cube<  // Dimensions: nx, ny, nz
            double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));

        // Derivative (water transport)
        d_chemass_dt_transp = std::unique_ptr<
            arma::field< // Compartments
            arma::field< // Chemical Species
            arma::Cube<  // Dimensions: nx, ny, nz
            double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));

        // ############################################
        // Single time, constant or isolated changes 
        // to state-variable

        // IC (single time change at start of simulation)
        d_chemass_ic = std::unique_ptr<
            arma::field< // Compartments
            arma::field< // Chemical Species
            arma::Cube<  // Dimensions: nx, ny, nz
            double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));

        // Derivative (ss: sink or load)
        d_chemass_ss = std::unique_ptr<
            arma::field< // Compartments
            arma::field< // Chemical Species
            arma::Cube<  // Dimensions: nx, ny, nz
            double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));

        // Derivative (ewf: external water flux)
        d_chemass_ewf = std::unique_ptr<
            arma::field< // Compartments
            arma::field< // Chemical Species
            arma::Cube<  // Dimensions: nx, ny, nz
            double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));

        // ############################################
        // Cumulative Mass changes 
        // Cumulating mass changes until next output is printed
        // Needed for Debug mode
        // ############################################

        // ############################################
        // Derivatives

        // Derivative (chemistry)
        d_chemass_dt_chem_out = std::unique_ptr<
            arma::field< // Compartments
            arma::field< // Chemical Species
            arma::Cube<  // Dimensions: nx, ny, nz
            double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));

        // Derivative (water transport)
        d_chemass_dt_transp_out = std::unique_ptr<
            arma::field< // Compartments
            arma::field< // Chemical Species
            arma::Cube<  // Dimensions: nx, ny, nz
            double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));

        // Derivative (ewf: external water flux)
        d_chemass_ewf_out = std::unique_ptr<
            arma::field< // Compartments
            arma::field< // Chemical Species
            arma::Cube<  // Dimensions: nx, ny, nz
            double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));
            
        // ############################################
        // Single time, constant or isolated changes 
        // to state-variable

        // Derivative (ss: sink and source)
        d_chemass_ss_out = std::unique_ptr<
            arma::field< // Compartments
            arma::field< // Chemical Species
            arma::Cube<  // Dimensions: nx, ny, nz
            double>>>>(new arma::field<arma::field<arma::cube>>(num_HydroComp));

        // ############################################
        // External IN-fluxes
        // ############################################
        ewf_conc = std::unique_ptr<
            arma::field< // Compartments
            arma::field< // Chemical Species
            arma::Cube<  // Dimensions: nx, ny, nz
            double>>>>(new arma::field<arma::field<arma::cube>>(num_EWF));

    }catch(const std::exception& e){

        // Write Error in Console only (the program hasn't started yet)
        std::cout << 
            "ERROR: An exception occured during memory allocation (openWQ_vars.hpp)" 
            << std::endl;
        exit (EXIT_FAILURE);

    }

}