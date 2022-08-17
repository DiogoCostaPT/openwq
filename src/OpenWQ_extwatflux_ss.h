
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

#ifndef OPENWQ_EXTWATFLUX_SSH_INCLUDED
#define OPENWQ_EXTWATFLUX_SSH_INCLUDED


#include <tuple>
#include <string>
#include <algorithm>
#include <time.h>
#include "jnlohmann/json.hpp"
using json = nlohmann::json;

#include "OpenWQ_global.h"
#include "OpenWQ_units.h"
#include "OpenWQ_output.h"

class OpenWQ_extwatflux_ss{

    public:

    // Save Sink and Source data to tuple (more efficient than allways calling jnlohmann)
    void Set_EWFandSS(
        json &EWF_SS_json,
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_units& OpenWQ_units,
        OpenWQ_output& OpenWQ_output,
        std::string inputType);

    // Check if sink or sources needs to be applied
    void CheckApply_SS(
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_units& OpenWQ_units,
        OpenWQ_output& OpenWQ_output,
        const unsigned int YYYY,         // current model step: Year
        const unsigned int MM,           // current model step: month
        const unsigned int DD,           // current model step: day
        const unsigned int HH,           // current model step: hour
        const unsigned int MIN,          // current model step: min
        const unsigned int SEC);         // current model step: sec

    // Apply Source
    void Apply_Source(
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_output& OpenWQ_output,
        const unsigned int cmpi,                        // compartment model index
        const unsigned int chemi,                       // chemical model index    
        int ix_json,                                    // compartment model ix
        int iy_json,                                    // compartment model iy
        int iz_json,                                    // compartment model iz
        const double ss_data_json);                     // source load

    // Apply Sink
    void Apply_Sink(
        OpenWQ_vars& OpenWQ_vars,
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
        OpenWQ_output& OpenWQ_output,
        const unsigned int cmpi,                        // compartment model index
        const unsigned int chemi,                       // chemical model index    
        int ix_json,                                    // compartment model ix
        int iy_json,                                    // compartment model iy
        int iz_json,                                    // compartment model iz
        const double ss_data_json);                     // source load

    void Convert_Mass_Units(
        double &ss_value,     // SS value
        std::string ss_unit);  // SS unit

    bool getModIndex(
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_output& OpenWQ_output,
        std::vector<std::string> &vec_list,
        std::string &obj_name,
        std::string &obj_text,
        unsigned long &vec_obj_index);

    bool getArrayElem( // elemEntry as string "all"
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_output& OpenWQ_output,
        std::string elemName,
        std::__cxx11::basic_string<char> elemEntry,
        int& elemVal,
        unsigned int& file_i,
        unsigned int& struc_i,
        unsigned int& row_i);

    void RemoveLoadBeforeSimStart(
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_units& OpenWQ_units,
        const int YYYY,             // current model step: Year
        const int MM,               // current model step: month
        const int DD,               // current model step: day
        const int HH,               // current model step: hour
        const int MIN,              // current model step: min
        const int SEC);             // current model step: sec

    void UpdateAllElemTimeIncremts(
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_units& OpenWQ_units,
        const int YYYY,             // current model step: Year
        const int MM,               // current model step: month
        const int DD,               // current model step: day
        const int HH,               // current model step: hour
        const int MIN,              // current model step: min
        const int SEC);             // current model step: sec

    int getNumberOfDays(
        const unsigned int YYYY_check,          // json: Year 
        const unsigned int MM_check);           // json: Month

};

#endif
