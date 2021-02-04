
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

#include "DEMOS_OpenWQ_load.h"

// Read JSON file to class
void DEMOS_OpenWQ_load::read_JSON_2class(
        json& jsondata,
        const std::string& jsonfile){

        try{
                std::ifstream i(jsonfile);
                i >> (jsondata);

        }catch (json::type_error){
                std::cout << "An exception occurred parsing" << jsonfile << std::endl;
                abort();
        }
}


 // Real all configuration files
void DEMOS_OpenWQ_load::loadinit(
        DEMOS_OpenWQ_json& DEMOS_OpenWQ_json){
        
        // Read Master file name
        const std::string DEMOS_OpenWQ_masterjson = "openWQ_master.json";
        read_JSON_2class(
                DEMOS_OpenWQ_json.Master,
                DEMOS_OpenWQ_masterjson); // master file

        // Read other JSON configuration files defined in Master file 
        read_JSON_2class(
                DEMOS_OpenWQ_json.Config,
                DEMOS_OpenWQ_json.Master["openWQ_config_file"]); // main confirguration

        read_JSON_2class(
                DEMOS_OpenWQ_json.BGCcycling,
                DEMOS_OpenWQ_json.Master["openWQ_BGC_cycling_file"]); // BGCcycling cycling

}