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

#include "OpenWQ_wqconfig.hpp"


// Constructor
OpenWQ_wqconfig::OpenWQ_wqconfig() {
      
    // Sink and source forcing
    SinkSource_FORC = 
        std::unique_ptr<
        arma::Mat<double>>
        (new  arma::mat(0,this->num_coldata_jsonAscii));

    // External fluxes forcing (JSON or ASCII datatypes)
    ExtFlux_FORC_jsonAscii = 
        std::unique_ptr<
        arma::Mat<double>>
        (new  arma::mat(0,this->num_coldata_jsonAscii));
    
    // External fluxes forcing (HDF5) 
    // Storing timestamps as time_t
    ExtFlux_FORC_HDF5vec_time =
        std::unique_ptr<        // EWF-h5 json block/request
        std::vector<            
        std::vector<time_t>>>
        (new  std::vector<std::vector<time_t>>);
    // Storing external_flux id
    ExtFlux_FORC_HDF5vec_ewfCompID =
        std::unique_ptr<
        std::vector<unsigned int>>
        (new  std::vector<unsigned int>);
    // Saving 1 timestep
    ExtFlux_FORC_data_tStep = 
        std::unique_ptr<
        arma::Cube<double>>
        (new  arma::cube);
    // Storing all timesteps
    ExtFlux_FORC_HDF5vec_data = 
        std::unique_ptr<
        std::vector<           // EWF-h5 json block/request
        std::vector<           // ChemSpecies 
        std::vector<           // timestamps
        arma::Cube<double>>>>>
        (new  std::vector<std::vector<std::vector<arma::cube>>>);
}

// Destructor
// OpenWQ_wqconfig::~OpenWQ_wqconfig() {}

/******** 
 * Methods 
 * *********/

/**
 * OpenWQ_masterjson
*/
std::string OpenWQ_wqconfig::get_OpenWQ_masterjson() {
    return this->OpenWQ_masterjson;
}
void OpenWQ_wqconfig::set_OpenWQ_masterjson(std::string OpenWQ_masterjson) {
    this->OpenWQ_masterjson = OpenWQ_masterjson;  
}




