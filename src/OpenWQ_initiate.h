

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

#ifndef OPENWQ_INITIATEH_INCLUDED
#define OPENWQ_INITIATEH_INCLUDED

#include <tuple>
#include <vector>
#include "jnlohmann/json.h"
using json = nlohmann::json;

#include "OpenWQ_global.h"
#include "OpenWQ_units.h"


class OpenWQ_initiate{
    
    public:
    
    // Allocate Memory
    void initmemory(
        OpenWQ_json& OpenWQ_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig);

    // Read and set simulation conditions and options
    void readSet(
        OpenWQ_json& OpenWQ_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_units& OpenWQ_units,
        const int icmp,
        const int ix,
        const int iy,
        const int iz,
        double igridcell_volume,  // all calculations assume unit = m3
        double iwater_volume);    // all calculations assume unit = m3

    void setIC(
        OpenWQ_json& OpenWQ_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_units& OpenWQ_units,
        const int icmp,
        const int ix,
        const int iy,
        const int iz,
        double igridcell_volume,  // all calculations assume unit = m3
        double iwater_volume);    // all calculations assume unit = m3

};


#endif