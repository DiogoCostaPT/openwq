
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
    OpenWQ_utils & OpenWQ_utils,
    OpenWQ_output& OpenWQ_output)
{

    // Local
    std::string input_filepath;
    json jsonMaster_SubStruct;
    std::string errorMsgIdentifier;

    /* ################################################
    // Read JSON files
    // Master, Config, ExtWatFlux, and SinkSource
    ################################################ */

    // ########################
    // Master file (read)
    OpenWQ_readjson::read_JSON_2class(
        OpenWQ_wqconfig,
        OpenWQ_output,
        OpenWQ_utils,
        OpenWQ_json.Master,                 // JSON structure to save to
        false,                              // Save in subfield of JSON structure? only if multiple files (e.g., source and sinks)
        "",                                 // if above true, provide name of subfield
        OpenWQ_wqconfig.get_OpenWQ_masterjson()); // Name of JSON file

    // Get json substructure
    errorMsgIdentifier = "Master file";
    jsonMaster_SubStruct = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master, "OPENWQ_INPUT",
        errorMsgIdentifier,
        true);

    // ########################
    // Main confirguration json (read)
    errorMsgIdentifier = "Master file inside OPENWQ_INPUT";
    input_filepath = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        jsonMaster_SubStruct, "CONFIG_FILEPATH",
        errorMsgIdentifier,
        true);

    OpenWQ_readjson::read_JSON_2class(
        OpenWQ_wqconfig,
        OpenWQ_output,
        OpenWQ_utils,
        OpenWQ_json.Config,
        false,
        "",
        input_filepath);

    // ########################
    // External fluxes json (read)
    // if not found, throw a warning message and continue
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        jsonMaster_SubStruct, "EXTERNAL_WATER_FLUXES",
        errorMsgIdentifier,
        false);
    unsigned int num_eff = jsonMaster_SubStruct["EXTERNAL_WATER_FLUXES"].size();

    errorMsgIdentifier = "Master file inside OPENWQ_INPUT > EXTERNAL_WATER_FLUXES";
    for (unsigned int eff = 0; eff < num_eff; eff++)
    {   
        
        // Check if row eff field exists
        OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            jsonMaster_SubStruct["EXTERNAL_WATER_FLUXES"], std::to_string(eff + 1),
            errorMsgIdentifier,
            true);

        // Check if row eff field exists
       input_filepath = OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            jsonMaster_SubStruct["EXTERNAL_WATER_FLUXES"][std::to_string(eff + 1)],"FILEPATH",
            errorMsgIdentifier,
            true);

        OpenWQ_readjson::read_JSON_2class(
            OpenWQ_wqconfig,
            OpenWQ_output,
            OpenWQ_utils,
            OpenWQ_json.ExtWatFlux,
            true,
            std::to_string(eff + 1),
            input_filepath);
    }

    // ########################
    // SinkSource json (read)
    // if not found, throw a warning message and continue
    errorMsgIdentifier = "Master file inside OPENWQ_INPUT";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        jsonMaster_SubStruct, "SINK_SOURCE",
        errorMsgIdentifier,
        false);
    unsigned int num_ssf = jsonMaster_SubStruct["SINK_SOURCE"].size();

    errorMsgIdentifier = "Master file inside OPENWQ_INPUT > SINK_SOURCE";
    for (unsigned int ssf = 0; ssf < num_ssf; ssf++)
    {
        // Check if row eff field exists
        OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            jsonMaster_SubStruct["SINK_SOURCE"], std::to_string(ssf + 1),
            errorMsgIdentifier,
            true);

        // Check if row eff field exists
       input_filepath = OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            jsonMaster_SubStruct["SINK_SOURCE"][std::to_string(ssf + 1)],"FILEPATH",
            errorMsgIdentifier,
            true);

        OpenWQ_readjson::read_JSON_2class(
            OpenWQ_wqconfig,
            OpenWQ_output,
            OpenWQ_utils,
            OpenWQ_json.SinkSource,
            true,
            std::to_string(ssf + 1),
            input_filepath);
    }

    /* ########################
    // Read JSON files
    // MODULES
    ########################### */

    errorMsgIdentifier = "Master file inside OPENWQ_INPUT";
    // check if field MODULES exist
    jsonMaster_SubStruct = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master, "MODULES",
        errorMsgIdentifier,
        true);

    // ########################
    // BGC_module cycling json (read)

    // check if field BIOGEOCHEMISTRY exist
    errorMsgIdentifier = "Master file inside OPENWQ_INPUT > MODULES";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        jsonMaster_SubStruct, "BIOGEOCHEMISTRY",
        errorMsgIdentifier,
        true);

    errorMsgIdentifier = "Master file inside OPENWQ_INPUT > MODULES > BIOGEOCHEMISTRY";
    input_filepath = OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            jsonMaster_SubStruct["BIOGEOCHEMISTRY"],"MODULE_CONFIG_FILEPATH",
            errorMsgIdentifier,
            true);

    OpenWQ_readjson::read_JSON_2class(
        OpenWQ_wqconfig,
        OpenWQ_output,
        OpenWQ_utils,
        OpenWQ_json.BGC_module,
        false,
        "",
        input_filepath);


    // ########################
    // TE_module cycling json (read)

    // check if field TRANSPORT_EROSION exist
    errorMsgIdentifier = "Master file inside OPENWQ_INPUT > MODULES";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        jsonMaster_SubStruct, "TRANSPORT_EROSION",
        errorMsgIdentifier,
        true);

    errorMsgIdentifier = "Master file inside OPENWQ_INPUT > MODULES > TRANSPORT_EROSION";
    input_filepath = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        jsonMaster_SubStruct["TRANSPORT_EROSION"],"MODULE_CONFIG_FILEPATH",
        errorMsgIdentifier,
        true);

    OpenWQ_readjson::read_JSON_2class(
        OpenWQ_wqconfig,
        OpenWQ_output,
        OpenWQ_utils,
        OpenWQ_json.TE_module,
        false,
        "",
        input_filepath);

    
    /* ################################################
    // Data Preparation
    // Process some of the openWQ config info
    ################################################ */
    OpenWQ_readjson::SetConfigInfo_driver(
        OpenWQ_json,
        OpenWQ_hostModelconfig,
        OpenWQ_wqconfig,
        OpenWQ_utils,
        OpenWQ_units,
        OpenWQ_output);

}

/* #################################################
// Read JSON file to class
################################################# */
void OpenWQ_readjson::read_JSON_2class(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_output& OpenWQ_output,
    OpenWQ_utils& OpenWQ_utils,
    json& jsondata,                         // JSON structure to save to
    const bool substruc_flag,               // Save in subfield of JSON structure? only if multiple files (e.g., source and sinks)
    const std::string JsonSubStruct_name,   // if true, name of subfield
    const std::string jsonfile)             // Name of JSON file
{ 

    // Local Variables
    std::string msg_string;             // error/warning message string
    bool readfail = false;

    /* ####################################
    // Save JSON file in JSON data structure
    #################################### */

    try
    {
        // Read json file with ifstrem
        std::ifstream f(jsonfile);

        // Raise exit flag if cannot find file
        if(f.fail()){

            // Error file
            readfail = true;

            // Create Message
            msg_string = 
                "<OpenWQ> ERROR: Could not find file: " 
                + jsonfile;
                
        }

        //* ##########################
        // If substruc_flag = true
        // save JSON inside JsonSubStruct_name (which is actually of int type) field in jsondata
        //* ####################################
        if (substruc_flag == false)
        {
            // Save ifstream to JSON data type
            jsondata = json::parse(     f,
                /* callback */          nullptr,
                /* allow exceptions */  false,
                /* skip_comments */     true
                );

            // Convert all text in JSON to upper case
            OpenWQ_readjson::ConvertJSONtext_2upperCase(
                OpenWQ_utils,
                jsondata);
        }
        else
        {
            // Save ifstream to JSON data type
            jsondata[JsonSubStruct_name] = json::parse(f,
                /* callback */                          nullptr,
                /* allow exceptions */                  false,
                /* skip_comments */                     true
                );
                    
            // Convert all text in JSON to upper case
            OpenWQ_readjson::ConvertJSONtext_2upperCase(
                OpenWQ_utils,
                jsondata[JsonSubStruct_name]);
        }
    }
    /* ####################################
    // Handle exceptions 
    #################################### */
    catch (const std::exception &e)
    {
        // Error flag
        readfail = true;

        // Create Message
        msg_string = 
            "<OpenWQ> ERROR: An exception occurred parsing JSON file: " 
            + jsonfile;

    }

    // Exist if problem with file
    if(readfail){

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
    OpenWQ_utils &OpenWQ_utils,
    json &jsondata){

    // Local Variables
    std::string old_jsonkey_layer_1;
    std::string old_jsonkey_layer_2;
    std::string old_jsonkey_layer_3;
    std::string old_jsonkey_layer_4;
    std::string old_jsonkey_layer_5;
    std::string old_jsonkey_layer_6;
    std::string old_jsonkey_layer_7;

    std::string new_jsonkey_layer_1;
    std::string new_jsonkey_layer_2;
    std::string new_jsonkey_layer_3;
    std::string new_jsonkey_layer_4;
    std::string new_jsonkey_layer_5;
    std::string new_jsonkey_layer_6;
    std::string new_jsonkey_layer_7;
    
    // ########################################
    // Identify all keys in JSON tree data and replace ny upper case keys

    // ------------> First layer of keys
    for (auto& x1 : jsondata.items()){

        try{
                
            old_jsonkey_layer_1 = x1.key();
            
            // Save key to o if a key
            if (!old_jsonkey_layer_1.empty()){ // if not null
                OpenWQ_readjson::change_JSON_key_to_upper_case(
                    OpenWQ_utils,
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
                            OpenWQ_utils,
                            jsondata[new_jsonkey_layer_1], 
                            old_jsonkey_layer_2,
                            new_jsonkey_layer_2);
                    }else{
                        OpenWQ_readjson::change_JSON_value_to_upper_case(
                            OpenWQ_utils,
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
                                    OpenWQ_utils,
                                    jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2], 
                                    old_jsonkey_layer_3,
                                    new_jsonkey_layer_3);
                            }else{
                                OpenWQ_readjson::change_JSON_value_to_upper_case(
                                    OpenWQ_utils,
                                    jsondata[new_jsonkey_layer_1],
                                    new_jsonkey_layer_2);
                                continue;
                            }

                            // -------------> Fourth possible layer of keys
                            for (auto& x4 : jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3].items()){
                                    
                                try{
                                
                                    old_jsonkey_layer_4 = x4.key();

                                    if (!old_jsonkey_layer_4.empty() && // if not null
                                        jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3].type() != json::value_t::array){ // and not array
                                        
                                        OpenWQ_readjson::change_JSON_key_to_upper_case(
                                            OpenWQ_utils,
                                            jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3], 
                                            old_jsonkey_layer_4,
                                            new_jsonkey_layer_4);
                                    }else{
                                        OpenWQ_readjson::change_JSON_value_to_upper_case(
                                        OpenWQ_utils,
                                        jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2],
                                        new_jsonkey_layer_3);
                                    continue;
                                    }   

                                    // -------------> Fifth possible layer of keys
                                    for (auto& x5 : jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4].items()){
                            
                                        try{
                                        
                                            old_jsonkey_layer_5 = x5.key();

                                            if (!old_jsonkey_layer_5.empty() && // if not null
                                                jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4].type() != json::value_t::array){ // and not array
                                                
                                                OpenWQ_readjson::change_JSON_key_to_upper_case(
                                                    OpenWQ_utils,
                                                    jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4], 
                                                    old_jsonkey_layer_5,
                                                    new_jsonkey_layer_5);
                                            }else{
                                                OpenWQ_readjson::change_JSON_value_to_upper_case(
                                                OpenWQ_utils,
                                                jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3],
                                                new_jsonkey_layer_4);
                                                continue;
                                            }

                                            // -------------> Sixth possible layer of keys
                                            for (auto& x6 : jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4][new_jsonkey_layer_5].items()){
                                
                                                try{
                                                
                                                    old_jsonkey_layer_6 = x6.key();

                                                    if (!old_jsonkey_layer_6.empty() && // if not null
                                                        jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4][new_jsonkey_layer_5].type() != json::value_t::array){ // and not array
                                                        
                                                        OpenWQ_readjson::change_JSON_key_to_upper_case(
                                                            OpenWQ_utils,
                                                            jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4][new_jsonkey_layer_5], 
                                                            old_jsonkey_layer_6,
                                                            new_jsonkey_layer_6);
                                                    }else{
                                                        OpenWQ_readjson::change_JSON_value_to_upper_case(
                                                        OpenWQ_utils,
                                                        jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4],
                                                        new_jsonkey_layer_5);
                                                        continue;
                                                    }

                                                    // -------------> Seven possible layer of keys
                                                    for (auto& x7 : jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4][new_jsonkey_layer_5][new_jsonkey_layer_6].items()){
                                        
                                                        try{
                                                        
                                                            old_jsonkey_layer_7 = x7.key();

                                                            if (!old_jsonkey_layer_7.empty() && // if not null
                                                                jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4][new_jsonkey_layer_5][new_jsonkey_layer_6].type() != json::value_t::array){ // and not array
                                                                
                                                                OpenWQ_readjson::change_JSON_key_to_upper_case(
                                                                    OpenWQ_utils,
                                                                    jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4][new_jsonkey_layer_5][new_jsonkey_layer_6], 
                                                                    old_jsonkey_layer_7,
                                                                    new_jsonkey_layer_7);
                                                            }else{
                                                                OpenWQ_readjson::change_JSON_value_to_upper_case(
                                                                OpenWQ_utils,
                                                                jsondata[new_jsonkey_layer_1][new_jsonkey_layer_2][new_jsonkey_layer_3][new_jsonkey_layer_4][new_jsonkey_layer_5],
                                                                new_jsonkey_layer_6);
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
                            
                }catch (...){}
            }

        }catch (...){}

    }
    

}

// ########################################
// Change JSON key
// ########################################
void OpenWQ_readjson::change_JSON_key_to_upper_case(
    OpenWQ_utils &OpenWQ_utils,
    json &object, 
    const std::string& old_key,
    std::string& new_key)
{

    try{

        // Convert to upper case
        new_key = OpenWQ_utils.ConvertStringToUpperCase(old_key);

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
    OpenWQ_utils &OpenWQ_utils,
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

        // Convert to upper case
        new_value = OpenWQ_utils.ConvertStringToUpperCase(old_value);    

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
                // Convert to upper case
                new_value_str = OpenWQ_utils.ConvertStringToUpperCase(old_value);

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
// Extract model configuration info from JSON files
// ########################################
void OpenWQ_readjson::SetConfigInfo_driver(
    OpenWQ_json &OpenWQ_json,
    OpenWQ_hostModelconfig & OpenWQ_hostModelconfig,
    OpenWQ_wqconfig &OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units &OpenWQ_units,
    OpenWQ_output& OpenWQ_output){
    
    // Set logFile
    // First thing because the log file needs to be initiated to 
    // log info about config
    SetConfigInfo_output_logFile(
        OpenWQ_json, OpenWQ_wqconfig, OpenWQ_utils, OpenWQ_output);

    // Set metadata
    SetConfigInfo_meta(
        OpenWQ_json, OpenWQ_wqconfig,OpenWQ_output);

    // Set computation settings
    SetConfigInfo_compute(
        OpenWQ_json, OpenWQ_wqconfig, OpenWQ_utils, OpenWQ_output);

    // Set chem module settings
    SetConfigInfo_chemModule(
        OpenWQ_json, OpenWQ_wqconfig, OpenWQ_utils, OpenWQ_output);

    // Set TE module settings
    SetConfigInfo_TEModule(
        OpenWQ_json, OpenWQ_hostModelconfig, OpenWQ_wqconfig, 
        OpenWQ_utils, OpenWQ_output);

    // Set output options
    SetConfigInfo_output_driver(
        OpenWQ_json, OpenWQ_hostModelconfig, OpenWQ_wqconfig, 
        OpenWQ_utils, OpenWQ_units, OpenWQ_output);

}

// Set metadata
void OpenWQ_readjson::SetConfigInfo_meta(
    OpenWQ_json &OpenWQ_json,
    OpenWQ_wqconfig &OpenWQ_wqconfig,
    OpenWQ_output& OpenWQ_output){
    
    // Local variables
    std::string msg_string;

    // PROJECT_NAME
    msg_string.clear();
    msg_string = "PROJECT_NAME: ";
    try{ msg_string.append(OpenWQ_json.Master["PROJECT_NAME"]);
    }catch(...){ msg_string.append("unkown (not provided in Master file)");};
    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string,false, true);

    // GEOGRAPHICAL_LOCATION
    msg_string.clear();
    msg_string = "GEOGRAPHICAL_LOCATION: ";
    try{ msg_string.append(OpenWQ_json.Master["GEOGRAPHICAL_LOCATION"]);
    }catch(...){ msg_string.append("unkown (not provided in Master file)");};
    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string,false, true);
    
    // AUTHORS
    msg_string.clear();
    msg_string = "AUTHORS: ";
    try{ msg_string.append(OpenWQ_json.Master["AUTHORS"]);
    }catch(...){ msg_string.append("unkown (not provided in Master file)");};
    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string,false, true);

    // DATE
    msg_string.clear();
    msg_string = "DATE: ";
    try{ msg_string.append(OpenWQ_json.Master["DATE"]);
    }catch(...){ msg_string.append("unkown (not provided in Master file)");};
    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string,false, true);

    // Separator
    msg_string.clear();
    msg_string = "\n ##############################\n ";
    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string,false, true);

}

// Set computation options
void OpenWQ_readjson::SetConfigInfo_compute(
    OpenWQ_json &OpenWQ_json,
    OpenWQ_wqconfig &OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_output& OpenWQ_output){
    
    // Local variables
    std::string num_cores_input;                            // input of threads info as string
    bool threads_input_err_flag = false;                    // flag for error in threads input (initiate a false, no error)
    std::string msg_string;             // error/warning message string
    std::string errorMsgIdentifier;
    json json_computeSettings;

    // Get json-subStructurd related to computational settings
    errorMsgIdentifier = "Master file";
    json_computeSettings = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master, "COMPUTATIONAL_SETTINGS",
        errorMsgIdentifier,
        true);

    // Run model: check if DEBUG model has been requested
    // The model will print all the derivatives
    errorMsgIdentifier = "Master file in COMPUTATIONAL_SETTINGS";
    OpenWQ_wqconfig.debug_mode = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        json_computeSettings, "RUN_MODE_DEBUG",
        errorMsgIdentifier,
        true);

    // Get number of threads in the system
    OpenWQ_wqconfig.num_threads_system = sysconf(_SC_NPROCESSORS_ONLN);
    
    // Get number of threads requested
    // It can be provided as an int or "all" (string)
    errorMsgIdentifier = "Master file in COMPUTATIONAL_SETTINGS";
    try{
        // Test if provided as integer with number of cores
        // Convert cast to int (if double or flow)
        OpenWQ_wqconfig.num_threads_requested = (int) json_computeSettings["USE_NUM_THREADS"];

    }catch(...){
        
        // Check if equal to "ALL"
        try{
            // If not integer, check if string
            num_cores_input = json_computeSettings["USE_NUM_THREADS"];
        
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
                 "<OpenWQ> WARNNING: Unrecognized input for COMPUTATIONAL_SETTINGS > USE_NUM_THREADS: '"
                + num_cores_input
                + "' (Only allowed \"all\" or integer). USE_NUM_THREADS has been defaulted to "
                + std::to_string(OpenWQ_wqconfig.num_threads_default) + ".";
            OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, false, true);

            // Default number of threads
            OpenWQ_wqconfig.num_threads_requested = OpenWQ_wqconfig.num_threads_default;

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
            + "). All system threads available have been engaged.";
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string,false, true);
        
        // Overwrite OpenWQ_wqconfig.num_threads_requested to max number in the system
        OpenWQ_wqconfig.num_threads_requested = OpenWQ_wqconfig.num_threads_system;
        
    }else{

        // Create Message
        msg_string = 
            "<OpenWQ> INFO: Number of threads requested and used: " 
            + std::to_string(OpenWQ_wqconfig.num_threads_requested) + ".";
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string,false, true);

    }

}


// Set chem info
void OpenWQ_readjson::SetConfigInfo_chemModule(
    OpenWQ_json &OpenWQ_json,
    OpenWQ_wqconfig &OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_output& OpenWQ_output){
    
    // Local variables
    std::string BGC_method_local;
    std::string errorMsgIdentifier;
    json BGQjson_subStruct;
    json BGQjson_ChemList;
    json BGQjson_mobileSpecies;

    // Check is MODULE key exists
    errorMsgIdentifier = "Master file";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master, "MODULES",
        errorMsgIdentifier,
        true);

    // Check if BIOGEOCHEMISTRY key exists
    errorMsgIdentifier = "Master file > MODULE";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master["MODULES"], "BIOGEOCHEMISTRY",
        errorMsgIdentifier,
        true);

    // Chem method
    errorMsgIdentifier = "Master file > MODULE > BIOGEOCHEMISTRY";
    BGC_method_local = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master["MODULES"]["BIOGEOCHEMISTRY"], "MODULE_NAME",
        errorMsgIdentifier,
        true);    
    
    (OpenWQ_wqconfig.BGC_module).append(BGC_method_local);

    // Load information fo the method
    // Native module
    if ((OpenWQ_wqconfig.BGC_module).compare("OPENWQ_NATIVE_BGC") == 0){
        
        // Check if BGQ json has CHEMICAL_SPECIES key
        errorMsgIdentifier = "BGQ file";
        BGQjson_subStruct = OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            OpenWQ_json.BGC_module, "CHEMICAL_SPECIES",
            errorMsgIdentifier,
            true); 

        // Check if BGQ json has CHEMICAL_SPECIES key
        errorMsgIdentifier = "BGQ file in CHEMICAL_SPECIES";
        BGQjson_ChemList = OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            BGQjson_subStruct, "LIST",
            errorMsgIdentifier,
            true); 

        // Get number of chemical species from BGC_json
        (OpenWQ_wqconfig.BGC_general_num_chem) = BGQjson_ChemList.size();

        // Get mobile species 
        // reset index to start on zero
        errorMsgIdentifier = "BGQ file in CHEMICAL_SPECIES";
        BGQjson_mobileSpecies = OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            BGQjson_subStruct, "BGC_GENERAL_MOBILE_SPECIES",
            errorMsgIdentifier,
            false);  // no abort

        for (unsigned int chemi = 0; chemi < BGQjson_mobileSpecies.size(); chemi++){
        
            OpenWQ_wqconfig.BGC_general_mobile_species.push_back(
                (int)BGQjson_mobileSpecies.at(chemi) - 1);

        }

        // Get chemical species list from BGC_json
        for (unsigned int chemi = 0; chemi < (OpenWQ_wqconfig.BGC_general_num_chem); chemi++)
        {
            (OpenWQ_wqconfig.BGC_general_chem_species_list).push_back(
                BGQjson_ChemList[std::to_string(chemi + 1)]);
        }
    }

}

// Set module options
void OpenWQ_readjson::SetConfigInfo_TEModule(
    OpenWQ_json &OpenWQ_json,
    OpenWQ_hostModelconfig & OpenWQ_hostModelconfig,
    OpenWQ_wqconfig &OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_output& OpenWQ_output){
    
    // Local variables
    std::string errorMsgIdentifier;
    std::string msg_string;
    std::string TE_method_local;    // module name
    unsigned int num_entries;
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
    json json_K_Erodib_K;
    json json_IntMov_subStruct;
    json json_BoundMix_subStruct;
    json json_BoundMix_subStruct_subStruct;

    // check if field MODULES exist
    errorMsgIdentifier = "Master file";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master, "MODULES",
        errorMsgIdentifier,
        true);

    // check if field MODULES > TRANSPORT_EROSION exist
    errorMsgIdentifier = "Master file in MODULES";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master["MODULES"], "TRANSPORT_EROSION",
        errorMsgIdentifier,
        true);

    // Get TRANSPORT_EROSION module name
    errorMsgIdentifier = "Master file in MODULES > TRANSPORT_EROSION";
    TE_method_local = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master["MODULES"]["TRANSPORT_EROSION"], "MODULE_NAME",
        errorMsgIdentifier,
        true);
    (OpenWQ_wqconfig.TE_module).append(TE_method_local);

    // Load information fo the method
    if ((OpenWQ_wqconfig.TE_module).compare("OPENWQ_NATIVE_TE_ADVDISP") == 0 
        || (OpenWQ_wqconfig.TE_module).compare("OPENWQ_NATIVE_TE_ADVP") == 0 
        || (OpenWQ_wqconfig.TE_module).compare("OPENWQ_NATIVE_TE_NO_ADVDISP") == 0){
        
        // Get Erodibility coeficients for native IntMob function
        errorMsgIdentifier = "TE file";
        json_IntMov_subStruct = OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            OpenWQ_json.TE_module, "INTMOB_CONFIGURATION",
            errorMsgIdentifier,
            true);

        errorMsgIdentifier = "Master file in MODULES > INTMOB_CONFIGURATION";
        json_K_Erodib_K = OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            json_IntMov_subStruct, "K_VAL",
            errorMsgIdentifier,
            true);

        for (unsigned int cmpi=0;cmpi<OpenWQ_hostModelconfig.get_num_HydroComp();cmpi++){
            try{
                OpenWQ_wqconfig.OpenWQ_TE_native_IntMob_Erodib_K.push_back( 
                    (double)json_K_Erodib_K.at(cmpi));
            }catch(...){
                msg_string = 
                    "<OpenWQ> ERROR: Problem with TE json > BOUNDMIX module > INTMOB_CONFIGURATION "
                    " > K_val vector. It must have size "
                    + std::to_string(OpenWQ_hostModelconfig.get_num_HydroComp())
                    + " (number of compartments in host_model) and contain only"
                    " double or integer entries.";
                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, true,true); 
                // Abort program
                exit(EXIT_FAILURE);
            }
        }
    
        // Get info for BoundMix function
        // Get number of entries
        errorMsgIdentifier = "TE file";
        json_BoundMix_subStruct = OpenWQ_utils.RequestJsonKeyVal_json(
            OpenWQ_wqconfig, OpenWQ_output,
            OpenWQ_json.TE_module, "BOUNDMIX_CONFIGURATION",
            errorMsgIdentifier,
            true);

        num_entries = json_BoundMix_subStruct.size();

        for (unsigned int entry_i = 0; entry_i < num_entries; entry_i++){

            // Check if sub-structure exists
            errorMsgIdentifier = "TE file";
            json_BoundMix_subStruct_subStruct = OpenWQ_utils.RequestJsonKeyVal_json(
                OpenWQ_wqconfig, OpenWQ_output,
                json_BoundMix_subStruct, std::to_string(entry_i + 1),
                errorMsgIdentifier,
                true);

            // Get entries
            errorMsgIdentifier = "TE file > " + std::to_string(entry_i + 1);
            input_direction = OpenWQ_utils.RequestJsonKeyVal_str(
                OpenWQ_wqconfig, OpenWQ_output,
                json_BoundMix_subStruct_subStruct, "DIRECTION",
                errorMsgIdentifier,
                true); 

            input_upper_compartment = OpenWQ_utils.RequestJsonKeyVal_str(
                OpenWQ_wqconfig, OpenWQ_output,
                json_BoundMix_subStruct_subStruct, "UPPER_COMPARTMENT",
                errorMsgIdentifier,
                true); 

            input_lower_compartment = OpenWQ_utils.RequestJsonKeyVal_str(
                OpenWQ_wqconfig, OpenWQ_output,
                json_BoundMix_subStruct_subStruct, "LOWER_COMPARTMENT",
                errorMsgIdentifier,
                true); 

            input_k_val = OpenWQ_utils.RequestJsonKeyVal_double(
                OpenWQ_wqconfig, OpenWQ_output,
                json_BoundMix_subStruct_subStruct, "K_VAL",
                errorMsgIdentifier,
                true); 

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
            for (unsigned int icmp_i = 0; icmp_i < OpenWQ_hostModelconfig.get_num_HydroComp(); icmp_i++)
            {
                // Upper compartment: index
                if (input_upper_compartment.compare(OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp_i)) == 0){
                    input_upper_compartment_index = icmp_i;
                    input_upper_compartment_index_exist = true;
                }
                // Lower compartment: index
                if (input_lower_compartment.compare(OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp_i)) == 0){
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
                OpenWQ_output.ConsoleLog(OpenWQ_wqconfig,msg_string,true,true);              // print in log file

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
    
}

// Set output options
void OpenWQ_readjson::SetConfigInfo_output_driver(
    OpenWQ_json &OpenWQ_json,
    OpenWQ_hostModelconfig & OpenWQ_hostModelconfig,
    OpenWQ_wqconfig &OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units &OpenWQ_units,
    OpenWQ_output& OpenWQ_output){

    // Set options and unit conversions
    SetConfigInfo_output_options(
        OpenWQ_json, OpenWQ_wqconfig, OpenWQ_utils, OpenWQ_units,
        OpenWQ_output);

    // Get compartments and chemicals to print
    SetConfigInfo_output_what2print(OpenWQ_json,
        OpenWQ_hostModelconfig, OpenWQ_wqconfig, OpenWQ_utils,
        OpenWQ_units,  OpenWQ_output);
    
}

// Set up logFile
void OpenWQ_readjson::SetConfigInfo_output_logFile(
    OpenWQ_json &OpenWQ_json,
    OpenWQ_wqconfig &OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_output& OpenWQ_output){

    // Local variables
    std::string errorMsgIdentifier;
    std::string output_format;
    std::string output_folder;
    std::string msg_string;

    // Check if OPENWQ_OUTPUT field exists
    errorMsgIdentifier = "Master file";
    OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master, "OPENWQ_OUTPUT",
        errorMsgIdentifier,
        true);

    errorMsgIdentifier = "Master file > OPENWQ_OUTPUT";
    output_format = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master["OPENWQ_OUTPUT"],"FORMAT",
        errorMsgIdentifier,
        true);

    output_folder = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master["OPENWQ_OUTPUT"],"RESULTS_FOLDERPATH",
        errorMsgIdentifier,
        true);

    // Create Log file name with full or relative path
    OpenWQ_wqconfig.LogFile_name_fullpath =  OpenWQ_wqconfig.LogFile_name;
    OpenWQ_wqconfig.LogFile_name_fullpath.insert(0,"/");
    OpenWQ_wqconfig.LogFile_name_fullpath.insert(0, output_format);
    OpenWQ_wqconfig.LogFile_name_fullpath.insert(0,"/");
    OpenWQ_wqconfig.LogFile_name_fullpath.insert(0, output_folder);

    // Create log file
    std::ofstream outfile (OpenWQ_wqconfig.LogFile_name_fullpath);

    // ###############
    // Write header in log file

    // Separator
    msg_string.clear();
    msg_string = "\n ############################## \n";
    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, false, true); 

}

// Set output options
void OpenWQ_readjson::SetConfigInfo_output_options(
    OpenWQ_json &OpenWQ_json,
    OpenWQ_wqconfig &OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units &OpenWQ_units,
    OpenWQ_output& OpenWQ_output){
    
    // Local variables
    std::string output_format;                              // format of output (CSV, VTK)
    std::string output_folder;
    json json_time;
    std::vector<std::string> units;                         // units (numerator and denominator)
    std::vector<double> unit_multiplers;                    // multiplers (numerator and denominator)
    bool volume_unit_flag;                                  // flag to note if denominator is a volume (needed for calculation of concentration in output)
    std::string num_cores_input;                            // input of threads info as string
    std::string msg_string;                                 // error/warning message string
    std::string errorMsgIdentifier;
    json json_output_subStruct; 

    // Output sub-structure
    errorMsgIdentifier = "Master file";
    json_output_subStruct = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master,"OPENWQ_OUTPUT",
        errorMsgIdentifier,
        true);

    // Output folder
    errorMsgIdentifier = "Master file > OPENWQ_OUTPUT";
    OpenWQ_wqconfig.output_dir = OpenWQ_utils.RequestJsonKeyVal_str(
        OpenWQ_wqconfig, OpenWQ_output,
        json_output_subStruct,"RESULTS_FOLDERPATH",
        errorMsgIdentifier,
        true);

    // Output Units (tuple with all the information needed)
    std::get<0>(OpenWQ_wqconfig.output_units) = 
        OpenWQ_utils.RequestJsonKeyVal_str(
            OpenWQ_wqconfig, OpenWQ_output,
            json_output_subStruct,"UNITS",
            errorMsgIdentifier,
            true);

    // Flag for no water
    OpenWQ_wqconfig.noWaterConc = OpenWQ_utils.RequestJsonKeyVal_int(
            OpenWQ_wqconfig, OpenWQ_output,
            json_output_subStruct,"NO_WATER_CONC_FLAG",
            errorMsgIdentifier,
            true);
    
     // Convert time units from host model units to seconds (OpenWQ time units)
    // 1) Calculate unit multiplers
    volume_unit_flag = OpenWQ_units.Calc_Unit_Multipliers(
                OpenWQ_wqconfig, OpenWQ_output,
                unit_multiplers,                            // multiplers (numerator and denominator)
                std::get<0>(OpenWQ_wqconfig.output_units),  // input units
                units,
                false);              // direction of the conversion: 
                                     // to native (true) or 
                                     // from native to desired output units (false)
    std::get<1>(OpenWQ_wqconfig.output_units) = unit_multiplers[0]; // multipler for numerator
    std::get<2>(OpenWQ_wqconfig.output_units) = unit_multiplers[1]; // multiupler for denominator
    std::get<3>(OpenWQ_wqconfig.output_units) = volume_unit_flag; // flag if denominator is volume

    // Output format
    output_format = OpenWQ_utils.RequestJsonKeyVal_str(
            OpenWQ_wqconfig, OpenWQ_output,
            json_output_subStruct,"FORMAT",
            errorMsgIdentifier,
            true);

    // ########################################
    // Output format ######

    // Create OpenWQ_wqconfig.output_dir folder if nonexistant
    OpenWQ_utils.check_mkdir(OpenWQ_wqconfig.output_dir);   
    OpenWQ_wqconfig.output_type = -1;   // reset

    // Create data type sub-folders if needed
    // CSV format
    if (output_format.compare("CSV") == 0){
        OpenWQ_wqconfig.output_type = 0;
        OpenWQ_wqconfig.output_dir.append("/CSV");
    // HDF5 format
    }else if (output_format.compare("HDF5") == 0){
        OpenWQ_wqconfig.output_type = 1;
        OpenWQ_wqconfig.output_dir.append("/HDF5");
    } else {
        // Create Message (Error Message)
        msg_string = 
            "<OpenWQ> ERROR: Output type unkown: " + output_format;
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig, msg_string, false, true); 
        exit(EXIT_FAILURE);
    }
    OpenWQ_utils.check_mkdir(OpenWQ_wqconfig.output_dir); 

    // ########################################
    // Time ######
    // Get print/output timestep AND convert to seconds
    // Get tuple (value, unit)
    json_time = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        json_output_subStruct,"TIMESTEP",
        errorMsgIdentifier,
        true);

    (OpenWQ_wqconfig.timetep_out) = json_time.at(0);       // Get value
    (OpenWQ_wqconfig.timestep_out_unit) = json_time.at(1); // Get units

    // Convert time units from host model units to seconds (OpenWQ time units)
    // 1) Calculate unit multiplers
    units.clear();                           // units (numerator and denominator)
    OpenWQ_units.Calc_Unit_Multipliers(
        OpenWQ_wqconfig, OpenWQ_output,
        unit_multiplers,                    // multiplers (numerator and denominator)
        OpenWQ_wqconfig.timestep_out_unit,  // input units
        units,                              // units (numerator and denominator)
        true);                              // direction of the conversion: 
                                            // to native (true) or 
                                            // from native to desired output units (false)
    
    // 2) Calculate value with new units
    OpenWQ_units.Convert_Units(
        OpenWQ_wqconfig.timetep_out,    // value passed by reference so that it can be changed
        unit_multiplers);               // units

}

// Set what to print
void OpenWQ_readjson::SetConfigInfo_output_what2print(
    OpenWQ_json &OpenWQ_json,
    OpenWQ_hostModelconfig & OpenWQ_hostModelconfig,
    OpenWQ_wqconfig &OpenWQ_wqconfig,
    OpenWQ_utils& OpenWQ_utils,
    OpenWQ_units &OpenWQ_units,
    OpenWQ_output& OpenWQ_output){
    
    // Local variables
    int nx;                                                 // interactive nx inforationfor each compartment
    int ny;                                                 // interactive ny inforationfor each compartment
    int nz;                                                 // interactive nz inforationfor each compartment
    int ix_json;                                            // iteractive ix info for COMPARTMENTS_AND_CELLS cell data 
    int iy_json;                                            // iteractive iy info for COMPARTMENTS_AND_CELLS cell data  
    int iz_json;                                            // iteractive iz info for COMPARTMENTS_AND_CELLS cell data  
    int spX_min, spX_max, spY_min, spY_max, spZ_min, spZ_max; // range of ix, iy, iz of cells to print (if selected "all")
    long num_compt2print;                                   // number of compartments to print
    std::string chem_name2print;                            // iteractive chem name from openWQ_OUTPUT
    std::string chem_namelist;                              // iteractive chem name from BGC list
    long num_chem2print;                                    // number of chemicals to print based on openWQ_OUTPUT               
    long num_cells2print;                                   // iteractive number of cells to print for each compartment
    std::vector<std::string> compt_names_vec;               // compartment names to print  
    std::string CompName_icmp;                              // iteractive compartment name from list
    std::string compt_name2print;                           // iteractive compartment name to print
    std::string cells_input;                                // interactive string for cells input for each compartment
    arma::mat cells2print_cmpt;                             // cumulative vector with all cells to print for each compartment
    bool noValPrintRequest_flag;                            // flag to through message if no valid compartments/cells have been selected for printing
    std::vector<int>::iterator it;                          // iteractor for flagging if compartment i has been selected for printing
    unsigned long long iRow;                                // dummy variable to track the number of rows for cell2print
    unsigned long long nRows;                               // dummy variable to store the total number of rows for cell2print
    std::string msg_string;
    json json_output_subStruct;
    json json_output_subStruct_CmpCells;
    std::string errorMsgIdentifier;

    // Chemicals to print
    // num of chem to print
    errorMsgIdentifier = "Master file";
    json_output_subStruct = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        OpenWQ_json.Master,"OPENWQ_OUTPUT",
        errorMsgIdentifier,
        true);

    errorMsgIdentifier = "Master file > OPENWQ_OUTPUT";
    num_chem2print = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        json_output_subStruct,"CHEMICAL_SPECIES",
        errorMsgIdentifier,
        true).size();
    
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

    errorMsgIdentifier = "Master file > OUTPUT";
    json_output_subStruct_CmpCells = OpenWQ_utils.RequestJsonKeyVal_json(
        OpenWQ_wqconfig, OpenWQ_output,
        json_output_subStruct,"COMPARTMENTS_AND_CELLS",
        errorMsgIdentifier,
        true);

    // num of comartments to print
    num_compt2print = json_output_subStruct_CmpCells.size();
    
    // Get list of compartments to print from COMPARTMENTS_AND_CELLS
    for (auto& x1 : json_output_subStruct_CmpCells.items()){
                
        try{
            compt_names_vec.push_back(x1.key());
        }catch (...){}
    }

    // Get indexes for the list of compartments requested
    for (unsigned int cmpti = 0; cmpti < num_compt2print; cmpti++){
        compt_name2print = compt_names_vec[cmpti]; 
        for (unsigned int icmp = 0; icmp < OpenWQ_hostModelconfig.get_num_HydroComp(); icmp++){     
            CompName_icmp = OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp);  // name
            // Check if compartments listed match internal compartment names
            if (CompName_icmp.compare(compt_name2print) == 0){                               
                OpenWQ_wqconfig.compt2print.push_back(icmp);
                break;
            }
        }
    }

    // ########################################
    // Cells to print 

    // Set noValPrintRequest_flag to default false
    // if no valid request are found, then this will remain false and a warning message will be sent
    // otherwise, this will change to true and all is good
    noValPrintRequest_flag = false;

    // Check what cells to print for each viable (compt2print) compartment requested
    for (unsigned int icmp = 0; icmp < OpenWQ_hostModelconfig.get_num_HydroComp(); icmp++){

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
        nx = OpenWQ_hostModelconfig.get_HydroComp_num_cells_x_at(icmp);
        ny = OpenWQ_hostModelconfig.get_HydroComp_num_cells_y_at(icmp);
        nz = OpenWQ_hostModelconfig.get_HydroComp_num_cells_z_at(icmp);
        
        // first set default no print
        // this gets true if loading of json input is sucessfull
        OpenWQ_wqconfig.cells2print_bool.push_back(false);

        // Get number of cell entries provided
        num_cells2print = json_output_subStruct_CmpCells
            [OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp)].size();

        // Loop over all cells
        for (unsigned int celli = 0; celli < num_cells2print; celli++){
            
            // Get ix value (as integer)
            try{
                ix_json = json_output_subStruct_CmpCells
                    [OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp)]
                    [std::to_string(celli + 1)]
                    .at(0);
                ix_json --; // remove 1 to match c++ convention to start in zero
            
            }catch(...){
                // if not integer, then check if "all"
                cells_input = json_output_subStruct_CmpCells
                    [OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp)]
                    [std::to_string(celli + 1)]
                    .at(0);

                if (cells_input.compare("ALL")==0){
                    // then, use "all" flag (=-1)
                    ix_json = -1;

                }else{
                    msg_string = 
                        "<OpenWQ> WARNING: Unkown entry for ix (" 
                        + cells_input
                        + ") for OPENWQ_OUTPUT > COMPARTMENTS_AND_CELLS > " 
                        + OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp)
                        + "(entry skipped)";

                    // Print it (Console and/or Log file)
                    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig,msg_string,true,true);

                    // not a valid entry
                    continue;
                    
                }
            }

            // Get iy value
            try{
                iy_json = json_output_subStruct_CmpCells
                    [OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp)]
                    [std::to_string(celli + 1)]
                    .at(1);
                iy_json--;  // remove 1 to match c++ convention to start in zero
            
            }catch(...){

                // if not integer, then check if "all"
                cells_input = json_output_subStruct_CmpCells
                    [OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp)]
                    [std::to_string(celli + 1)]
                    .at(1);

                if (cells_input.compare("ALL")==0){
                    // then, use "all" flag (=-1)
                    iy_json = -1;

                }else{
                    msg_string = 
                        "<OpenWQ> WARNING: Unkown entry for iy (" 
                        + cells_input
                        + ") for OPENWQ_OUTPUT > COMPARTMENTS_AND_CELLS > " 
                        + OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp)
                        + "(entry skipped)";

                    // Print it (Console and/or Log file)
                    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig,msg_string,true,true);

                    // not a valid entry
                    continue;

                }
            }

            // Get iz value
            try{
                iz_json = json_output_subStruct_CmpCells
                    [OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp)]
                    [std::to_string(celli + 1)]
                    .at(2);
                iz_json --; // remove 1 to match c++ convention to start in zero

            }catch(...){

                // if not integer, then check if "all"
                cells_input = json_output_subStruct_CmpCells
                    [OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp)]
                    [std::to_string(celli + 1)]
                    .at(2);

                if (cells_input.compare("ALL")==0){
                    // then, use "all" flag (=-1)
                    iz_json = -1;

                }else{
                    msg_string = 
                        "<OpenWQ> WARNING: Unkown entry for iz (" 
                        + cells_input
                        + ") for OPENWQ_OUTPUT > COMPARTMENTS_AND_CELLS > " 
                        + OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp)
                        + "(entry skipped)";

                    // Print it (Console and/or Log file)
                    OpenWQ_output.ConsoleLog(OpenWQ_wqconfig,msg_string,true,true);

                    // not a valid entry
                    continue;

                }
            }

            // Check if cell requested is witin the boundaries of the spatial domain
            // If yes, add the cell selected to cells2print_vec
            // Otherwise, write warning message (skip entry)
            if (ix_json <= nx
                && iy_json <= ny
                && iz_json <= nz){

                // ix
                if(ix_json != -1){spX_min = ix_json; spX_max = ix_json;}
                else{spX_min = 0; spX_max = nx - 1;}
                // iy
                if(iy_json != -1){spY_min = iy_json; spY_max = iy_json;}
                else{spY_min = 0; spY_max = ny - 1;}
                // iz
                if(iz_json != -1){spZ_min = iz_json; spZ_max = iz_json;}
                else{spZ_min = 0; spZ_max = nz - 1;}

                // Create arma mat to append
                nRows = (spX_max - spX_min + 1) * (spY_max - spY_min + 1) * (spZ_max - spZ_min + 1);
                arma::mat cells2print_row(nRows,3,arma::fill::zeros);       // iteractive vector with cell x, y and z indexes

                // Reset dummy variables
                iRow = 0;

                // add cell requested to list to print for each compartment
                // first create the vector cells2print_row with x, y and z values
                // loop is to account for "all" entries
                for (int ix_j = spX_min; ix_j <= spX_max; ix_j++){
                    for (int iy_j = spY_min; iy_j <= spY_max; iy_j++){
                        for (int iz_j = spZ_min; iz_j <= spZ_max; iz_j++){
                            
                            cells2print_row(iRow,0) = ix_j;
                            cells2print_row(iRow,1) = iy_j;
                            cells2print_row(iRow,2) = iz_j;
                            iRow++;

                        }
                    }
                }

                // add that cells2print_row to cells2print_cmpt
                cells2print_cmpt.insert_rows(
                    cells2print_cmpt.n_rows,
                    cells2print_row);

                // Reset dummy variables
                cells2print_row.zeros();

            }else{
                
                // Create Message (Error - locate problematic cell)
                msg_string = 
                    "<OpenWQ> ERROR: Cell entry provided out of domain"
                    " in OPENWQ_OUTPUT > COMPARTMENTS_AND_CELLS > "
                    + OpenWQ_hostModelconfig.get_HydroComp_name_at(icmp)
                    + "> '" + std::to_string(celli + 1) + "'";

                // Print it (Console and/or Log file)
                OpenWQ_output.ConsoleLog(OpenWQ_wqconfig,msg_string,true,true);

                // not a valid entry
                continue;

            }
                
        }

        // Add the complete list of cells to print
        OpenWQ_wqconfig.cells2print_vec.push_back(cells2print_cmpt);
        
         // all went well in "try", so set print to true
         // and noValPrintRequest_flag to true
        if(cells2print_cmpt.is_empty() == false){
            OpenWQ_wqconfig.cells2print_bool[icmp] = true;
            noValPrintRequest_flag = true;
        }

        // Clear cells2print_cmpt for reuse
        cells2print_cmpt.clear();

    }

    // If no valid output request are found, then through a warning message
    if (noValPrintRequest_flag == false){

        // Create Message (Error - locate problematic cell)
        msg_string = 
            "<OpenWQ> WARNING: No 'COMPARTMENTS_AND_CELLS' requests have been found. "
            "The model will not print any results. "
            "Revise the 'OPENWQ_OUTPUT' keys in the master-json file and try again.";

        // Print it (Console and/or Log file)
        OpenWQ_output.ConsoleLog(OpenWQ_wqconfig,msg_string,true,true);

    }

}