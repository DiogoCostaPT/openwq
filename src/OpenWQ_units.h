
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

#ifndef OPENWQ_UNITSH_INCLUDED
#define OPENWQ_UNITSH_INCLUDED

#include <tuple>
#include <vector>
#include "jnlohmann/json.hpp"
using json = nlohmann::json;

#include "OpenWQ_global.h"

/* #################################################
// Functions to convert input units
// Both internal openWQ and host hydrological model units
################################################# */
class OpenWQ_units{

    public: 

    // IC conditions
    void Convert_IC_Units(
        double &ic_value,       // IC value of chemical (passed by reference)
        std::string ic_type,    // IC value type of chemical (mass or concentration)
        std::string ic_units);

    // Sink & Source
    void Convert_SS_Units(
        double &ss_value,       // SS value
        std::string ss_unit);   // SS unit

    // Time step
    void Convert_Time_Units(
        double &time_value,      // SS value
        std::string time_unit);

};

#endif

