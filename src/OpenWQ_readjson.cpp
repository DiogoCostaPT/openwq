
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
        OpenWQ_json& OpenWQ_json){
        
        /* ########################
        // Read Master file name
        ######################## */
        const std::string OpenWQ_masterjson = "openWQ_master.json";
        read_JSON_2class(
                OpenWQ_json.Master,      // JSON structure to save to
                false,                  // Save in subfield of JSON structure? only if multiple files (e.g., source and sinks)   
                "",                     // if above true, provide name of subfield        
                OpenWQ_masterjson);     // Name of JSON file

        /* ########################
        // // Read other JSON configuration files defined in Master file 
        ######################## */
        // Main confirguration
        read_JSON_2class(
                OpenWQ_json.Config,                                     
                false,                                                  
                "",                                                                        
                OpenWQ_json.Master["openWQ_INPUT"]["Config_file"]);     

        // BGCcycling cycling
        read_JSON_2class(
                OpenWQ_json.BGCcycling,                                 
                false,
                "",
                OpenWQ_json.Master["openWQ_INPUT"]["BGC_cycles_file"]); 

        // SinkSource
        int num_ssf = OpenWQ_json.Master["openWQ_INPUT"]["SinkSource_files"].size();
        for (int ssf=0;ssf<num_ssf;ssf++){
        read_JSON_2class(
                OpenWQ_json.SinkSource,
                true,
                std::to_string(ssf),
                OpenWQ_json.Master["openWQ_INPUT"]["SinkSource_files"][std::to_string(ssf+1)]["filepath"]); 
        }

}

/* #################################################
// Read JSON file to class
################################################# */
void OpenWQ_readjson::read_JSON_2class(
        json& jsondata,                         // JSON structure to save to
        const bool substruc_flag,               // Save in subfield of JSON structure? only if multiple files (e.g., source and sinks)        
        const std::string JsonSubStruct_name,   // if true, name of subfield    
        const std::string jsonfile){            // Name of JSON file
        
        /* ####################################
        // Save JSON file in JSON data structure
        #################################### */
        try{
                // Read json file with ifstrem
                std::ifstream i(jsonfile);

                //* ##########################
                // If substruc_flag = true
                // save JSON inside JsonSubStruct_name field in jsondata
                //* ####################################
                if (substruc_flag == false){
                        i >> (jsondata);
                }else{
                        i >> (jsondata[JsonSubStruct_name]);
                }

        }
        /* ####################################
        // Handle exceptions 
        #################################### */
        catch (const std::exception& e){
                std::cout << "ERROR: An exception occurred parsing JSON file: " << jsonfile << std::endl;
                exit (EXIT_FAILURE);
        }
}