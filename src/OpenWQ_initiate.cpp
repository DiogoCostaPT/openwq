
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
        (*OpenWQ_vars.d_chemass_ewf)(icmp) = domain_field;

        // Cumulative Derivatives (for export in debug mode)
        (*OpenWQ_vars.d_chemass_dt_chem_out)(icmp) = domain_field;
        (*OpenWQ_vars.d_chemass_dt_transp_out)(icmp) = domain_field;
        (*OpenWQ_vars.d_chemass_ss_out)(icmp) = domain_field;
        (*OpenWQ_vars.d_chemass_ewf_out)(icmp) = domain_field;

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

        /* ########################################
        // EWF H5DF ExtFlux_FORC_HDF5vec
        ######################################## */
        std::vector<arma::Mat<double>> placeholder_timeStamps;
        std::vector<arma::Mat<double>> placeholder_ConcXYZ;

        (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec).push_back(placeholder_timeStamps);
        (*OpenWQ_wqconfig.ExtFlux_FORC_HDF5vec).push_back(placeholder_ConcXYZ);

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
    

}

/* #################################################
// Read and Set simulations conditions and options
################################################# */
void OpenWQ_initiate::readSet(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units& OpenWQ_units,
    OpenWQ_output& OpenWQ_output){    // volume (water or soil), all calculations assume unit = m3
        
    /* #################################################
    // Read and set IC conditions
    ################################################# */
   
    for (unsigned int icmp=0;icmp<OpenWQ_hostModelconfig.num_HydroComp;icmp++){

        setIC_driver(
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            OpenWQ_utils,
            OpenWQ_units,
            OpenWQ_output,
            icmp); 

    }

}

/* #################################################
// Read and set IC conditions
################################################# */
void OpenWQ_initiate::setIC_driver(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units& OpenWQ_units,
    OpenWQ_output& OpenWQ_output,
    const int icmp){

    // Local variables
    std::string chemname;
    std::string ic_data_format;        // interactive data format string
    std::string msg_string;                     // error/warning message string
    std::string errorMsgIdentifier;

    // Find compartment icmp name from code (host hydrological model)
    std::string CompName_icmp = std::get<1>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp)); // Compartment icomp name

    // Check if BIOGEOCHEMISTRY_CONFIGURATION key exists
    errorMsgIdentifier = " Config file";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Config, "BIOGEOCHEMISTRY_CONFIGURATION",
        errorMsgIdentifier);

    /* ########################################
    // Loop over chemical species
    ######################################## */

    for (unsigned int chemi=0;chemi<(OpenWQ_wqconfig.BGC_general_num_chem);chemi++){

        // Get chem name
        chemname = (OpenWQ_wqconfig.BGC_general_chem_species_list)[chemi]; // chemical name in BGC-json list

        // First default all IC chemical mass to zero, 
        // so that then we just need to change the ic entries requested
        (*OpenWQ_vars.d_chemass_ic)(icmp)(chemi).zeros();

        // Get tuple with IC information for compartment CompName_icmp and chemical chemname
        // If not found in compartment icmp, it's because IC were not defined - set to zero.
        /* ########################################
        // IC conditions provided
        ######################################## */

        if (OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"].contains(CompName_icmp)){
            
            ic_data_format = OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"][CompName_icmp]
                ["INITIAL_CONDITIONS"]["DATA_FORMAT"];

            if(ic_data_format.compare("JSON")==0){

                setIC_json(OpenWQ_json,
                    OpenWQ_vars, OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_utils,
                    OpenWQ_units,
                    OpenWQ_output,
                    icmp,
                    chemi);
                
            }else if(ic_data_format.compare("HDF5")==0){

                setIC_h5(OpenWQ_json,
                    OpenWQ_vars,
                    OpenWQ_hostModelconfig,
                    OpenWQ_wqconfig,
                    OpenWQ_utils,
                    OpenWQ_units,
                    OpenWQ_output,
                    icmp,
                    chemi);

            }else{

                // Create Message (Warning Message)
                msg_string = 
                    "<OpenWQ> WARNING: Unkown data format (" 
                    + ic_data_format
                    + ") for BIOGEOCHEMISTRY_CONFIGURATION > " + CompName_icmp 
                    + " > INITIAL_CONDITIONS "
                    + "(entry skipped)";

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);

                // not a valid entry
                continue;

            }
        }
        /* ########################################
        // IC conditions NOT provided set to ZERO
        ######################################## */
        //catch(json::exception& e){ 
        else{
            // Create Message
            msg_string = 
                "<OpenWQ> IC conditions not defined for compartment:" 
                + CompName_icmp + " and chemical:" 
                + chemname + " (set to zero)";

            // Print it (Console and/or Log file)
            OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true, true);
        }  
    }
}

/* #################################################
// Read and set IC conditions: JSON format
################################################# */
void OpenWQ_initiate::setIC_json(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units& OpenWQ_units,
    OpenWQ_output& OpenWQ_output,
    const int icmp,
    const int chemi){
    
    int nx, ny, nz;                             // interactive compartment domain dimensions       
    int ix, iy, iz;                             // interactive compartment domain cell indexes
    int ic_x;                                   // IC information: x location
    int ic_y;                                   // IC information: y location
    int ic_z;                                   // IC information: z location
    std::string ic_x_str;                       // IC information: x location (if "all")
    std::string ic_y_str;                       // IC information: y location (if "all")
    std::string ic_z_str;                        // IC information: z location (if "all")
    double ic_value;                            // IC value provided in json for chemical i
    double i_volume;                            // Volume of compartment if concentrations requested, otherwise set to 1 for no effect
    std::string ic_units;                       // IC value units of chemical (e.g, kg/m3, mg/l))
    std::vector<std::string> units;             // units (numerator and denominator)
    std::vector<double> unit_multiplers;        // multiplers (numerator and denominator)
    int num_icData;                             // interactive number of ic entries for each chemi within each icm
    json ic_info_i;                             // all IC info for a chemi within a icmp
    std::string msg_string;                     // error/warning message string
    std::string errorMsgIdentifier; 

    // Find compartment icmp name from code (host hydrological model)
    std::string CompName_icmp = std::get<1>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp)); // Compartment icomp name

    // Get chem name
    std::string chemname = (OpenWQ_wqconfig.BGC_general_chem_species_list)[chemi]; // chemical name in BGC-json list

    // Dimensions for compartment icmp
    nx = std::get<2>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of x elements
    ny = std::get<3>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of y elements
    nz = std::get<4>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of z elements

    // Check if BIOGEOCHEMISTRY_CONFIGURATION key exists
    errorMsgIdentifier = "Config file";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Config, "BIOGEOCHEMISTRY_CONFIGURATION",
        errorMsgIdentifier);

    errorMsgIdentifier = "Config file > BIOGEOCHEMISTRY_CONFIGURATION";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"], CompName_icmp,
        errorMsgIdentifier);

    errorMsgIdentifier = "Config file > BIOGEOCHEMISTRY_CONFIGURATION > " + CompName_icmp;
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"][CompName_icmp], "INITIAL_CONDITIONS",
        errorMsgIdentifier);

    errorMsgIdentifier = "Config file > BIOGEOCHEMISTRY_CONFIGURATION > " + CompName_icmp + "INITIAL_CONDITIONS";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"][CompName_icmp]["INITIAL_CONDITIONS"], "DATA",
        errorMsgIdentifier);

    errorMsgIdentifier = "Config file > BIOGEOCHEMISTRY_CONFIGURATION > " + CompName_icmp + "INITIAL_CONDITIONS > DATA";
    ic_info_i = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"][CompName_icmp]["INITIAL_CONDITIONS"]["DATA"],chemname,
        errorMsgIdentifier);

    num_icData = ic_info_i.size();
    
    // Loop over all IC entries within each chemical
    for (int nICchem=0;nICchem<num_icData;nICchem++){

        // ########################################
        // Get ic entry info, ix, iy, ix, value and units
        
        // Check if requesting to print "ALL"
        // ix
        ic_x = 0.0f;
        try{ // try if string input
            ic_x_str = ic_info_i[
                std::to_string(nICchem+1)].at(0);  // get IC x
            
            if(ic_x_str.compare("ALL") == 0){
                ic_x = -1;

            }else{
                // Create Message (Warning Message)
                msg_string = 
                    "<OpenWQ> WARNING: Unkown entry (" 
                    + ic_x_str
                    + ") for BIOGEOCHEMISTRY_CONFIGURATION > " + CompName_icmp 
                    + " > INITIAL_CONDITIONS > " +  chemname 
                    + " > " + std::to_string(nICchem+1)
                    + " (entry skipped)";

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(
                    OpenWQ_wqconfig,    // for Log file name
                    msg_string,         // message
                    true,               // print in console
                    true);              // print in log file

                // not a valid entry
                continue;
            
            }
        }catch(...){
            // if not "all", then use the value provided
            ic_x = ic_info_i[
                std::to_string(nICchem+1)].at(0);  // get IC x
            ic_x--; // substract 1 to get c++ index starting at 0
        }

        // iy
        ic_y = 0.0f;
        try{ // try if string input
            ic_y_str = ic_info_i[
                std::to_string(nICchem+1)].at(1);  // get IC y
            
            if(ic_x_str.compare("ALL") == 0){
                ic_y = -1;

            }else{
                // Create Message (Warning Message)
                msg_string = 
                    "<OpenWQ> WARNING: Unkown entry (" 
                    + ic_y_str
                    + ") for BIOGEOCHEMISTRY_CONFIGURATION > " + CompName_icmp 
                    + " > INITIAL_CONDITIONS > " +  chemname 
                    + " > " + std::to_string(nICchem+1)
                    + " (entry skipped";

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(
                    OpenWQ_wqconfig,    // for Log file name
                    msg_string,         // message
                    true,               // print in console
                    true);              // print in log file

                // not a valid entry
                continue;
            
            }
        }catch(...){
            // if not "all", then use the value provided
            ic_y = ic_info_i[
                std::to_string(nICchem+1)].at(1);  // get IC y
            ic_y--; // substract 1 to get c++ index starting at 0
        }

        // iz
        ic_z = 0.0f;
        try{ // try if string input
            ic_z_str = ic_info_i[
                std::to_string(nICchem+1)].at(2);  // get IC z
            
            if(ic_x_str.compare("ALL") == 0){
                ic_z = -1;

            }else{
                // Create Message (Warning Message)
                msg_string = 
                    "<OpenWQ> WARNING: Unkown entry (" 
                    + ic_x_str
                    + ") for BIOGEOCHEMISTRY_CONFIGURATION > " + CompName_icmp 
                    + " > INITIAL_CONDITIONS > " +  chemname 
                    + " > " + std::to_string(nICchem+1)
                    + "(entry skipped";

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(
                    OpenWQ_wqconfig,    // for Log file name
                    msg_string,         // message
                    true,               // print in console
                    true);              // print in log file

                // not a valid entry
                continue;
            
            }
        }catch(...){
            // if not "all", then use the value provided
            ic_z = ic_info_i[
                std::to_string(nICchem+1)].at(2);  // get IC z
            ic_z--; // substract 1 to get c++ index starting at 0
        }

        // get IC value
        ic_value = ic_info_i[
                std::to_string(nICchem+1)].at(3);

        // get IC units      
        ic_units = ic_info_i[
                std::to_string(nICchem+1)].at(4);    

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

        // X, Y, Z loops
        #pragma omp parallel for private (ix, iy, iz) collapse (3) num_threads(OpenWQ_wqconfig.num_threads_requested)
        for (ix=0;ix<nx;ix++){
            for (iy=0;iy<ny;iy++){
                for (iz=0;iz<nz;iz++){
                        
                    // Calculate state variable mass for the requested cells
                    if((ic_x == ix || ic_x == -1) 
                        && (ic_y == iy || ic_y == -1)
                        && (ic_z == iz || ic_z == -1) ){

                            // If no demoninator, then it means that units are in mass and not concentration
                            // so, multiplication by volume does not apply (so, set to 1)
                            if (units[1].compare("EMPTY") == 0){
                                i_volume = 1.0f;
                            }else{
                                i_volume = (*OpenWQ_hostModelconfig.waterVol_hydromodel)[icmp](ix,iy,iz);
                            }
                            
                            // Get IC mass
                            (*OpenWQ_vars.d_chemass_ic)(icmp)(chemi)(ix,iy,iz) =// units: g (basic units of MASS in openWQ)
                                ic_value        // converted to mg/l (or g/m3) in OpenWQ_initiate::Transform_Units
                                * i_volume;     // passed in m3 (can be volume of water of volume of soil)
                    }

                }
            }
        }
    }
}

/* #################################################
// Read and set IC conditions: H5 format
################################################# */
void OpenWQ_initiate::setIC_h5(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units& OpenWQ_units,
    OpenWQ_output& OpenWQ_output,
    const int icmp,
    const int chemi){

    // Local variables
    int ic_x, ic_y, ic_z, nx, ny, nz, irow;
    double ic_value;                            // IC value provided in json for chemical i
    double i_volume;                            // Volume of compartment if concentrations requested, otherwise set to 1 for no effect
    std::string msg_string;
    std::string ic_h5_folderPath;
    std::string ic_h5_timestamp;
    std::string ic_filenamePath;
    std::string ic_h5_units;
    std::vector<std::string> units;             // units (numerator and denominator)
    std::vector<double> unit_multiplers;        // multiplers (numerator and denominator)
    std::size_t it;
    arma::mat xyzIC_h5;
    arma::mat dataIC_h5;
    std::string errorMsgIdentifier;
    json json_ic_h5_icmp_SubStruct;

    // Find compartment icmp name from code (host hydrological model)
    std::string CompName_icmp = std::get<1>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp)); // Compartment icomp name

    // Dimensions for compartment icmp
    nx = std::get<2>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of x elements
    ny = std::get<3>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of y elements
    nz = std::get<4>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // num of z elements

    // Get chem name
    std::string chemname = (OpenWQ_wqconfig.BGC_general_chem_species_list)[chemi]; // chemical name in BGC-json list

    // Check BIOGEOCHEMISTRY_CONFIGURATION keys exists
    errorMsgIdentifier = "Config file";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Config, "BIOGEOCHEMISTRY_CONFIGURATION",
        errorMsgIdentifier);

    errorMsgIdentifier = "Config file > BIOGEOCHEMISTRY_CONFIGURATION";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"], CompName_icmp,
        errorMsgIdentifier);

    errorMsgIdentifier = "Config file > BIOGEOCHEMISTRY_CONFIGURATION > " + CompName_icmp;
    json_ic_h5_icmp_SubStruct = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"][CompName_icmp], "INITIAL_CONDITIONS",
        errorMsgIdentifier);

    // h5 IC folder path
    errorMsgIdentifier = "Config file > BIOGEOCHEMISTRY_CONFIGURATION > " + CompName_icmp + "INITIAL_CONDITIONS";
    ic_h5_folderPath = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        json_ic_h5_icmp_SubStruct,"FOLDERPATH",
        errorMsgIdentifier);

    // h5 ic timestamp
    ic_h5_timestamp = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        json_ic_h5_icmp_SubStruct ,"TIMESTAMP",
        errorMsgIdentifier);

    // h5 ic units
    ic_h5_units = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        json_ic_h5_icmp_SubStruct ,"UNITS",
        errorMsgIdentifier);

    // replace "/" by "|" is needed because "/" is not compatible with directory full paths
    it = (int) ic_h5_units.find("/");
    if (it <= ic_h5_units.size()){
        ic_h5_units.replace(it,1, "|");
     }

    // Generate full ic filename
    ic_filenamePath = ic_h5_folderPath;

    ic_filenamePath.append("/");
    ic_filenamePath.append(CompName_icmp); // compartment
    ic_filenamePath.append("@");
    ic_filenamePath.append(chemname);     // chemical name
    ic_filenamePath.append("#");
    ic_filenamePath.append(ic_h5_units); // units
    ic_filenamePath.append("-main.h5"); 

    // Get unit conversion multipliers
    OpenWQ_units.Calc_Unit_Multipliers(
        OpenWQ_wqconfig,
        OpenWQ_output,
        unit_multiplers,    // multiplers (numerator and denominator)
        ic_h5_units,           // input units
        units,
        true);              // direction of the conversion: 
                            // to native (true) or 
                            // from native to desired output units (false)

    // Get x,y,z elements in h5 ic data
    xyzIC_h5
        .load(arma::hdf5_name(
            ic_filenamePath,            // file name
            "xyz_elements"));          // options

    // Get the corresponding data
    dataIC_h5
        .load(arma::hdf5_name(
            ic_filenamePath,            // file name
            ic_h5_timestamp));          // options

    // If no data found for requested ic_h5_timestamp
    // Then, through warning message and return
    if(dataIC_h5.is_empty() == true){

        // Create Message
        msg_string = 
            "<OpenWQ> WARNING: IC conc not found for timestamp='"
            + ic_h5_timestamp
            + "' in h5 file='" + ic_filenamePath
            + "'. Request ignored and IC concentrations defaulted to zero for all chemicals of "
            "compartment=" + OpenWQ_hostModelconfig.cmpt_names[icmp];

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig,msg_string,true,true);

        return;
    }

    // Saving IC in state variable 
    #pragma omp parallel for private (irow, ic_x, ic_y, ic_z) num_threads(OpenWQ_wqconfig.num_threads_requested)
    for (irow=0;irow<(int)xyzIC_h5.n_rows-1;irow++){

        // Get ix, iy, iz IC locations
        // Output data starts at 1, but openwq (c++) starts at 0
        ic_x = xyzIC_h5(irow, 0) - 1;
        ic_y = xyzIC_h5(irow, 1) - 1;
        ic_z = xyzIC_h5(irow, 2) - 1;
            
        // Calculate state variable mass for the requested cells
        if((ic_x >= 0 && ic_x < nx) 
            && (ic_y >= 0 && ic_y < ny)
            && (ic_z >= 0 && ic_z < nz) ){

            ic_value = dataIC_h5(irow);

            // Convert units if needd
            OpenWQ_units.Convert_Units(
                ic_value,           // value passed by reference so that it can be changed
                unit_multiplers);   // units

            // If no demoninator, then it means that units are in mass and not concentration
            // so, multiplication by volume does not apply (so, set to 1)
            if (units[1].compare("EMPTY") == 0){
                i_volume = 1.0f;
            }else{
                i_volume = (*OpenWQ_hostModelconfig.waterVol_hydromodel)[icmp](ic_x,ic_y,ic_z);
            }
            
            // Get IC mass
            (*OpenWQ_vars.d_chemass_ic)(icmp)(chemi)(ic_x,ic_y,ic_z) =// units: g (basic units of MASS in openWQ)
                ic_value        // converted to mg/l (or g/m3) in OpenWQ_initiate::Transform_Units
                * i_volume;     // passed in m3 (can be volume of water of volume of soil)
        
        }else{
            
            // Through a warning if IC request is out of boundaries
            //Create message
            msg_string = 
                "<OpenWQ> WARNING: IC h5-format data load out of boundaries."
                "Requested load ignored: "
                "Compartment=" + OpenWQ_hostModelconfig.cmpt_names[icmp]
                + ", Chemical=" + OpenWQ_wqconfig.BGC_general_chem_species_list[chemi]
                + ", ix=" + std::to_string(ic_x)
                + ", iy=" + std::to_string(ic_y)
                + ", iz=" + std::to_string(ic_z);

            // Print it (Console and/or Log file)
             OpenWQ_output.ConsoleLog(OpenWQ_wqconfig,msg_string,true,true);

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