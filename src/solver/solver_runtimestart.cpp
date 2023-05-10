
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


#include "OpenWQ_solver.hpp"


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
        #pragma omp parallel for collapse(2) num_threads(OpenWQ_wqconfig.get_num_threads_requested())
        for (unsigned int icmp=0;icmp<OpenWQ_hostModelconfig.get_num_HydroComp();icmp++){

            // Chemical loop
            for (unsigned int chemi=0;chemi<(OpenWQ_wqconfig.BGC_general_num_chem);chemi++){

                // Reset derivatives of state-variables to zero after each time interaction
                (*OpenWQ_vars.d_chemass_ic)(icmp)(chemi).zeros();
                (*OpenWQ_vars.d_chemass_ss)(icmp)(chemi).zeros();
                (*OpenWQ_vars.d_chemass_ewf)(icmp)(chemi).zeros();
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
        #pragma omp parallel for collapse(2) num_threads(OpenWQ_wqconfig.get_num_threads_requested())
        for (unsigned int icmp=0;icmp<OpenWQ_hostModelconfig.get_num_HydroComp();icmp++){

            // Chemical loop
            for (unsigned int chemi=0;chemi<(OpenWQ_wqconfig.BGC_general_num_chem);chemi++){

                // Reset derivatives of state-variables to zero after each time interaction
                (*OpenWQ_vars.d_chemass_ss_out)(icmp)(chemi).zeros();
                (*OpenWQ_vars.d_chemass_ewf_out)(icmp)(chemi).zeros();
                (*OpenWQ_vars.d_chemass_dt_chem_out)(icmp)(chemi).zeros();
                (*OpenWQ_vars.d_chemass_dt_transp_out)(icmp)(chemi).zeros();

            }

        }
    }

}

// ########################################
// Reset EWF conc 
// // Specially needed for discrete conc requests
// ########################################
void OpenWQ_solver::Reset_EWFconc(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_vars& OpenWQ_vars){

    // Compartment loop
    #pragma omp parallel for collapse(2) num_threads(OpenWQ_wqconfig.get_num_threads_requested())
    for (unsigned int ewfi=0;ewfi<OpenWQ_hostModelconfig.get_num_HydroExtFlux();ewfi++){

        // Chemical loop
        for (unsigned int chemi=0;chemi<(OpenWQ_wqconfig.BGC_general_num_chem);chemi++){

            // Reset ewf_conc after each iteraction
            // Specially needed for discrete conc requests
            (*OpenWQ_vars.ewf_conc)(ewfi)(chemi).zeros();

        }

    }
}