
// Copyright 2020, Diogo Costa, diogo.pinhodacosta@canada.ca
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

#include "OpenWQ_readjson.h"


/* #################################################
 // Real all configuration files
 ################################################# */
void OpenWQ_readjson::read_all(
    OpenWQ_json &OpenWQ_json,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig &OpenWQ_wqconfig,
    OpenWQ_units &OpenWQ_units,
    OpenWQ_output& OpenWQ_output)
{

    /* ################################################
    // Read JSON files
    ################################################ */

    // read
    OpenWQ_readjson::read_JSON_2class(
        OpenWQ_wqconfig,
        OpenWQ_output,
        OpenWQ_json.Master,                 // JSON structure to save to
        false,                              // Save in subfield of JSON structure? only if multiple files (e.g., source and sinks)
        "",                                 // if above true, provide name of subfield
        OpenWQ_wqconfig.OpenWQ_masterjson); // Name of JSON file

    // ########################
    // Main confirguration json (read)
    OpenWQ_readjson::read_JSON_2class(
        OpenWQ_wqconfig,
        OpenWQ_output,
        OpenWQ_json.Config,
        false,
        "",
        OpenWQ_json.Master["OPENWQ_INPUT"]["CONFIG_FILEPATH"]);

    // ########################
    // SinkSource json (read)openWQ_OUTPUT
    unsigned int num_ssf = OpenWQ_json.Master["OPENWQ_INPUT"]["SINKSOURCE_FILEPATHS"].size();
    for (unsigned int ssf = 0; ssf < num_ssf; ssf++)
    {
        OpenWQ_readjson::read_JSON_2class(
            OpenWQ_wqconfig,
            OpenWQ_output,
            OpenWQ_json.SinkSource,
            true,
            std::to_string(ssf + 1),
            OpenWQ_json.Master["OPENWQ_INPUT"]["SINKSOURCE_FILEPATHS"][std::to_string(ssf + 1)]["FILEPATH"]);
    }

    /* ########################
    // MODULES
    ########################### */

    // ########################
    // BGC_module cycling json (read)
    OpenWQ_readjson::read_JSON_2class(
        OpenWQ_wqconfig,
        OpenWQ_output,
        OpenWQ_json.BGC_module,
        false,
        "",
        OpenWQ_json.Master["MODULES"]["BIOGEOCHEMISTRY"]["MODULE_CONFIG_FILEPATH"]);


    // ########################
    // TE_module cycling json (read)
    OpenWQ_readjson::read_JSON_2class(
        OpenWQ_wqconfig,
        OpenWQ_output,
        OpenWQ_json.TE_module,
        false,
        "",
        OpenWQ_json.Master["MODULES"]["TRANSPORT_EROSION"]["MODULE_CONFIG_FILEPATH"]);

    
    
    /* ################################################
    // Process some of the openWQ config info
    ################################################ */
    OpenWQ_readjson::SetConfigInfo(
        OpenWQ_json,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_units,
        OpenWQ_output);

}

/* #################################################
// Read JSON file to class
################################################# */
void OpenWQ_readjson::read_JSON_2class(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_output& OpenWQ_output,
    json &jsondata,                       // JSON structure to save to
    const bool substruc_flag,             // Save in subfield of JSON structure? only if multiple files (e.g., source and sinks)
    const std::string JsonSubStruct_name, // if true, name of subfield
    const std::string jsonfile)
{ // Name of JSON file

    // Local Variables
    std::string msg_string;             // error/warning message string

    /* ####################################
    // Save JSON file in JSON data structure
    #################################### */

    try
    {
        // Read json file with ifstrem
        std::ifstream f(jsonfile);

        //* ##########################
        // If substruc_flag = true
        // save JSON inside JsonSubStruct_name (which is actually of int type) field in jsondata
        //* ####################################
        if (substruc_flag == false)
        {
            // Save ifstream to JSON data type
            jsondata = json::parse(f,
                /* callback */ nullptr,
                /* allow exceptions */ false,
                /* skip_comments */ true);

            // Convert all text in JSON to upper case
            OpenWQ_readjson::ConvertJSONtext_2upperCase(
                jsondata);
        }
        else
        {
            // Save ifstream to JSON data type
            jsondata[JsonSubStruct_name] = json::parse(f,
                /* callback */ nullptr,
                /* allow exceptions */ false,
                /* skip_comments */ true);
                    
            // Convert all text in JSON to upper case
            OpenWQ_readjson::ConvertJSONtext_2upperCase(
                jsondata[JsonSubStruct_name]);
        }
    }
    /* ####################################
// Handle exceptions 
#################################### */
    catch (const std::exception &e)
    {
        
        // Create Message
        msg_string = 
            "<OpenWQ> ERROR: An exception occurred parsing JSON file: " 
            + jsonfile;

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(
            OpenWQ_wqconfig,    // for Log file name
            msg_string,         // message
            true,               // print in console
            true);              // print in log file
        
        // Abort (Fatal error)
        exit(EXIT_FAILURE);
    }
}


/* ########################################
// Convert JSON text to Upper Case
######################################### */
void OpenWQ_readjson::ConvertJSONtext_2upperCase(
    json &jsondata){

    // Local Variables
    std::string old_jsonkey_layer_1;
    std::string old_jsonkey_layer_2;
    std::string old_jsonkey_layer_3;
    std::string old_jsonkey_layer_4;
    std::string old_jsonkey_layer_5;

    std::string new_jsonkey_layer_1;
    std::string new_jsonkey_layer_2;
    std::string new_jsonkey_layer_3;
    std::string new_jsonkey_layer_4;
    std::string new_jsonkey_layer_5;
    
    // ########################################
    // Identify all keys in JSON tree data and replace ny upper case keys

    // ------------> First layer of keys
    for (auto& x1 : jsondata.items()){

        try{
                
            old_jsonkey_layer_1 = x1.key();
            
            // Save key to o if a key
            if (!old_jsonkey_layer_1.empty()){ // if not null
                OpenWQ_readjson::change_JSON_key_to_upper_case(
                    jsondata, 
                    old_jsonkey_layer_1,
                    new_jsonkey_layer_1);
            }else{
                continue;
            }
            
            // -------------> Second possible layer of keys
            for (auto& x2 : jsondata[new_jsonkey_layer_1].items()){
                    
                try{
                    
                    old_jsonkey_layer_2 = x2.key();

                    if (!old_jsonkey_layer_2.empty()&& // if not null
                        jsondata[new_jsonkey_layer_1].type() != json::value_t::array){ // and not array
                        
                        OpenWQ_readjson::change_JSON_key_to_upper_case(
                            jsondata[new_jsonkey_layer_1], 
                            old_jsonkey_layer_2,
                            new_jsonkey_layer_2);
                    }else{
                        OpenWQ_readjson::change_JSON_value_to_upper_case(
                            jsondata,
                            new_jsonkey_layer_1);
                        continue;
                    }

                    // -------------> Third possible layer of keys
                    for (auto& x3 : jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2].items()){
                                
                        try{
                        
                            old_jsonkey_layer_3 = x3.key();

                            if (!old_jsonkey_layer_3.empty()&& // if not null
                                jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2].type() != json::value_t::array){ // and not array
                                
                                OpenWQ_readjson::change_JSON_key_to_upper_case(
                                    jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2], 
                                    old_jsonkey_layer_3,
                                    new_jsonkey_layer_3);
                            }else{
                                OpenWQ_readjson::change_JSON_value_to_upper_case(
                                    jsondata[new_jsonkey_layer_1],
                                    new_jsonkey_layer_2);
                                continue;
                            }

                            // -------------> Third possible layer of keys
                            for (auto& x4 : jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3].items()){
                                    
                                try{
                                
                                    old_jsonkey_layer_4 = x4.key();

                                    if (!old_jsonkey_layer_4.empty() && // if not null
                                        jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3].type() != json::value_t::array){ // and not array
                                        
                                        OpenWQ_readjson::change_JSON_key_to_upper_case(
                                            jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3], 
                                            old_jsonkey_layer_4,
                                            new_jsonkey_layer_4);
                                    }else{
                                        OpenWQ_readjson::change_JSON_value_to_upper_case(
                                        jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2],
                                        new_jsonkey_layer_3);
                                    continue;
                                    }   

                                    // -------------> Fourth possible layer of keys
                                    for (auto& x5 : jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4].items()){
                            
                                        try{
                                        
                                            old_jsonkey_layer_5 = x5.key();

                                            if (!old_jsonkey_layer_5.empty() && // if not null
                                                jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4].type() != json::value_t::array){ // and not array
                                                
                                                OpenWQ_readjson::change_JSON_key_to_upper_case(
                                                    jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4], 
                                                    old_jsonkey_layer_5,
                                                    new_jsonkey_layer_5);
                                            }else{
                                                OpenWQ_readjson::change_JSON_value_to_upper_case(
                                                jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3],
                                                new_jsonkey_layer_4);
                                                continue;
                                            }

                                        }catch (...){}
                                    }                                   
                                        
                                }catch (...){}
                            }

                        }catch (...){}
                    }
                            
                }catch (...){}
            }

        }catch (...){}

    }
    

}

// ########################################
// Change JSON key
// ########################################
void OpenWQ_readjson::change_JSON_key_to_upper_case(
    json &object, 
    const std::string& old_key,
    std::string& new_key)
{

    try{
        OpenWQ_readjson::ConvertStringToUpperCase(
            old_key,
            new_key);

        // get iterator to old key; TODO: error handling if key is not present
        json::iterator it = object.find(old_key);

        // create null value for new key and swap value from old key
        std::swap(object[new_key], it.value());

    }catch(const std::exception &e){}

}

// ########################################
// Change JSON value
// ########################################
void OpenWQ_readjson::change_JSON_value_to_upper_case(
    json &object,
    std::string new_jsonkey_layer_1){
    
    // Local variables
    long num_vals;
    bool array_flag = false;


    // Vector of words to keep with native upper/lower case provided by user
    std::vector<std::string> exclude_vales;
    exclude_vales.push_back("FILEPATH");
    exclude_vales.push_back("FOLDERPATH");
    exclude_vales.push_back("MODULE_CONFIG_FILEPATH");
    unsigned int num_exclude_vales = exclude_vales.size();

    // if FILEPATH or FOLDERPATH, do not change to upper case
    for (unsigned int w=0;w<num_exclude_vales;w++){

        if (new_jsonkey_layer_1.find(exclude_vales[w]) != std::string::npos){
                return;
        }

    }

    // check if object is an array
    if(object[new_jsonkey_layer_1].type() == json::value_t::array)
        array_flag = true;        
    
    // ########################################
    // Replace elements by upper case version
    // ########################################

    // if string (not array or tuple) - most of the cases
    if (array_flag != true){ // check if value is array

        // Local variables
        
        std::string new_value;
        std::string old_value;

        num_vals = 1;
        old_value = object[new_jsonkey_layer_1];

        OpenWQ_readjson::ConvertStringToUpperCase(
                old_value,
                new_value);    

        // replace value
        object[new_jsonkey_layer_1].swap(new_value);         

    // If value is an array or tuple 
    // (loop over the array elements to replace lower case string to upper case strings
    }else{

        // initiate json arrays and get existing one 
        json::array_t old_value_array = object[new_jsonkey_layer_1];
        json::array_t new_value_array;
        
        // size of array
        num_vals = old_value_array.size();

        // Loop over number of values
        // if NOT FILEPATH or FOLDERPATH, change string to Upper Case
        for (unsigned int i=0;i<num_vals;i++){
                
            // Get element in array/tuple
            auto old_value = old_value_array.at(i);
            
            // if string
            try{         
                std::string new_value_str;

                // Convert to upper case
                OpenWQ_readjson::ConvertStringToUpperCase(
                    old_value,
                    new_value_str);

                // add string to new_value_array
                new_value_array.push_back(new_value_str);

            // if num
            }catch(...){
                double new_value_num = old_value;
                
                // add num to new_value_array
                new_value_array.push_back(new_value_num);
            }

        }                        

        // replace value
        object[new_jsonkey_layer_1].swap(new_value_array); 

    }

}

// ########################################
// Converts string text to lower case
// ########################################

void OpenWQ_readjson::ConvertStringToUpperCase(
    const std::string &old_jsonkey,
    std::string &new_jsonkey)
{
    new_jsonkey = old_jsonkey;

    // Try to transform to upper case if text
    try{
        std::transform(
        new_jsonkey.begin(),
        new_jsonkey.end(),
        new_jsonkey.begin(), // Convert to lower case to avoid issues
        [](unsigned char c) { return std::toupper(c); });
    }catch (...){}
}

// ########################################
// Extract model configuration info from JSON files
// ########################################
void OpenWQ_readjson::SetConfigInfo(
    OpenWQ_json &OpenWQ_json,
    OpenWQ_hostModelconfig & OpenWQ_hostModelconfig,
    OpenWQ_wqconfig &OpenWQ_wqconfig,
    OpenWQ_units &OpenWQ_units,
    OpenWQ_output& OpenWQ_output){

    // Local Variables
    std::tuple<unsigned int, std::string> time_tuple;       // IC information in config file
    std::string output_format;                              // format of output (CSV, VTK)
    long num_chem2print;                                    // number of chemicals to print based on openWQ_OUTPUT               
    long num_compt2print;                                   // number of compartments to print
    long num_cells2print;                                   // iteractive number of cells to print for each compartment
    std::vector<std::string> compt_names_vec;               // compartment names to print
    std::string chem_name2print;                            // iteractive chem name from openWQ_OUTPUT
    std::string chem_namelist;                              // iteractive chem name from BGC list
    std::string CompName_icmp;                              // iteractive compartment name from list
    std::string compt_name2print;                           // iteractive compartment name to print
    std::string cells_input;                                // interactive string for cells input for each compartment
    arma::mat cells2print_cmpt;                             // cumulative vector with all cells to print for each compartment
    arma::mat cells2print_row(1,3,arma::fill::zeros);       // iteractive vector with cell x, y and z indexes
    unsigned int ix_json;                                            // iteractive ix info for COMPARTMENTS_AND_CELLS cell data 
    unsigned int iy_json;                                            // iteractive iy info for COMPARTMENTS_AND_CELLS cell data  
    unsigned int iz_json;                                            // iteractive iz info for COMPARTMENTS_AND_CELLS cell data  
    unsigned int nx;                                                 // interactive nx inforationfor each compartment
    unsigned int ny;                                                 // interactive ny inforationfor each compartment
    unsigned int nz;                                                 // interactive nz inforationfor each compartment
    std::vector<int>::iterator it;                          // iteractor for flagging if compartment i has been selected for printing
    std::vector<double> unit_multiplers;                    // multiplers (numerator and denominator)
    bool volume_unit_flag;                // flag to note if denominator is a volume (needed for calculation of concentration in output)
    std::string num_cores_input;                            // input of threads info as string
    bool threads_input_err_flag = false;                    // flag for error in threads input (initiate a false, no error)
    std::string msg_string;                                 // error/warning message string
    
    // ########################################
    // COMPUTATION SETTINGS
    // ########################################

    // Get number of threads in the system
    OpenWQ_wqconfig.num_threads_system = sysconf(_SC_NPROCESSORS_ONLN);
    
    // Get number of threads requested
    // It can be provided as an int or "all" (string)
    try{
        // Test if provided as integer with number of cores
        // Convert cast to int (if double or flow)
        OpenWQ_wqconfig.num_threads_requested = (int) OpenWQ_json.Master["COMPUTATIONAL_SETTINGS"]["USE_NUM_THREADS"];

    }catch(...){
        
        // Check if equal to "ALL"
        try{
            // If not integer, check if string
            num_cores_input = OpenWQ_json.Master["COMPUTATIONAL_SETTINGS"]["USE_NUM_THREADS"];
        
            // If string check if it is a string equal to "ALL"
            if (num_cores_input.compare("ALL") == 0){

                OpenWQ_wqconfig.num_threads_requested = OpenWQ_wqconfig.num_threads_system;

            }else{
                // Set error flag = true
                threads_input_err_flag = true;
            }
        }catch(...){
             // Set error flag = true
            threads_input_err_flag = true;
        }

        // Error handling 
        // if not "ALL" get integer. Throw error if not integer either
        if (threads_input_err_flag == true){

            // Create Message
            msg_string =
                 "<OpenWQ> ERROR: Unrecognized input for COMPUTATIONAL_SETTINGS > USE_NUM_THREADS: "
                + num_cores_input
                + "(Only allowed \"all\" or integer).";

            // Print it (Console and/or Log file)
            OpenWQ_output.ConsoleLog(
                OpenWQ_wqconfig,    // for Log file name
                msg_string,         // message
                true,               // print in console
                true);              // print in log file

            // Abort 
            exit (EXIT_FAILURE);
        }
    }
    
    // Check if number of threads requested is higher than existing in the system
    // if yes, throw warning message and limited the request
    if (OpenWQ_wqconfig.num_threads_system < OpenWQ_wqconfig.num_threads_requested){

        // Create Message
        msg_string = 
            "<OpenWQ> WARNING: Number of threads in the system (" 
            + std::to_string(OpenWQ_wqconfig.num_threads_system)
            + ") is lower than requested ("
            + std::to_string(OpenWQ_wqconfig.num_threads_requested)
            + "). All system threads have been engaged.";

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(
            OpenWQ_wqconfig,    // for Log file name
            msg_string,         // message
            true,               // print in console
            true);              // print in log file
        
        // Overwrite OpenWQ_wqconfig.num_threads_requested to max number in the system
        OpenWQ_wqconfig.num_threads_requested = OpenWQ_wqconfig.num_threads_system;
        
    }else{

        // Create Message
        msg_string = 
            "<OpenWQ> INFO: Number of threads requested and used: " 
            + std::to_string(OpenWQ_wqconfig.num_threads_requested);

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(
            OpenWQ_wqconfig,    // for Log file name
            msg_string,         // message
            true,               // print in console
            true);              // print in log file

    }
    
    // ########################################
    // HOST MODEL
    // ########################################

    // Number of hydrological compartments in host model
    OpenWQ_hostModelconfig.num_HydroComp = OpenWQ_hostModelconfig.HydroComp.size(); 

    
    // ###########################################
    // MODULES
    // ###########################################

    // ###########################
    // TRANSPORT / INTERNAL MOBILIZATION / INTERNAL EROSION
    // Mobilization of immobile species (turning them into suspended)

    // Get method
    std::string TE_method_local = (OpenWQ_json.Master["MODULES"]["TRANSPORT_EROSION"]["MODULE_NAME"]);
    (OpenWQ_wqconfig.TE_module).append(TE_method_local);

    // Load information fo the method
    if ((OpenWQ_wqconfig.TE_module).compare("OPENWQ_NATIVE_TE") == 0 
        || (OpenWQ_wqconfig.TE_module).compare("OPENWQ_NATIVE_TE_NO_ADVDISP") == 0){
        
        // Get Erodibility coeficients for native IntMob function 
        std::vector<double> K_Erodib_K_local = OpenWQ_json.TE_module["INTMOB_CONFIGURATION"]["K_VAL"];
        OpenWQ_wqconfig.OpenWQ_TE_native_IntMob_Erodib_K.insert( OpenWQ_wqconfig.OpenWQ_TE_native_IntMob_Erodib_K.end(), K_Erodib_K_local.begin(), K_Erodib_K_local.end());
    
        // Get info for BoundMix function
        // Get number of entries
        int num_entries = OpenWQ_json.TE_module["BOUNDMIX_CONFIGURATION"].size();


        for (unsigned int entry_i = 0; entry_i < num_entries; entry_i++){

            // Strings for inputs
            std::string input_direction;    // user input
            std::string input_upper_compartment;    // user input
            std::string input_lower_compartment;    // user input
            double input_k_val;                     // user input

            // iteractive variables
            std::string icmp_i_name;
            unsigned int input_direction_index;
            unsigned int input_upper_compartment_index;
            unsigned int input_lower_compartment_index;
            bool input_upper_compartment_index_exist;
            bool input_lower_compartment_index_exist;

            // Get entries
            input_direction = OpenWQ_json.TE_module["BOUNDMIX_CONFIGURATION"][std::to_string(entry_i + 1)]["DIRECTION"];
            input_upper_compartment = OpenWQ_json.TE_module["BOUNDMIX_CONFIGURATION"][std::to_string(entry_i + 1)]["UPPER_COMPARTMENT"];
            input_lower_compartment = OpenWQ_json.TE_module["BOUNDMIX_CONFIGURATION"][std::to_string(entry_i + 1)]["LOWER_COMPARTMENT"];
            input_k_val = OpenWQ_json.TE_module["BOUNDMIX_CONFIGURATION"][std::to_string(entry_i + 1)]["K_VAL"];

            // Get corresponding indexes
            
            // 1) input_direction
            if (input_direction.compare("X") == 0){
                input_direction_index = 0;
            }else if (input_direction.compare("Y") == 0){
                input_direction_index = 1;
            }else if (input_direction.compare("Z") == 0){
                input_direction_index = 2;
            }else{

                // Create Message (Warning Message)
                msg_string = 
                    "<OpenWQ> WARNING: BOUNDMIX module - unkown 'UPPER_COMPARTMENT'or 'LOWER_COMPARTMENT' in entry = "
                    + std::to_string(entry_i);

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(
                    OpenWQ_wqconfig,    // for Log file name
                    msg_string,         // message
                    true,               // print in console
                    true);              // print in log file

                // Ignore entry
                continue;

            }

            // 2) upper compartment AND lower compartment index
            // Loop through native compartment names
            for (unsigned int icmp_i = 0; icmp_i < OpenWQ_hostModelconfig.num_HydroComp; icmp_i++)
            {

                // Upper compartment: index
                if (input_upper_compartment.compare(std::get<1>(OpenWQ_hostModelconfig.HydroComp[icmp_i])) == 0){
                    input_upper_compartment_index = icmp_i;
                    input_upper_compartment_index_exist = true;
                }

                // Lower compartment: index
                if (input_lower_compartment.compare(std::get<1>(OpenWQ_hostModelconfig.HydroComp[icmp_i])) == 0){
                    input_lower_compartment_index = icmp_i;
                    input_lower_compartment_index_exist = true;
                }

            }

            // Create Message (Warning Message) if compartments provided don't exist
            if (input_upper_compartment_index_exist == false 
                && input_lower_compartment_index_exist == false){
                 
                msg_string = 
                    "<OpenWQ> WARNING: BOUNDMIX module - unkown 'DIRECTION' = " 
                    + input_direction
                    + "in entry = "
                    + std::to_string(entry_i);

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(
                    OpenWQ_wqconfig,    // for Log file name
                    msg_string,         // message
                    true,               // print in console
                    true);              // print in log file

                // Ignore entry
                continue;

            }
                
            // Add values to tuple
            OpenWQ_wqconfig.OpenWQ_TE_native_BoundMix_info.push_back(
                std::tuple<unsigned int,unsigned int,unsigned int,double>(
                    input_direction_index,
                    input_upper_compartment_index,
                    input_lower_compartment_index,
                    input_k_val
                )
            );

        }
    }


    // #############################
    // CHEMISTRY

    // Get method
    std::string BGC_method_local = (OpenWQ_json.Master["MODULES"]["BIOGEOCHEMISTRY"]["MODULE_NAME"]);
    (OpenWQ_wqconfig.BGC_module).append(BGC_method_local);

    // Load information fo the method
    // Native module
    if ((OpenWQ_wqconfig.BGC_module).compare("OPENWQ_NATIVE_BGC") == 0){
    
        // Get number of chemical species from BGC_json
        (OpenWQ_wqconfig.BGC_general_num_chem) = OpenWQ_json.BGC_module
            ["CHEMICAL_SPECIES"]["LIST"].size();

        // Get mobile species 
        // reset index to start on zero
        std::vector<unsigned int> mobile_species_local = 
            OpenWQ_json.BGC_module["CHEMICAL_SPECIES"]["BGC_GENERAL_MOBILE_SPECIES"];

        for (unsigned int chemi = 0; chemi < mobile_species_local.size(); chemi++){
        
            OpenWQ_wqconfig.BGC_general_mobile_species.push_back(mobile_species_local[chemi] - 1);

        }

        // Get chemical species list from BGC_json
        for (unsigned int chemi = 0; chemi < (OpenWQ_wqconfig.BGC_general_num_chem); chemi++)
        {
            (OpenWQ_wqconfig.BGC_general_chem_species_list).push_back(OpenWQ_json.BGC_module
                ["CHEMICAL_SPECIES"]["LIST"][std::to_string(chemi + 1)]);
        }
    }
    
    
    
    // ########################################
    // Set Output options
    // ########################################
    
    // Output folder
    OpenWQ_wqconfig.output_dir = OpenWQ_json.Master
        ["OPENWQ_OUTPUT"]
        ["RESULTS_FOLDERPATH"]; 

    // Output Units (tuple with all the information needed)
    std::get<0>(OpenWQ_wqconfig.output_units) = OpenWQ_json.Master
        ["OPENWQ_OUTPUT"]
        ["UNITS"];  // Units as provided  by user

    // Run model: check if DEBUG model has been requested
    // The model will print all the derivatives
    OpenWQ_wqconfig.debug_mode = OpenWQ_json.Master["COMPUTATIONAL_SETTINGS"]["RUN_MODE_DEBUG"];
    
     // Convert time units from host model units to seconds (OpenWQ time units)
    // 1) Calculate unit multiplers
    volume_unit_flag = OpenWQ_units.Calc_Unit_Multipliers(
                OpenWQ_wqconfig,
                OpenWQ_output,
                unit_multiplers,                            // multiplers (numerator and denominator)
                std::get<0>(OpenWQ_wqconfig.output_units),  // input units
                false);              // direction of the conversion: 
                                     // to native (true) or 
                                     // from native to desired output units (false)
    std::get<1>(OpenWQ_wqconfig.output_units) = unit_multiplers[0]; // multipler for numerator
    std::get<2>(OpenWQ_wqconfig.output_units) = unit_multiplers[1]; // multiupler for denominator
    std::get<3>(OpenWQ_wqconfig.output_units) = volume_unit_flag; // flag if denominator is volumeo

    // Output format
    output_format = OpenWQ_json.Master
        ["OPENWQ_OUTPUT"]["FORMAT"];

    // ########################################
    // Output format ######

    // Create OpenWQ_wqconfig.output_dir folder if nonexistant
    OpenWQ_readjson::check_mkdir_openWQ(
        OpenWQ_wqconfig.output_dir);
            
    OpenWQ_wqconfig.output_type = -1;   // reset

    // CSV format
    if (output_format.compare("CSV") == 0){

        OpenWQ_wqconfig.output_type = 0;

        // create dir if needed
        OpenWQ_wqconfig.output_dir.append("/CSV");
        OpenWQ_readjson::check_mkdir_openWQ(
            OpenWQ_wqconfig.output_dir);

    // VTK format
    }else if (output_format.compare("VTU") == 0){

        OpenWQ_wqconfig.output_type = 1;

        // create dir if needed
        OpenWQ_wqconfig.output_dir.append("/VTU");
        OpenWQ_readjson::check_mkdir_openWQ(
            OpenWQ_wqconfig.output_dir);

    // HDF5 format
    }else if (output_format.compare("HDF5") == 0){

        OpenWQ_wqconfig.output_type = 2;

        // create dir if needed
        OpenWQ_wqconfig.output_dir.append("/HDF5");
        OpenWQ_readjson::check_mkdir_openWQ(
            OpenWQ_wqconfig.output_dir); 

    } else {
        
        // Create Message (Error Message)
        msg_string = 
            "<OpenWQ> ERROR: Output type unkown: " 
            + output_format;

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(
            OpenWQ_wqconfig,    // for Log file name
            msg_string,         // message
            true,               // print in console
            true);              // print in log file
    }

    // ########################################
    // Time ######
    // Get print/output timestep AND convert to seconds
    time_tuple = OpenWQ_json.Master
        ["OPENWQ_OUTPUT"]["TIMESTEP"];  // Get tuple (value, unit)
    (OpenWQ_wqconfig.timetep_out) = std::get<0>(time_tuple);       // Get value
    (OpenWQ_wqconfig.timestep_out_unit) = std::get<1>(time_tuple); // Get units

    
    // Convert time units from host model units to seconds (OpenWQ time units)
    // 1) Calculate unit multiplers
    OpenWQ_units.Calc_Unit_Multipliers(
        OpenWQ_wqconfig,
        OpenWQ_output,
        unit_multiplers,                    // multiplers (numerator and denominator)
        OpenWQ_wqconfig.timestep_out_unit,  // input units
        true);              // direction of the conversion: 
                            // to native (true) or 
                            // from native to desired output units (false)
    // 2) Calculate value with new units
    OpenWQ_units.Convert_Units(
        OpenWQ_wqconfig.timetep_out,    // ic_value passed by reference so that it can be changed
        unit_multiplers);               // units

    // ########################################
    // Chemicals to print
    // num of chem to print
    num_chem2print = OpenWQ_json.Master
        ["OPENWQ_OUTPUT"]["CHEMICAL_SPECIES"].size();
    
    // Get indexes for the list of chemicals requested
    for (unsigned int chemi = 0; chemi < num_chem2print; chemi++){

        // Chemical name (to print)
        chem_name2print = OpenWQ_wqconfig.BGC_general_chem_species_list[chemi];
        
        for (unsigned int chemlisti = 0; chemlisti < (OpenWQ_wqconfig.BGC_general_num_chem); chemlisti++){
            
            chem_namelist = OpenWQ_wqconfig.BGC_general_chem_species_list[chemlisti];

            // Check if compartments listed match internal compartment names
            if (chem_namelist.compare(chem_name2print) == 0){                               
                
                OpenWQ_wqconfig.chem2print.push_back(chemlisti);
                break;

            }
        }
    }


    // ########################################
    // Compartments to print
    // num of comartments to print
    num_compt2print = OpenWQ_json.Master
        ["OPENWQ_OUTPUT"]["COMPARTMENTS_AND_CELLS"].size();
    
    // Get list of compartments to print from COMPARTMENTS_AND_CELLS
    for (auto& x1 : OpenWQ_json.Master
        ["OPENWQ_OUTPUT"]["COMPARTMENTS_AND_CELLS"].items()){
                
        try{
            compt_names_vec.push_back(x1.key());

        }catch (...){}
    }

    // Get indexes for the list of compartments requested
    for (unsigned int cmpti = 0; cmpti < num_compt2print; cmpti++){

        compt_name2print = compt_names_vec[cmpti];
        
        for (unsigned int icmp = 0; icmp < OpenWQ_hostModelconfig.num_HydroComp; icmp++){
                
            CompName_icmp = std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(icmp));  // name

            // Check if compartments listed match internal compartment names
            if (CompName_icmp.compare(compt_name2print) == 0){                               
                
                OpenWQ_wqconfig.compt2print.push_back(icmp);
                break;

            }
        }
    }

    // ########################################
    // Cells to print 
    // Check what cells to print for each viable (compt2print) compartment requested
    for (unsigned int icmp = 0; icmp < OpenWQ_hostModelconfig.num_HydroComp; icmp++){

        // Check if icmp is included in compt2print
        it = find (
            OpenWQ_wqconfig.compt2print.begin(), 
            OpenWQ_wqconfig.compt2print.end(),
            icmp);

        // if icmp is not in compt2print, then print dummy_cell and skip
        if (it == OpenWQ_wqconfig.compt2print.end()){
            // set cells2print_vec to be null because compartment has not been selected
            OpenWQ_wqconfig.cells2print_bool.push_back(false);
            OpenWQ_wqconfig.cells2print_vec.push_back(cells2print_cmpt); 
            cells2print_cmpt.clear();
            continue;
        }

        // compartnment icmp domain
        nx = std::get<2>(OpenWQ_hostModelconfig.HydroComp.at(icmp));
        ny = std::get<3>(OpenWQ_hostModelconfig.HydroComp.at(icmp));
        nz = std::get<4>(OpenWQ_hostModelconfig.HydroComp.at(icmp));
        
        // Check if requesting to print "ALL_CELLS"
        try{
            cells_input = OpenWQ_json.Master
                ["OPENWQ_OUTPUT"]["COMPARTMENTS_AND_CELLS"]
                [std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(icmp))];

            // if "ALL_CELLS", then put true in cells2print_bool for compartment icmp
            if(cells_input.compare("ALL_CELLS") == 0){

                // set print all true
                OpenWQ_wqconfig.cells2print_bool.push_back(true);

                // Construct vector cells2print_vec with all x,y,z combinations
                for (unsigned int iz=0;iz<nz;iz++){   
                    for (unsigned int iy=0;iy<ny;iy++){
                        for (unsigned int ix=0;ix<nx;ix++){
                        
                            // add cell requested to list to print for each compartment
                            // first create the vector cells2print_row with x, y and z values
                            cells2print_row(0,0) = ix;
                            cells2print_row(0,1) = iy;
                            cells2print_row(0,2) = iz;

                            // add that cells2print_row to cells2print_cmpt
                            cells2print_cmpt.insert_rows(
                                cells2print_cmpt.n_rows,
                                cells2print_row);

                            // set to zero cells2print_row for re-use
                            cells2print_row.zeros();

                        }
                    }
                }
            }else{
                
                // if entry is no "ALL_CELLS", 
                // then exclude from printing all (set print all false)
                OpenWQ_wqconfig.cells2print_bool.push_back(false); 


                // Create Message (Warning Message)
                msg_string = 
                    "<OpenWQ> WARNING: Unkown entry (" 
                    + cells_input
                    + ") for OPENWQ_OUTPUT > COMPARTMENTS_AND_CELLS > " 
                    + std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(icmp));

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(
                    OpenWQ_wqconfig,    // for Log file name
                    msg_string,         // message
                    true,               // print in console
                    true);              // print in log file
                    }


        // If not "ALL_CELLS", then look for vectors with cell locations requested"
        }catch(...){

            // Set no print all because specific cell for printing are provided
            OpenWQ_wqconfig.cells2print_bool.push_back(false);

            // Get number of cell entries provided
            num_cells2print = OpenWQ_json.Master
                ["OPENWQ_OUTPUT"]["COMPARTMENTS_AND_CELLS"]
                [std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(icmp))].size();

            for (unsigned int celli = 0; celli < num_cells2print; celli++){
                
                // Get ix value
                ix_json = OpenWQ_json.Master
                    ["OPENWQ_OUTPUT"]["COMPARTMENTS_AND_CELLS"]
                    [std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(icmp))]
                    [std::to_string(celli + 1)]
                    .at(0);
                ix_json --; // remove 1 to match c++ convention to start in zero

                // Get iy value
                iy_json = OpenWQ_json.Master
                    ["OPENWQ_OUTPUT"]["COMPARTMENTS_AND_CELLS"]
                    [std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(icmp))]
                    [std::to_string(celli + 1)]
                    .at(1);
                iy_json--;  // remove 1 to match c++ convention to start in zero

                // Get iz value
                iz_json = OpenWQ_json.Master
                    ["OPENWQ_OUTPUT"]["COMPARTMENTS_AND_CELLS"]
                    [std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(icmp))]
                    [std::to_string(celli + 1)]
                    .at(2);
                iz_json --; // remove 1 to match c++ convention to start in zero

                // Check if cell requested is witin the boundaries of the spatial domain
                // If yes, add the cell selected to cells2print_vec
                // Otherwise, write warning message (skip entry)
                if (ix_json <= nx
                    && iy_json <= ny
                    && iz_json <= nz){

                     // add cell requested to list to print for each compartment
                    // first create the vector cells2print_row with x, y and z values
                    cells2print_row(0,0) = ix_json;
                    cells2print_row(0,1) = iy_json;
                    cells2print_row(0,2) = iz_json;

                    // add that cells2print_row to cells2print_cmpt
                    cells2print_cmpt.insert_rows(
                        cells2print_cmpt.n_rows,
                        cells2print_row);

                    // clear cells2print_row for re-use
                    cells2print_row.zeros();

                }else{
                    
                      // Create Message (Error - locate problematic cell)
                    msg_string = 
                        "<OpenWQ> ERROR: Cell entry provided out of domain"
                        " in OPENWQ_OUTPUT > COMPARTMENTS_AND_CELLS > "
                        + std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(icmp))
                        + "> '" + std::to_string(celli + 1) + "'";

                    // Print it (Console and/or Log file)
                    OpenWQ_output.ConsoleLog(
                        OpenWQ_wqconfig,    // for Log file name
                        msg_string,         // message
                        true,               // print in console
                        true);              // print in log file

                }
                  
            }

        }

        // Add the complete list of cells to print
        OpenWQ_wqconfig.cells2print_vec.push_back(cells2print_cmpt);
        // Clear cells2print_cmpt for reuse
        cells2print_cmpt.clear();

    }

    
}

// Check if directory exists and create it
void OpenWQ_readjson::check_mkdir_openWQ(
    std::string &dirname){
    
    struct stat st = {0};
    
    // convert to *char
    const char *cstr = dirname.c_str();

    // mkdir
    if (stat(cstr, &st) == -1) {
        mkdir(cstr, 0700);
    }

}