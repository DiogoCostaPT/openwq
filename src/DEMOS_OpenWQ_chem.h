
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


#ifndef DEMOS_OPENWQ_CHEMH_INCLUDED
#define DEMOS_OPENWQ_CHEMH_INCLUDED

#include <armadillo>
#include "string.h"
#include <algorithm>
#include "exprtk.hpp"
#include <cstdio>
#include <tuple>

// #include "utility.h"

#include "DEMOS_OpenWQ_global.h"

// Biogeochemistry

class DEMOS_OpenWQ_chem{

    public:

    void Run(
        DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,
        DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars,
        DEMOS_OpenWQ_hostModelconfig& DEMOS_OpenWQ_hostModelconfig);

    void BGC_Transform(
        DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,
        DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars,
        DEMOS_OpenWQ_hostModelconfig& DEMOS_OpenWQ_hostModelconfig,
        int icmp);

};

#endif