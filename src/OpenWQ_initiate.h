

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
#include "jnlohmann/json.hpp"
using json = nlohmann::json;

#include "global/OpenWQ_json.hpp"
#include "global/OpenWQ_vars.hpp"
#include "global/OpenWQ_wqconfig.hpp"
#include "global/OpenWQ_hostModelconfig.hpp"
#include "OpenWQ_units.h"
#include "OpenWQ_output.h"
#include "OpenWQ_utils.hpp"


class OpenWQ_initiate{
    
    public:
    
    // Allocate Memory
    void initmemory(
        OpenWQ_json& OpenWQ_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_output& OpenWQ_output);

    // Read and set simulation conditions and options
    void readSet(
        OpenWQ_json& OpenWQ_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_utils& OpenWQ_utils,
        OpenWQ_units& OpenWQ_units,
        OpenWQ_output& OpenWQ_output);

    // Set initial conditions
    void setIC_driver(
        OpenWQ_json& OpenWQ_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_utils& OpenWQ_utils,
        OpenWQ_units& OpenWQ_units,
        OpenWQ_output& OpenWQ_output,
        const int icmp);

    void setIC_json(
        OpenWQ_json& OpenWQ_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_utils& OpenWQ_utils,
        OpenWQ_units& OpenWQ_units,
        OpenWQ_output& OpenWQ_output,
        const int icmp,
        const int chemi);

    void setIC_h5(
        OpenWQ_json& OpenWQ_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_utils&nOpenWQ_utils,
        OpenWQ_units& OpenWQ_units,
        OpenWQ_output& OpenWQ_output,
        const int icmp,
        const int chemi);

    // Update time variables
    void setTimeVars(
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        time_t simtime);
};


#endif