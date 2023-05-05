

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
#include "units/OpenWQ_units.hpp"
#include "output/OpenWQ_output.hpp"
#include "utils/OpenWQ_utils.hpp"


class OpenWQ_initiate{
    
    public:
        // Allocate Memory
        // initiate_decl.cpp
        void initmemory(
            OpenWQ_json& OpenWQ_json,
            OpenWQ_vars& OpenWQ_vars,
            OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
            OpenWQ_wqconfig& OpenWQ_wqconfig,
            OpenWQ_output& OpenWQ_output);

        
        // Update time variables
        // initiate_runtimestart.cpp
        void setTimeVars(
            OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
            OpenWQ_wqconfig& OpenWQ_wqconfig,
            time_t simtime);

    

        // Read and set simulation conditions and options
        // initiate_runtimestart.cpp
        void readSet(
            OpenWQ_json& OpenWQ_json,
            OpenWQ_vars& OpenWQ_vars,
            OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
            OpenWQ_wqconfig& OpenWQ_wqconfig,
            OpenWQ_utils& OpenWQ_utils,
            OpenWQ_units& OpenWQ_units,
            OpenWQ_output& OpenWQ_output);

    private:
        // Set initial conditions
        // initiate_runtimestart.cpp        
        void setIC_driver(
            OpenWQ_json& OpenWQ_json,
            OpenWQ_vars& OpenWQ_vars,
            OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
            OpenWQ_wqconfig& OpenWQ_wqconfig,
            OpenWQ_utils& OpenWQ_utils,
            OpenWQ_units& OpenWQ_units,
            OpenWQ_output& OpenWQ_output,
            const int icmp);
        
        // initiate_runtimestart.cpp
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

        // initiate_runtimestart.cpp
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
};


#endif