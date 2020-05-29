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

void read_json_infosize(const std::string& configjson_file)
{

        ClassGenProj ClassGP; 

        std::ifstream i(configjson_file);
        json j;
        i >> j;

        try{
                ClassGP.geninfo.project_name = j["project_name"];
                ClassGP.geninfo.geographical_location = j["geographical_location"];
                ClassGP.geninfo.authors = j["authors"];
                ClassGP.geninfo.date = j["date"];
                ClassGP.geninfo.additional_info = j["additional_info"];
                ClassGP.size.number_of_compartments = j["number_of_compartments"];
                ClassGP.size.number_of_chemical_species = j["number_of_chemical_species"]; 
                ClassGP.size.dim_nx = j["dimensions_nx"];
                ClassGP.size.dim_ny = j["dimensions_ny"];
                ClassGP.size.dim_nz = j["dimensions_nz"];

        }catch (json::type_error){
                std::cout << "An exception occurred parsing" << configjson_file << std::endl;
        }

        

    
}