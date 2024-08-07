
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
        OpenWQ_units &OpenWQ_units)
{

        /* ########################
        // Read JSON files
        ######################## */

        // ########################
        // Master file json

        // read
        OpenWQ_readjson::read_JSON_2class(
            OpenWQ_json.Master,                 // JSON structure to save to
            false,                              // Save in subfield of JSON structure? only if multiple files (e.g., source and sinks)
            "",                                 // if above true, provide name of subfield
            OpenWQ_wqconfig.OpenWQ_masterjson); // Name of JSON file

        // Main confirguration json (read)
        OpenWQ_readjson::read_JSON_2class(
            OpenWQ_json.Config,
            false,
            "",
            OpenWQ_json.Master["OPENWQ_INPUT"]["CONFIG_FILEPATH"]);

        // BGCcycling cycling json (read)
        OpenWQ_readjson::read_JSON_2class(
            OpenWQ_json.BGCcycling,
            false,
            "",
            OpenWQ_json.Master["OPENWQ_INPUT"]["BGC_CYCLES_FILEPATH"]);

        // SinkSource json (read)openWQ_OUTPUT
        unsigned int num_ssf = OpenWQ_json.Master["OPENWQ_INPUT"]["SINKSOURCE_FILES"].size();
        for (unsigned int ssf = 0; ssf < num_ssf; ssf++)
        {
                read_JSON_2class(
                    OpenWQ_json.SinkSource,
                    true,
                    std::to_string(ssf + 1),
                    OpenWQ_json.Master["OPENWQ_INPUT"]["SINKSOURCE_FILES"][std::to_string(ssf + 1)]["FILEPATH"]);
        }

        
        /* ########################
        // Process some of the openWQ config info
        ######################## */
        OpenWQ_readjson::SetConfigInfo(
                OpenWQ_json,
                OpenWQ_hostModelconfig,
                OpenWQ_wqconfig,
                OpenWQ_units);

}

/* #################################################
// Read JSON file to class
################################################# */
void OpenWQ_readjson::read_JSON_2class(
    json &jsondata,                       // JSON structure to save to
    const bool substruc_flag,             // Save in subfield of JSON structure? only if multiple files (e.g., source and sinks)
    const std::string JsonSubStruct_name, // if true, name of subfield
    const std::string jsonfile)
{ // Name of JSON file

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
                std::cout << "<OpenWQ> ERROR: An exception occurred parsing JSON file: " << jsonfile << std::endl;
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
        OpenWQ_units &OpenWQ_units){

        // Local Variables
        std::tuple<unsigned int, std::string> time_tuple;       // IC information in config file
        std::string output_format;                              // format of output (CSV, VTK)
        long num_chem2print;                                    // number of chemicals to print based on openWQ_OUTPUT               
        long num_compt2print;                                   // number of compartments to print
        std::string chem_name2print;                            // iteractive chem name from openWQ_OUTPUT
        std::string chem_namelist;                              // iteractive chem name from BGC list
        std::string CompName_icmp;                              // iteractive compartment name from list
        std::string compt_name2print;                           // iteractive compartment name to print

        // ########################################
        // CHEMISTRY
        // ########################################

        // Get number of chemical species from BGC_json
        (OpenWQ_wqconfig.num_chem) = OpenWQ_json.BGCcycling
                ["CHEMICAL_SPECIES"]["LIST"].size();

        // Get chemical species list from BGC_json
        for (unsigned int chemi = 0; chemi < (OpenWQ_wqconfig.num_chem); chemi++)
        {
                (OpenWQ_wqconfig.chem_species_list).push_back(OpenWQ_json.BGCcycling
                        ["CHEMICAL_SPECIES"]["LIST"][std::to_string(chemi + 1)]);
        }

        // ########################################
        // Set Output options
        // ########################################
        
        // Output format
        output_format = OpenWQ_json.Master
                ["OPENWQ_OUTPUT"]["FORMAT"];

        // Output folder
        OpenWQ_wqconfig.output_dir = OpenWQ_json.Master
                ["OPENWQ_OUTPUT"]
                ["RESULTS_FOLDERPATH"]; 
        
        // ########################################
        // Output format ######

        // Create OpenWQ_wqconfig.output_dir folder if nonexistant
        OpenWQ_readjson::check_mkdir_openWQ(
                OpenWQ_wqconfig.output_dir);
                
        OpenWQ_wqconfig.output_type = -1;               // reset

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
                std::cout << "<OpenWQ> ERROR: Output type unkown: " << output_format << std::endl;
        }

        // ########################################
        // Time ######
        // Get print/output timestep AND convert to seconds
        time_tuple = OpenWQ_json.Master
                ["OPENWQ_OUTPUT"]["TIMESTEP"];  // Get tuple (value, unit)
        (OpenWQ_wqconfig.timetep_out) = std::get<0>(time_tuple);       // Get value
        (OpenWQ_wqconfig.timestep_out_unit) = std::get<1>(time_tuple); // Get units

        // Convert time units from host model units to seconds (OpenWQ time units)
        OpenWQ_units.Convert_Time_Units(
                OpenWQ_wqconfig.timetep_out,
                OpenWQ_wqconfig.timestep_out_unit);

        // ########################################
        // Chemicals to print
        // num of chem to print
        num_chem2print = OpenWQ_json.Master
                ["OPENWQ_OUTPUT"]["CHEMICAL_SPECIES"].size();
        
        for (unsigned int chemi = 0; chemi < num_chem2print; chemi++){

                // Chemical name (to print)
               chem_name2print = OpenWQ_wqconfig.chem_species_list[chemi];
               
                for (unsigned int chemlisti = 0; chemlisti < (OpenWQ_wqconfig.num_chem); chemlisti++){
                        
                        chem_namelist = OpenWQ_wqconfig.chem_species_list[chemlisti];

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
                ["OPENWQ_OUTPUT"]["COMPARTMENTS"].size();
        
        // Get chemical species names to print 
        std::vector<std::string> compt_names_vec = OpenWQ_json.Master
                ["OPENWQ_OUTPUT"]["COMPARTMENTS"];

    
        for (unsigned int cmpti = 0; cmpti < num_compt2print; cmpti++){

               compt_name2print = compt_names_vec[cmpti];
               
                for (unsigned int icmp = 0; icmp < OpenWQ_hostModelconfig.num_HydroComp; icmp++){
                        
                        CompName_icmp = std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(icmp));  // name

                        if (CompName_icmp.compare(compt_name2print) == 0){                               
                                
                                OpenWQ_wqconfig.compt2print.push_back(icmp);
                                break;

                        }
                }
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