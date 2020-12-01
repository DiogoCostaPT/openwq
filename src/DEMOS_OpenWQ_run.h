

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


#ifndef DEMOS_OPENWQ_RUNH_INCLUDED
#define DEMOS_OPENWQ_RUNH_INCLUDED

#include <armadillo>
#include "string.h"
#include <algorithm>

#include "exprtk.hpp"

#include <cstdio>

#include "utility.h"
#include "readSetFluxes.h"
#include "ADE_solver.h"


#include "DEMOS_OpenWQ_global.h"
#include "DEMOS_OpenWQ_chem.h"
#include "DEMOS_OpenWQ_print.h"



class DEMOS_OpenWQ_run{

    public:

    void callrun(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json);


    void ChemCompExchange(DEMOS_OpenWQ_json& JSONfile, DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, int source, std::string kinetics, 
        std::vector<std::string> parameter_names, std::vector<double> parameter_values,
        std::array<double,7> & linedata, int & index_chem);

};

#endif