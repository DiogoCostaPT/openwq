
#ifndef TRANSPSOLVERH_INCLUDED
#define TRANSPSOLVERH_INCLUDED

#include <armadillo>
#include "string.h"
#include <algorithm>

#include "global.h"
#include "utility.h"
#include "readSetFluxes.h"
#include "main_solver.h"
#include "writeVTU.h"
#include "ChemTransf.h"
#include "ADE_solver.h"

void main_solver(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar);

#endif