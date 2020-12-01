
#ifndef TRANSPSOLVERH_INCLUDED
#define TRANSPSOLVERH_INCLUDED

#include <armadillo>
#include "string.h"
#include <algorithm>

#include "DEMOS_OpenWQ_global.h"
#include "utility.h"
#include "readSetFluxes.h"
#include "main_solver.h"
#include "writeVTU.h"
#include "ChemTransf.h"
#include "ADE_solver.h"

void main_solver(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars);

#endif