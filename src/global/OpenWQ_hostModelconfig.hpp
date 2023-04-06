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
#include <vector>

/* #################################################
 Link: openWQ to Host Hydrological Model
################################################# */
class OpenWQ_hostModelconfig
{
    private:
        // Host model iteraction step (dynamic value)
        long interaction_step = 0;


    public:
    
    // ########################
    // Host model COMPARTMENT or EXTERNAL FLUX characterization via tuple
    typedef std::tuple<int,std::string,int, int, int> hydroTuple;
    // Add host_hydrological_model compartment:
    // (1) int => index in openWQ 
    // (2) std::string => reference name in JSON file
    // (3) int => number of cells in x-direction
    // (4) int => number of cells in y-direction
    // (5) int => number of cells in z-direction
    // ########################

    // Constructor
    OpenWQ_hostModelconfig();


    // Vectors with characterization of the different 
    // model compartments and external fluxes
    std::vector<hydroTuple> HydroComp;
    std::vector<hydroTuple> HydroExtFlux;
    std::vector<hydroTuple> HydroDepend;

    // Compartment and external water fluxes names
    std::vector<std::string> cmpt_names;    // names of comparments fluxes
    std::vector<std::string> ewf_names;     // names of external water fluxes
    std::vector<std::string> depend_names;  // names of external water fluxes

    // Number of hydrological compartments and EWF
    // (that can store and transport water)
    unsigned int num_HydroComp;
    unsigned int num_EWF;
    unsigned int num_Depend;

    // Host model time step (in seconds)
    long time_step;

    // Stores water fluxes when concentration are requested for outputs
    std::unique_ptr<std::vector<arma::Cube<double>>> waterVol_hydromodel;

    // To store all dependency variables 
    // to be available for BGC calculations and expressionss
    std::unique_ptr<std::vector<arma::Cube<double>>> dependVar; // global data
    std::unique_ptr<std::vector<double>> dependVar_scalar;      // data updated for use with exprtk

    
    // Water volume minimum limit (critical for concentration calculations)
    // to avoid concentration instabilities and numerical blowup
    // uses native units: m3
    const double watervol_minlim = 1;



    // Methods
    
    // Increase interaction step by 1
    void increment_interaction_step();

    // Return True if interaction step is 0, False otherwise
    bool is_first_interaction_step();

};