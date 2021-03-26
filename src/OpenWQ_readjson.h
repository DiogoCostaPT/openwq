
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

#ifndef OpenWQ_READJSONH_INCLUDED
#define OpenWQ_READJSONH_INCLUDED

#include <armadillo>
#include <string>
#include <algorithm>

#include "exprtk.hpp"

#include <cstdio>

#include "jnlohmann/json.h"
using json = nlohmann::json;

//#include "utility.h"

#include "OpenWQ_global.h"
#include "OpenWQ_units.h"

class OpenWQ_readjson{

    public:

    void read_all(
        OpenWQ_json& OpenWQ_json,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_units& OpenWQ_units);

    void read_JSON_2class(
        json& jsondata,                         // JSON structure to save to
        const bool substruc_flag,               // Save in subfield of JSON structure? only if multiple files (e.g., source and sinks)        
        const std::string JsonSubStruct_name,   // if true, name of subfield    
        const std::string jsonfile);            // Name of JSON file

    void ConvertJSONtext_2upperCase(
        json &jsondata);
    
    void ConvertStringToUpperCase(
        const std::string &old_jsonkey,
        std::string &new_jsonkey);

    void change_JSON_key_to_upper_case(
        json &object, 
        const std::string& old_key,
        std::string& new_key);

    void SetConfigInfo(
        OpenWQ_json &OpenWQ_json,
        OpenWQ_wqconfig &OpenWQ_wqconfig,
        OpenWQ_units &OpenWQ_units);
};

#endif

