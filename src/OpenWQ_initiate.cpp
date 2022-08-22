
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
    for (unsigned int icmp=0;icmp<OpenWQ_hostModelconfig.num_HydroComp;icmp++){
            
        // Dimensions for compartment icmp
        n_xyz[0] = std::get<2>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of x elements
        n_xyz[1] = std::get<3>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of y elements
        n_xyz[2] = std::get<4>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of z elements
        
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

        // Cumulative Derivatives (for export in debug mode)
        (*OpenWQ_vars.d_chemass_dt_chem_out)(icmp) = domain_field;
        (*OpenWQ_vars.d_chemass_dt_transp_out)(icmp) = domain_field;
        (*OpenWQ_vars.d_chemass_ss_out)(icmp) = domain_field;

        // Hydro model variables (water volumes for calc of concentrations)
        // Set them to ones in case concentrations are not requested
        // In such cases, the output will multiply by one (so it's fine)
        (*OpenWQ_hostModelconfig.waterVol_hydromodel).push_back(domain_xyz);

    }

    /* ########################################
    // Loop over EWF
    // Assign and  allocate memory to openWQ variables
    ######################################## */
    for (unsigned int ewfi=0;ewfi<OpenWQ_hostModelconfig.num_EWF;ewfi++){
            
        // Dimensions for compartment ewfi
        n_xyz[0] = std::get<2>(OpenWQ_hostModelconfig.HydroExtFlux.at(ewfi)); // num of x elements
        n_xyz[1] = std::get<3>(OpenWQ_hostModelconfig.HydroExtFlux.at(ewfi)); // num of y elements
        n_xyz[2] = std::get<4>(OpenWQ_hostModelconfig.HydroExtFlux.at(ewfi)); // num of z elements
        
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
    for (unsigned int depi=0;depi<OpenWQ_hostModelconfig.num_Depend;depi++){
            
        // Dimensions for compartment depi
        n_xyz[0] = std::get<2>(OpenWQ_hostModelconfig.HydroDepend.at(depi)); // num of x elements
        n_xyz[1] = std::get<3>(OpenWQ_hostModelconfig.HydroDepend.at(depi)); // num of y elements
        n_xyz[2] = std::get<4>(OpenWQ_hostModelconfig.HydroDepend.at(depi)); // num of z elements
        
        // Generate arma::cube of compartment depi size
        arma::Cube<double> domain_xyz(n_xyz[0],n_xyz[1],n_xyz[2]);

        // Set to zero
        domain_xyz.zeros();
        
        /* ########################################
        // Allocate Memory
        ######################################## */
        
        // OpenWQ state variable
        (*OpenWQ_hostModelconfig.dependVar).push_back(domain_xyz);
        (*OpenWQ_hostModelconfig.dependVar_scalar).push_back(0.0f);

    }

    
     /* ########################################
    // Log file
    ######################################## */

    // Create Log file name with full or relative path
    OpenWQ_wqconfig.LogFile_name.insert(0,"/");
    OpenWQ_wqconfig.LogFile_name.insert(0,
        OpenWQ_json.Master["OPENWQ_OUTPUT"]["RESULTS_FOLDERPATH"]);

    // Create log file
    std::ofstream outfile (OpenWQ_wqconfig.LogFile_name);

    // ###############
    // Write header in log file

    // Separator
    msg_string.clear();
    msg_string = "\n ############################## \n";
    OpenWQ_output.ConsoleLog(
        OpenWQ_wqconfig,    // for Log file name
        msg_string, // message
        false,              // print in console
        true);              // print in log file

    // PROJECT_NAME
    msg_string.clear();
    msg_string = "PROJECT_NAME: ";
    msg_string.append(OpenWQ_json.Master["PROJECT_NAME"]);
    OpenWQ_output.ConsoleLog(
        OpenWQ_wqconfig,    // for Log file name
        msg_string, // message
        false,              // print in console
        true);              // print in log file

    // GEOGRAPHICAL_LOCATION
    msg_string.clear();
    msg_string = "GEOGRAPHICAL_LOCATION: ";
    msg_string.append(OpenWQ_json.Master["GEOGRAPHICAL_LOCATION"]);
    OpenWQ_output.ConsoleLog(
        OpenWQ_wqconfig,    // for Log file name
        msg_string, // message
        false,              // print in console
        true);              // print in log file
    
    // AUTHORS
    msg_string.clear();
    msg_string = "AUTHORS: ";
    msg_string.append(OpenWQ_json.Master["AUTHORS"]);
    OpenWQ_output.ConsoleLog(
        OpenWQ_wqconfig,    // for Log file name
        msg_string, // message
        false,              // print in console
        true);              // print in log file

    // DATE
    msg_string.clear();
    msg_string = "DATE: ";
    msg_string.append(OpenWQ_json.Master["DATE"]);
    OpenWQ_output.ConsoleLog(
        OpenWQ_wqconfig,    // for Log file name
        msg_string, // message
        false,              // print in console
        true);              // print in log file

    // Separator
    msg_string.clear();
    msg_string = "\n ##############################\n ";
    OpenWQ_output.ConsoleLog(
        OpenWQ_wqconfig,    // for Log file name
        msg_string, // message
        false,              // print in console
        true);              // print in log file

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
    OpenWQ_output& OpenWQ_output){    // volume (water or soil), all calculations assume unit = m3
    
    // Local variables
    unsigned int nx, ny, nz;    // interactive compartment domain dimensions
    unsigned int ix, iy, iz;    // interactive compartment domain cell indexes

    
    /* #################################################
    // Read and set IC conditions
    ################################################# */
   
    for (unsigned int icmp=0;icmp<OpenWQ_hostModelconfig.num_HydroComp;icmp++){

        // Dimensions for compartment icmp
        nx = std::get<2>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of x elements
        ny = std::get<3>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of y elements
        nz = std::get<4>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of z elements

        // X, Y, Z loops
        #pragma omp parallel for private (ix, iy, iz) collapse (3) num_threads(OpenWQ_wqconfig.num_threads_requested)
        for (ix=0;ix<nx;ix++){
            for (iy=0;iy<ny;iy++){
                for (iz=0;iz<nz;iz++){

                    setIC(
                        OpenWQ_json,
                        OpenWQ_vars,
                        OpenWQ_hostModelconfig,
                        OpenWQ_wqconfig,
                        OpenWQ_units,
                        OpenWQ_output,
                        icmp,
                        ix,
                        iy,
                        iz,
                        (*OpenWQ_hostModelconfig.waterVol_hydromodel)[icmp](ix,iy,iz) );    // volume (water or soil) in m3

                }
            }
        }
    }

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
    OpenWQ_output& OpenWQ_output,
    const int icmp,
    const int ix,
    const int iy,
    const int iz,
    double i_volume){    // volume (water or soil)

    // Local variables
    std::string chemname;                       // chemical name
    std::tuple<double,std::string> ic_info_i;   // IC information in config file
    double ic_value;                            // IC value of chemical i
    std::string ic_units;                       // IC value units of chemical (e.g, kg/m3, mg/l))
    std::vector<std::string> units;             // units (numerator and denominator)
    std::vector<double> unit_multiplers;        // multiplers (numerator and denominator)
    std::string msg_string;                     // error/warning message string

    // Find compartment icmp name from code (host hydrological model)
    std::string CompName_icmp = std::get<1>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp)); // Compartment icomp name

    /* ########################################
    // Loop over chemical species
    ######################################## */

    for (unsigned int chemi=0;chemi<(OpenWQ_wqconfig.BGC_general_num_chem);chemi++){

        chemname = (OpenWQ_wqconfig.BGC_general_chem_species_list)[chemi]; // chemical name in BGC-json list

        // Get tuple with IC information for compartment CompName_icmp and chemical chemname
        // If not found in compartment icmp, it's because IC were not defined - set to zero.
        /* ########################################
        // IC conditions provided
        ######################################## */

        if (OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"].contains(CompName_icmp)){

       // try{ 
            ic_info_i = 
                OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"][CompName_icmp]
                ["INITIAL_CONDITIONS"][chemname];

            ic_value = std::get<0>(ic_info_i);      // get IC value
            ic_units = std::get<1>(ic_info_i);      // get IC units 

            // ########################################
            // Transform units based on info provided
            // 1) Calculate unit multiplers
            OpenWQ_units.Calc_Unit_Multipliers(
                OpenWQ_wqconfig,
                OpenWQ_output,
                unit_multiplers,    // multiplers (numerator and denominator)
                ic_units,           // input units
                units,
                true);              // direction of the conversion: 
                                    // to native (true) or 
                                    // from native to desired output units (false)
            // 2) Calculate value with new units
            OpenWQ_units.Convert_Units(
                ic_value,           // value passed by reference so that it can be changed
                unit_multiplers);   // units


            /* ########################################
            // Apply IC conditons (no need to handle error)
            // Already done in OpenWQ_initiate::Transform_Units)
            ######################################## */

            // If no demoninator, then it means that units are in mass and not concentration
            // so, multiplication by volume does not apply (so, set to 1)
            if (units[1].compare("EMPTY") == 0){
                i_volume = 1;}
            
            // Calculate state variable mass
            (*OpenWQ_vars.d_chemass_ic)(icmp)(chemi)(ix,iy,iz) =// units: g (basic units of MASS in openWQ)
                ic_value // converted to mg/l (or g/m3) in OpenWQ_initiate::Transform_Units
                * i_volume; // passed in m3 (can be volume of water of volume of soil)

        }

        /* ########################################
        // IC conditions NOT provided set to ZERO
        ######################################## */
        //catch(json::exception& e){ 
        else{
            // Create Message
            msg_string = 
                "<OpenWQ> IC conditions not defined: set to zero (compartment: " 
                + CompName_icmp + "; element: " 
                + std::to_string(ix) + ", " 
                + std::to_string(iy) + ", " 
                + std::to_string(iz) + "); chemical: " 
                + chemname + ")";

            // Print it (Console and/or Log file)
            OpenWQ_output.ConsoleLog(
                OpenWQ_wqconfig,    // for Log file name
                msg_string,         // message
                true,               // print in console
                true);              // print in log file
        }  
    }
}


/* #################################################
// Get Time variables
################################################# */
void OpenWQ_initiate::setTimeVars(
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    time_t simtime){

    // Update interaction number
    OpenWQ_hostModelconfig.interaction_step += 1;

    // Update other time variables
    if (OpenWQ_hostModelconfig.interaction_step == 1){

        // Initiate OpenWQ_wqconfig.nexttime_out with simtime
        // this is only applicable at the start of the simulation
        OpenWQ_wqconfig.nexttime_out = simtime;

        // Initiate time_step
        OpenWQ_hostModelconfig.time_step = 0.0f;
    }else{

        // Update time step and difference as fraction of 1 day (because reaction kinetics are
        // given is rate per day. Then update time_previous for next time step
        OpenWQ_hostModelconfig.time_step = (simtime - OpenWQ_wqconfig.time_previous);

    }

    // Update time previous
    OpenWQ_wqconfig.time_previous = simtime;

}