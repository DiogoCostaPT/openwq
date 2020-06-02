// Copyright 2020, Diogo Costa
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


#include "process_json.h"


void read_json_configfile(Prj_GenInfo& Prj_GenInfo, const std::string& configjson_file)
{
        try{
                std::ifstream i(configjson_file);
                json jsonfile;
                i >> jsonfile;

                Prj_GenInfo.project_name = jsonfile["project_name"];
                Prj_GenInfo.geographical_location = jsonfile["geographical_location"];
                Prj_GenInfo.authors = jsonfile["authors"];
                Prj_GenInfo.date = jsonfile["date"];
                Prj_GenInfo.additional_info = jsonfile["additional_info"];
                Prj_GenInfo.water_balance_setup_file = jsonfile["water_balance_setup_file"];
                Prj_GenInfo.biogeochemistry_setup_file = jsonfile["biogeochemistry_setup_file"];

        }catch (json::type_error){
                std::cout << "An exception occurred parsing" << configjson_file << std::endl;
                abort();
        }
}


// Get specific field from JSON file
void read_json_getfilefield(std::string &jsonfile, const char jsonfield[], int &go_to_global)
{      
         try{
                std::ifstream i(jsonfile);
                json jsonfile_data;
                i >> jsonfile_data;

                go_to_global = jsonfile_data[jsonfield];

        }catch (json::type_error){
                std::cout << "An exception occurred parsing" << 
                        jsonfile << std::endl;
                abort();
        }
}


// Get dimensions of compartments
void  read_json_comptdim(Prj_GenInfo& Prj_GenInfo,std::unique_ptr<unsigned int[]> & n_xyz, int umcmp,int numspec)
{

        try{
                std::ifstream i(Prj_GenInfo.water_balance_setup_file);
                json jsonfile;
                i >> jsonfile;

                //Prj_Dim.num_compartments = jsonfile["number_of_compartments"];

        }catch (json::type_error){
                std::cout << "An exception occurred parsing" << 
                        Prj_GenInfo.water_balance_setup_file << std::endl;
                abort();
        }

}