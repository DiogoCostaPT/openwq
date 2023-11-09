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
        // Vectors with characterization of the different 
        // model compartments and external fluxes
        std::vector<hydroTuple> HydroComp;
        std::vector<hydroTuple> HydroExtFlux;
        std::vector<hydroTuple> HydroDepend;

        // Stores water fluxes when concentration are requested for outputs
        std::unique_ptr<std::vector<arma::Cube<double>>> waterVol_hydromodel;

        // To store all dependency variables 
        // to be available for BGC calculations and expressionss
        std::unique_ptr<std::vector<arma::Cube<double>>> dependVar; // global data
        std::unique_ptr<std::vector<double>> dependVar_scalar;      // data updated for use with exprtk


        // Host model iteraction step (dynamic value)
        long interaction_step = 0;

     
        // Time
        double time_step = 0.0f;    // Host model time step (in seconds)

            
        // Water volume minimum limit (critical for concentration calculations)
        // to avoid concentration instabilities and numerical blowup
        // uses native units: m3
        const double watervol_minlim = 1;



    public:
    
        // Constructor
        OpenWQ_hostModelconfig();
        // Destructor
        ~OpenWQ_hostModelconfig();

        /******** Methods *********/
    
        /********************
         * HydroTuple methods
        *********************/
        // Add a compartment to the vector of compartments
        void add_HydroComp(int index, std::string name, int num_cells_x, int num_cells_y, int num_cells_z);
        // Add a external flux to the vector of external fluxes
        void add_HydroExtFlux(int index, std::string name, int num_cells_x, int num_cells_y, int num_cells_z);
        // Add a dependency variable to the vector of dependency variables
        void add_HydroDepend(int index, std::string name, int num_cells_x, int num_cells_y, int num_cells_z);

        // Return sizes for hydrotuples
        unsigned int get_num_HydroComp();
        unsigned int get_num_HydroExtFlux();
        unsigned int get_num_HydroDepend();

        // get names of compartments and external fluxes
        // for a specific index
        std::string get_HydroComp_name_at(int index);
        std::string get_HydroDepend_name_at(int index);

        // Get vector of compartment names and external fluxes names
        std::vector<std::string> get_HydroComp_names();
        std::vector<std::string> get_HydroExtFlux_names();

        // Get number of cells in x, y and z directions - HydroComp
        unsigned int get_HydroComp_num_cells_x_at(int index);
        unsigned int get_HydroComp_num_cells_y_at(int index);
        unsigned int get_HydroComp_num_cells_z_at(int index);
        // Get number of cells in x, y and z directions - HydroExtFlux
        unsigned int get_HydroExtFlux_num_cells_x_at(int index);
        unsigned int get_HydroExtFlux_num_cells_y_at(int index);
        unsigned int get_HydroExtFlux_num_cells_z_at(int index);
        // Get number of cells in x, y and z directions - HydroDepend
        unsigned int get_HydroDepend_num_cells_x_at(int index);
        unsigned int get_HydroDepend_num_cells_y_at(int index);
        unsigned int get_HydroDepend_num_cells_z_at(int index);

        /*
        * waterVol methods
        */
        void add_waterVol_hydromodel(arma::Cube<double> waterVol);
        double get_waterVol_hydromodel_at(int index, int ix, int iy, int iz);
        void set_waterVol_hydromodel_at(int index, int ix, int iy, int iz, double value);
        // waterVol_minlim method
        double get_watervol_minlim();
        
        /*
        * dependVar methods
        */
        void add_dependVar(arma::Cube<double> dependVar);
        double get_dependVar_at(int index, int ix, int iy, int iz);
        void set_dependVar_at(int index, int ix, int iy, int iz, double value); 

        void add_dependVar_scalar(double dependVar_scalar);
        double get_dependVar_scalar_at(int index);
        void set_dependVar_scalar_at(int index, double value);

        /*
        * interaction_step methods
        */
        // Increase interaction step by 1
        void increment_interaction_step();
        // Return True if interaction step is 0, False otherwise
        bool is_first_interaction_step();

        /*
        * time_step methods
        */
        bool is_time_step_0();
        void set_time_step(double time_step);
        double get_time_step();


    

};