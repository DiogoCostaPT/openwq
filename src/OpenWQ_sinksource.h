
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

#ifndef OPENWQ_SINKSOURCEH_INCLUDED
#define OPENWQ_SINKSOURCEH_INCLUDED


#include <tuple>
#include <string>
#include <algorithm>
#include "jnlohmann/json.h"
using json = nlohmann::json;

#include "OpenWQ_global.h"

class OpenWQ_sinksource{

    public:

    void CheckApply(
        OpenWQ_json& OpenWQ_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        const unsigned int YYYY,                       // current model step: Year
        const unsigned int MM,                         // current model step: month
        const unsigned int DD,                         // current model step: day
        const unsigned int HH);                        // current model step: hour

    void Apply_Source(
        OpenWQ_vars& OpenWQ_vars,
        const unsigned int cmpi,               // compartment model index
        const unsigned int chemi,              // chemical model index    
        const unsigned int ix_json,            // compartment model ix
        const unsigned int iy_json,            // compartment model iy
        const unsigned int iz_json,            // compartment model iz
        const double ss_data_json);            // source load

    void Apply_Sink(
        OpenWQ_vars& OpenWQ_vars,
        const unsigned int cmpi,                // compartment model index
        const unsigned int chemi,               // chemical model index    
        const unsigned int ix_json,             // compartment model ix
        const unsigned int iy_json,             // compartment model iy
        const unsigned int iz_json,             // compartment model iz
        const double ss_data_json);             // source load

    void Transform_SS_Units(
        double &ss_value,     // SS value
        std::string ss_unit);  // SS unit


};

#endif
