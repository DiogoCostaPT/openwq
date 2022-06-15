
// Copyright 2020, Diogo Costa (diogo.pinhodacosta@canada.ca)
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


#include "OpenWQ_solver.h"

/* #################################################
// General numerical solver
################################################# */

void OpenWQ_solver::Numerical_Solver(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_vars& OpenWQ_vars){

    // Local variables
    unsigned int nx, ny, nz;    // interactive compartment domain dimensions
    unsigned int ix, iy, iz;    // interactive compartment domain cell indexes
    double dm_dt_chem;          // ineractive final derivative (mass) - chemistry transformations
    double dm_dt_trans;         // ineractive final derivative (mass) - transport
    double dm_start;            // interactive dynamic change to state-variable at start of simulations
    double dm_inst;             // interactive instantaneous isolated changes to state-vsriable


    /* #####################################################
    // Compartment loop
    ##################################################### */

    #pragma omp parallel for private (nx, ny, nz, ix, iy, iz, dm_start, dm_inst, dm_dt_chem, dm_dt_trans) num_threads(OpenWQ_wqconfig.num_threads_requested)
    for (unsigned int icmp=0;icmp<OpenWQ_hostModelconfig.num_HydroComp;icmp++){

        // Dimensions for compartment icmp
        nx = std::get<2>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of x elements
        ny = std::get<3>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of y elements
        nz = std::get<4>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of z elements

        // Chemical loop
        for (unsigned int chemi=0;chemi<(OpenWQ_wqconfig.BGC_general_num_chem);chemi++){

            // X, Y, Z loops
            for (ix=0;ix<nx;ix++){
                for (iy=0;iy<ny;iy++){
                    for (iz=0;iz<nz;iz++){

                        // ####################################
                        // 1
                        // Single-time change at simulation start
                        //  (IC and input unit conversions)
                        if (OpenWQ_hostModelconfig.interaction_step == 1){

                            dm_start = 
                                (*OpenWQ_vars.d_chemass_ic)(icmp)(chemi)(ix,iy,iz);

                        }else{
                            dm_start = 0.0f;
                        };

                        // ####################################
                        // 2
                        // Instantaneous, isolated change to state-variable
                        // Sink & Sources

                        dm_inst = (*OpenWQ_vars.d_chemass_ss)(icmp)(chemi)(ix,iy,iz);
                        // updating cumulative calc for output in debug mode
                        (*OpenWQ_vars.d_chemass_ss_out)(icmp)(chemi)(ix,iy,iz) += dm_inst;

                        // ####################################
                        // 4
                        // Dynamic change (derivatives): chemistry and tranport
                        // multiplies by hostmodel dt (OpenWQ_hostModelconfig.time_step)

                        // Chemistry
                        dm_dt_chem = (*OpenWQ_vars.d_chemass_dt_chem)(icmp)(chemi)(ix,iy,iz)
                            * OpenWQ_hostModelconfig.time_step;
                        // updating cumulative calc for output in debug mode
                        (*OpenWQ_vars.d_chemass_dt_chem_out)(icmp)(chemi)(ix,iy,iz) += dm_dt_chem;

                        // Transport (no need to multiply by timestep because the water flux is usually
                        // is provided as a water volume, and not as a water volume per unit of time)
                        dm_dt_trans = (*OpenWQ_vars.d_chemass_dt_transp)(icmp)(chemi)(ix,iy,iz); 
                        // updating cumulative calc for output in debug mode
                        (*OpenWQ_vars.d_chemass_dt_transp_out)(icmp)(chemi)(ix,iy,iz) += dm_dt_trans;  
                        
                        // ####################################
                        // 5
                        // Apply all changes to state variable (all at once)

                        // Change state-variable
                        (*OpenWQ_vars.chemass)(icmp)(chemi)(ix,iy,iz) += 
                            (dm_start               // Mass change due to IC (= to zero if not 1st timestep)
                                + dm_inst           // Mass change due to SS input
                                + dm_dt_chem        // Mass change due to chemistry                            
                                + dm_dt_trans);     // Mass change due to transport

                    }
                }
            }
        }
    }
}


/* #################################################
// Reset derivatives 
// (needed a the start of each temporal iteraction)
################################################# */

void OpenWQ_solver::Reset_Deriv(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_vars& OpenWQ_vars,
    bool inst_deriv_flag,
    bool cum_deriv_flag){


    // Reset Instant Derivative
    if (inst_deriv_flag == true)
    {
        // Compartment loop
        #pragma omp parallel for collapse(2) num_threads(OpenWQ_wqconfig.num_threads_requested)
        for (unsigned int icmp=0;icmp<OpenWQ_hostModelconfig.num_HydroComp;icmp++){

            // Chemical loop
            for (unsigned int chemi=0;chemi<(OpenWQ_wqconfig.BGC_general_num_chem);chemi++){

                // Reset derivatives of state-variables to zero after each time interaction
                (*OpenWQ_vars.d_chemass_ic)(icmp)(chemi).zeros();
                (*OpenWQ_vars.d_chemass_ss)(icmp)(chemi).zeros();
                (*OpenWQ_vars.d_chemass_dt_chem)(icmp)(chemi).zeros();
                (*OpenWQ_vars.d_chemass_dt_transp)(icmp)(chemi).zeros();

            }

        }
    }

    // Reset Cumulative Derivative
    // only used for exporting results in debug mode
    if (cum_deriv_flag == true)
    {
        // Compartment loop
        #pragma omp parallel for collapse(2) num_threads(OpenWQ_wqconfig.num_threads_requested)
        for (unsigned int icmp=0;icmp<OpenWQ_hostModelconfig.num_HydroComp;icmp++){

            // Chemical loop
            for (unsigned int chemi=0;chemi<(OpenWQ_wqconfig.BGC_general_num_chem);chemi++){

                // Reset derivatives of state-variables to zero after each time interaction
                (*OpenWQ_vars.d_chemass_ss_out)(icmp)(chemi).zeros();
                (*OpenWQ_vars.d_chemass_dt_chem_out)(icmp)(chemi).zeros();
                (*OpenWQ_vars.d_chemass_dt_transp_out)(icmp)(chemi).zeros();

            }

        }
    }

}