
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

#include "OpenWQ_load.h"

// Read JSON file to class
void OpenWQ_load::read_JSON_2class(
        json& jsondata,
        const std::string& jsonfile){

        try{
                std::ifstream i(jsonfile);
                i >> (jsondata);

        }catch (const std::exception& e){
                std::cout << "ERROR: An exception occurred parsing JSON file: " << jsonfile << std::endl;
                exit (EXIT_FAILURE);
        }
}


 // Real all configuration files
void OpenWQ_load::loadinit(
        OpenWQ_json& OpenWQ_json){
        
        // Read Master file name
        const std::string OpenWQ_masterjson = "openWQ_master.json";
        read_JSON_2class(
                OpenWQ_json.Master,
                OpenWQ_masterjson); // master file

        // Read other JSON configuration files defined in Master file 
        read_JSON_2class(
                OpenWQ_json.Config,
                OpenWQ_json.Master["openWQ_config_file"]); // main confirguration

        read_JSON_2class(
                OpenWQ_json.BGCcycling,
                OpenWQ_json.Master["openWQ_BGC_cycling_file"]); // BGCcycling cycling

}