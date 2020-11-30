
#ifndef DEMOS_OPENWQ_MODELCONFIGH_INCLUDED

// Copyright 2020, Diogo Costa (diogo.pinhodacosta@canada.ca)
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


#define DEMOS_OPENWQ_MODELCONFIGH_INCLUDED


#include "global.h"

using json = nlohmann::json;


class DEMOS_OpenWQ{
    public:
    
    void read_JSON_2class(json & jsondata,const std::string& jsonfile);

    void ModConfig(JSONfiles& JSONfiles,const std::string configjson_file);

};


#endif