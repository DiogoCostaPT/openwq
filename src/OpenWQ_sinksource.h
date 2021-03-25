
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
#include "OpenWQ_units.h"

class OpenWQ_sinksource{

    public:

    // Save Sink and Source data to tuple (more efficient then allways calling jnlohmann)
    void SetSinkSource(
        OpenWQ_json& OpenWQ_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_units& OpenWQ_units);

    // Check if sink or sources needs to be applied
    void CheckApply(
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        const unsigned int YYYY,                       // current model step: Year
        const unsigned int MM,                         // current model step: month
        const unsigned int DD,                         // current model step: day
        const unsigned int HH);                        // current model step: hour

    // Apply Sink or Source
    void Apply(
        OpenWQ_json& OpenWQ_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        const unsigned int ssf,     // source-sink file
        const unsigned int ssi,     // load section                 
        const unsigned int di);     // data row

    // Apply Source
    void Apply_Source(
        OpenWQ_vars& OpenWQ_vars,
        const unsigned int cmpi,               // compartment model index
        const unsigned int chemi,              // chemical model index    
        const unsigned int ix_json,            // compartment model ix
        const unsigned int iy_json,            // compartment model iy
        const unsigned int iz_json,            // compartment model iz
        const double ss_data_json);            // source load

    // Apply Sink
    void Apply_Sink(
        OpenWQ_vars& OpenWQ_vars,
        const unsigned int cmpi,                // compartment model index
        const unsigned int chemi,               // chemical model index    
        const unsigned int ix_json,             // compartment model ix
        const unsigned int iy_json,             // compartment model iy
        const unsigned int iz_json,             // compartment model iz
        const double ss_data_json);             // source load

    void Convert_Mass_Units(
        double &ss_value,     // SS value
        std::string ss_unit);  // SS unit

    bool getModIndex(
        std::vector<std::string> &vec_list,
        std::string &obj_name,
        std::string &obj_text,
        unsigned long &vec_obj_index);

};

#endif
