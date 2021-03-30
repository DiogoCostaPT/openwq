
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


#include "OpenWQ_initiate.h"

/* #################################################
// Initialize memory of major variables: arma::field
################################################# */
void OpenWQ_initiate::initmemory(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig){

    // Pointer to nx, ny, nx information (grids or HRU)
    std::unique_ptr<unsigned int[]> n_xyz(new unsigned int[3]); // pointer to nx, ny, nx information
    n_xyz[0] = 0;
    n_xyz[1] = 0;
    n_xyz[2] = 0;

    // Local variables
    std::string HydroCmpName;   // Hydrological compartment names as specified in main.cpp and 
                                // OpenWQ_json.openWQ_config.json (they need to match)
    unsigned int num_HydroComp = OpenWQ_hostModelconfig.HydroComp.size(); // number of hydrological compartments in host model
    
    // Create arma for chemical species
    unsigned int numspec = OpenWQ_json.BGCcycling["CHEMICAL_SPECIES"]["LIST"].size(); // number of chemical species in BGCcycling
    typedef arma::field<arma::Cube<double>> arma_fieldcube; // typedef data structure: used for domain_field

    /* ########################################
    // Loop over compartments
    // Assign and  allocate memory to openWQ variables: chemass
    ######################################## */
    for (unsigned int icmp=0;icmp<num_HydroComp;icmp++){
            
        // Dimensions for compartment icmp
        n_xyz[0] = std::get<2>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of x elements
        n_xyz[1] = std::get<3>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of y elements
        n_xyz[2] = std::get<4>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of z elements
        
        // Generate arma::cube of compartment icmp size
        arma::Cube<double> domain_xyz(n_xyz[0],n_xyz[1],n_xyz[2]);

        // Get number of species in compartment icmp
        HydroCmpName = std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(icmp));
        
        // Create arma for chemical species
        // Needs to be reset because each compartment can have a different number of compartments
        arma_fieldcube domain_field(numspec); // all species are simulated for all compartments

        /* ########################################
        // Loop over dimensions of compartment icmp
        // Push 3D arma::cube into the arma::field of each chemical species
        ######################################## */
        for (unsigned int s=0;s<numspec;s++){
            domain_field(s) = domain_xyz;
        }

        /* ########################################
        // Allocate Memory
        ######################################## */
        // OpenWQ state variable
        (*OpenWQ_vars.chemass)(icmp) = domain_field;
        // Hydro model variables
        (*OpenWQ_hostModelconfig.SM_space_hydromodel) = domain_xyz;
        (*OpenWQ_hostModelconfig.Tair_space_hydromodel) = domain_xyz;
    }

}

/* #################################################
// Read and Set simulations conditions and options
################################################# */
void OpenWQ_initiate::readSet(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_units& OpenWQ_units,
    const int icmp,
    const int ix,
    const int iy,
    const int iz,
    double igridcell_volume,  // all calculations assume unit = m3
    double iwater_volume){    // all calculations assume unit = m3
    
    /* #################################################
    // Read and set IC conditions
    ################################################# */
    setIC(
        OpenWQ_json,
        OpenWQ_vars,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_units,
        icmp,
        ix,
        iy,
        iz,
        igridcell_volume,   // all calculations assume unit = m3
        iwater_volume);     // all calculations assume unit = m3

}

/* #################################################
// Read and set IC conditions
################################################# */
void OpenWQ_initiate::setIC(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_units& OpenWQ_units,
    const int icmp,
    const int ix,
    const int iy,
    const int iz,
    double igridcell_volume,  // all calculations assume unit = m3
    double iwater_volume){    // all calculati

    // Local variables
    std::string chemname; // chemical name
    std::tuple<unsigned int,std::string,std::string> ic_info_i; // IC information in config file
    double ic_value; // IC value of chemical i
    std::string ic_type; // IC value type of chemical (mass or concentration)
    std::string ic_units; // // IC value units of chemical (e.g, kg/m3, mg/l))

    // Find compartment icmp name from code (host hydrological model)
    std::string CompName_icmp = std::get<1>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp)); // Compartment icomp name

    /* ########################################
    // Loop over chemical species
    ######################################## */

    for (unsigned int chemi=0;chemi<(OpenWQ_wqconfig.num_chem);chemi++){

        chemname = (OpenWQ_wqconfig.chem_species_list)[chemi]; // chemical name in BGC-json list

        // Get tuple with IC information for compartment CompName_icmp and chemical chemname
        // If not found in compartment icmp, it's because IC were not defined - set to zero.
        /* ########################################
        // IC conditions provided
        ######################################## */
        try{ 
            ic_info_i = 
                OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"][CompName_icmp]
                ["INITIAL_CONDITIONS"][chemname];

            ic_value = std::get<0>(ic_info_i);      // get IC value
            ic_type = std::get<1>(ic_info_i);       // get IC type
            ic_units = std::get<2>(ic_info_i);      // get IC units 

            // Transform units based on info provided
            OpenWQ_units.Convert_IC_Units(
                ic_value, // ic_value passed by reference so that it can be changed
                ic_type,
                ic_units);

            // Apply IC conditons (no need to handle error
            // Already done in OpenWQ_initiate::Transform_Units)
            /* ########################################
            // if CONCENTRATION
            ######################################## */
            if(ic_type.compare("concentration") == 0){
                (*OpenWQ_vars.chemass)(icmp)(chemi)(ix,iy,iz) =// units: g (basic units of MASS in openWQ)
                    ic_value // converted to mg/l (or g/m3) in OpenWQ_initiate::Transform_Units
                    * iwater_volume; // passed in m3
            }
            /* ########################################
            // if MASS
            ######################################## */
            else if(ic_type.compare("mass") == 0){
                (*OpenWQ_vars.chemass)(icmp)(chemi)(ix,iy,iz) =// units: g (basic units of MASS in openWQ)
                    ic_value // converted to mg/l (or g/m3) in OpenWQ_initiate::Transform_Units
                    * igridcell_volume; // passed in m3
            }

        }
        /* ########################################
        // IC conditions NOT provided set to ZERO
        ######################################## */
        catch(json::type_error& e){ 
            std::cout << "<OpenWQ> IC conditions not defined: set to zero " 
                    << "(compartment: " << CompName_icmp << ", " 
                    << "chemical: " << chemname << ")"
                    << std::endl;
            (*OpenWQ_vars.chemass)(icmp)(chemi)(ix,iy,iz) = 0.0f;
        }  
    }
}