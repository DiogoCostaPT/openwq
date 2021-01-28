
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
void DEMOS_OpenWQ_load::read_JSON_2class(json & jsondata,const std::string& jsonfile)
{
        try{
                std::ifstream i(jsonfile);
                i >> (jsondata);

        }catch (json::type_error){
                std::cout << "An exception occurred parsing" << jsonfile << std::endl;
                abort();
        }
}


 // Real all configuration files
void DEMOS_OpenWQ_load::loadinit(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,
    std::string& DEMOS_OpenWQ_configjson,
    int& numcmp, 
    int& numinteract){

    read_JSON_2class(DEMOS_OpenWQ_json.Master,DEMOS_OpenWQ_configjson); // master file
    read_JSON_2class(DEMOS_OpenWQ_json.H2O,DEMOS_OpenWQ_json.Master["water_balance_setup_file"]); // H2O file
    read_JSON_2class(DEMOS_OpenWQ_json.CMPI,DEMOS_OpenWQ_json.Master["cmp_interaction_file"]); // CMP file
    read_JSON_2class(DEMOS_OpenWQ_json.WQ,DEMOS_OpenWQ_json.Master["wq_balance_setup_file"]); // WQ file
    read_JSON_2class(DEMOS_OpenWQ_json.BGC,DEMOS_OpenWQ_json.Master["BGC_cycling_setup_file"]); // BGC file

    // Assign the main arma::field variables
    numcmp = DEMOS_OpenWQ_json.H2O["compartments"].size(); // number of compartments
    numinteract = DEMOS_OpenWQ_json.CMPI["interactions"].size(); // number of interactions between compartments

}