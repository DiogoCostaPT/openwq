
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

#ifndef PROCESS_JSONH_INCLUDED
#define PROCESS_JSONH_INCLUDED

#include <string>
#include <fstream>
#include <iostream>
#include "jnlohmann/json.h"
#include "global.h"

using json = nlohmann::json;

void read_json_configfile(Prj_GenInfo& Prj_GenInfo, const std::string& configjson_file);

void read_json_getfilefield(std::string &jsonfile, const char jsonfield[], int &go_to_global);

void  read_json_comptdim(Prj_GenInfo& Prj_GenInfo,std::unique_ptr<unsigned int[]> & n_xyz, int umcmp,int numspec);

#endif