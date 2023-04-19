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
std::string OpenWQ_wqconfig::get_OpenWQ_masterjson() 
{
    return this->OpenWQ_masterjson;
}
void OpenWQ_wqconfig::set_OpenWQ_masterjson(std::string OpenWQ_masterjson) 
{
    this->OpenWQ_masterjson = OpenWQ_masterjson;  
}

std::string OpenWQ_wqconfig::get_LogFile_name() 
{
    return this->LogFile_name;
}

std::string OpenWQ_wqconfig::get_LogFile_name_fullpath() 
{
    return this->LogFile_name_fullpath;
}

void OpenWQ_wqconfig::set_LogFile_name_fullpath(std::string output_folder,
                                                std::string output_format) 
{
    this->LogFile_name_fullpath = this->LogFile_name;
    this->LogFile_name_fullpath.insert(0,"/");
    this->LogFile_name_fullpath.insert(0, output_format);
    this->LogFile_name_fullpath.insert(0,"/");
    this->LogFile_name_fullpath.insert(0, output_folder);
}


int OpenWQ_wqconfig::get_num_threads_default() {
    return this->num_threads_default;
}

void OpenWQ_wqconfig::set_num_threads_system(int num_threads_system) {
    this->num_threads_system = num_threads_system;
}

void OpenWQ_wqconfig::set_num_threads_requested(int num_threads_requested) {
    this->num_threads_requested = num_threads_requested;
}
int OpenWQ_wqconfig::get_num_threads_requested() {
    return this->num_threads_requested;
}

void OpenWQ_wqconfig::set_threads_requested_to_system() {
  this->num_threads_requested = this->num_threads_system;
}

void OpenWQ_wqconfig::set_threads_requested_to_default() {
  this->num_threads_requested = this->num_threads_default;
}

bool OpenWQ_wqconfig::is_num_threads_requested_valid() {
  return this->num_threads_system >= this->num_threads_requested;
}

std::string OpenWQ_wqconfig::get_num_threads_warning() {
  return "<OpenWQ> WARNING: Number of threads in the system (" 
      + std::to_string(this->num_threads_system)
      + ") is lower than requested ("
      + std::to_string(this->num_threads_requested)
      + "). All system threads available have been engaged.";
}

std::string OpenWQ_wqconfig::get_num_threads_info() {
  return "<OpenWQ> INFO: Number of threads requested and used: " 
    + std::to_string(this->num_threads_requested) + ".";;
}

double OpenWQ_wqconfig::get_time_previous() {
    return this->time_previous;
}
void OpenWQ_wqconfig::set_time_previous(double time_previous) {
    this->time_previous = time_previous;  
}

double OpenWQ_wqconfig::get_nexttime_out() {
    return this->nexttime_out;
}
void OpenWQ_wqconfig::set_nexttime_out(double nexttime_out) {
    this->nexttime_out = nexttime_out;  
}
void OpenWQ_wqconfig::update_nexttime_out() {
    this->nexttime_out += this->timestep_out;
}
void OpenWQ_wqconfig::set_timestep_out(double timestep_out) {
    this->timestep_out = timestep_out;
}
double OpenWQ_wqconfig::get_timestep_out() {
    return this->timestep_out;
}
void OpenWQ_wqconfig::set_timestep_out_unit(std::string timestep_out_unit) {
    this->timestep_out_unit = timestep_out_unit;
}
std::string OpenWQ_wqconfig::get_timestep_out_unit() {
    return this->timestep_out_unit;
}
// TODO: Reduntant but necessary for now
void OpenWQ_wqconfig::convert_units_timestep_out(std::vector<double> unit_multiplers){    
    this->timestep_out = this->timestep_out * unit_multiplers[0] / unit_multiplers[1];
}

