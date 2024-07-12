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

#include "OpenWQ_initiate.hpp"

/* #################################################
// Initialize memory of major variables: arma::field
################################################# */
void OpenWQ_initiate::initmemory(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_output& OpenWQ_output){

    // Pointer to nx, ny, nx information (grids or HRU)
    std::unique_ptr<unsigned int[]> n_xyz(new unsigned int[3]); // pointer to nx, ny, nz information
    n_xyz[0] = 0;
    n_xyz[1] = 0;
    n_xyz[2] = 0;

    // Local variables
    std::string msg_string;     // error/warning message string

    // Create arma for chemical species
    typedef arma::field<arma::Cube<double>> arma_fieldcube; // typedef data structure: used for domain_field

    /* ########################################
    // Loop over compartments
    // Assign and  allocate memory to openWQ variables
    ######################################## */
    for (unsigned int icmp=0;icmp<OpenWQ_hostModelconfig.get_num_HydroComp();icmp++){
            
        // Dimensions for compartment icmp
        n_xyz[0] = OpenWQ_hostModelconfig.get_HydroComp_num_cells_x_at(icmp); // num of x elements
        n_xyz[1] = OpenWQ_hostModelconfig.get_HydroComp_num_cells_y_at(icmp); // num of y elements
        n_xyz[2] = OpenWQ_hostModelconfig.get_HydroComp_num_cells_z_at(icmp); // num of z elements
        
        // Generate arma::cube of compartment icmp size
        arma::Cube<double> domain_xyz(n_xyz[0],n_xyz[1],n_xyz[2]);

        // Set to zero
        domain_xyz.zeros();
        
        // Create arma for chemical species
        // Needs to be reset because each compartment can have a different number of compartments
        arma_fieldcube domain_field(OpenWQ_wqconfig.BGC_general_num_chem); // all species are simulated for all compartments

        /* ########################################
        // Loop over dimensions of compartment icmp
        // Push 3D arma::cube into the arma::field of each chemical species
        ######################################## */
        for (unsigned int chemi=0;chemi<OpenWQ_wqconfig.BGC_general_num_chem;chemi++){
            domain_field(chemi) = domain_xyz;
        }

        /* ########################################
        // Allocate Memory
        ######################################## */
        
        // OpenWQ state variable
        (*OpenWQ_vars.chemass)(icmp) = domain_field;

        // Derivatives
        (*OpenWQ_vars.d_chemass_dt_chem)(icmp) = domain_field;
        (*OpenWQ_vars.d_chemass_dt_transp)(icmp) = domain_field;
        (*OpenWQ_vars.d_chemass_ic)(icmp) = domain_field;
        (*OpenWQ_vars.d_chemass_ss)(icmp) = domain_field;
        (*OpenWQ_vars.d_chemass_ewf)(icmp) = domain_field;

        // Cumulative Derivatives (for export in debug mode)
        (*OpenWQ_vars.d_chemass_dt_chem_out)(icmp) = domain_field;
        (*OpenWQ_vars.d_chemass_dt_transp_out)(icmp) = domain_field;
        (*OpenWQ_vars.d_chemass_ss_out)(icmp) = domain_field;
        (*OpenWQ_vars.d_chemass_ewf_out)(icmp) = domain_field;

        // Hydro model variables (water volumes for calc of concentrations)
        // Set them to ones in case concentrations are not requested
        // In such cases, the output will multiply by one (so it's fine)
        OpenWQ_hostModelconfig.add_waterVol_hydromodel(domain_xyz);

    }

    /* ########################################
    // Loop over EWF
    // Assign and  allocate memory to openWQ variables
    ######################################## */
    for (unsigned int ewfi=0;ewfi<OpenWQ_hostModelconfig.get_num_HydroExtFlux();ewfi++){
            
        // Dimensions for compartment ewfi
        n_xyz[0] = OpenWQ_hostModelconfig.get_HydroExtFlux_num_cells_x_at(ewfi); // num of x elements
        n_xyz[1] = OpenWQ_hostModelconfig.get_HydroExtFlux_num_cells_y_at(ewfi); // num of y elements
        n_xyz[2] = OpenWQ_hostModelconfig.get_HydroExtFlux_num_cells_z_at(ewfi); // num of z elements
        
        // Generate arma::cube of compartment ewfi size
        arma::Cube<double> domain_xyz(n_xyz[0],n_xyz[1],n_xyz[2]);

        // Set to zero
        domain_xyz.zeros();
        
        // Create arma for chemical species
        // Needs to be reset because each compartment can have a different number of compartments
        arma_fieldcube domain_field(OpenWQ_wqconfig.BGC_general_num_chem); // all species are simulated for all compartments

        /* ########################################
        // Loop over dimensions of compartment ewfi
        // Push 3D arma::cube into the arma::field of each chemical species
        ######################################## */
        for (unsigned int chemi=0;chemi<OpenWQ_wqconfig.BGC_general_num_chem;chemi++){
            domain_field(chemi) = domain_xyz;
        }

        /* ########################################
        // Allocate Memory
        ######################################## */
        
        // OpenWQ state variable
        (*OpenWQ_vars.ewf_conc)(ewfi) = domain_field;

    }

    // Hydro model variables used as BGC dependencies
    // Just need to do this once (no need to repeat in this loop)
    for (unsigned int depi=0;depi<OpenWQ_hostModelconfig.get_num_HydroDepend();depi++){
            
        // Dimensions for compartment depi
        n_xyz[0] = OpenWQ_hostModelconfig.get_HydroDepend_num_cells_x_at(depi); // num of x elements
        n_xyz[1] = OpenWQ_hostModelconfig.get_HydroDepend_num_cells_y_at(depi); // num of y elements
        n_xyz[2] = OpenWQ_hostModelconfig.get_HydroDepend_num_cells_z_at(depi); // num of z elements
        
        // Generate arma::cube of compartment depi size
        arma::Cube<double> domain_xyz(n_xyz[0],n_xyz[1],n_xyz[2]);

        // Set to zero
        domain_xyz.zeros();
        
        /* ########################################
        // Allocate Memory
        ######################################## */
        
        // OpenWQ state variable
        OpenWQ_hostModelconfig.add_dependVar(domain_xyz);
        OpenWQ_hostModelconfig.add_dependVar_scalar(0.0f);

    }
    

}
