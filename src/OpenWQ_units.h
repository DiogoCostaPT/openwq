
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
#include <time.h>
#include "jnlohmann/json.hpp"
using json = nlohmann::json;

#include "OpenWQ_global.h"
#include "OpenWQ_output.h"

/* #################################################
// Functions to convert input units
// Both internal openWQ and host hydrological model units
################################################# */
class OpenWQ_units{

    public: 

    void Convert_Units(
        double& input_value,                      // input value (passed by reference)
        std::vector<double> unit_multiplers);   // multiplers (numerator and denominator) 

    bool Calc_Unit_Multipliers(
        OpenWQ_wqconfig& OpenWQ_wqconfig,
        OpenWQ_output& OpenWQ_output,
        std::vector<double>& unit_multiplers,   // multiplers (numerator and denominator)
        std::string input_unit,                 // input units
        std::vector<std::string>& units,        // units (numerator and denominator)
        bool direction_2native);                // direction of the conversion: 
                                                // to native (true) or 
                                                // from native to desired output units (false)

    bool Check_Mass_Units(
        std::string unit_i,         // unit of numerator or denominator
        double& unit_multipler_i);  // multiplier of numerator or denominator

    bool Check_Volume_Units(
        std::string unit_i,         // unit of numerator or denominator
        double& unit_multipler_i);  // multiplier of numerator or denominator

    bool Check_Time_Units(
        std::string unit_i,         // unit of numerator or denominator
        double& unit_multipler_i);  // multiplier of numerator or denominator

    time_t converTime_ints2time_t(
        int YYYY, 
        int MM, 
        int DD, 
        int HH, 
        int MIN,
        int SEC);

    time_t convert_timeChar2time_t(
    std::string datetime_str);
        
};

#endif

