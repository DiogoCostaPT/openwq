
#ifndef TRANSPSOLVERH_INCLUDED
#define TRANSPSOLVERH_INCLUDED

#include <armadillo>
#include "string.h"
#include <algorithm>

#include "global.h"
#include "utility.h"
#include "readSetFluxes.h"
#include "transp_solver.h"
#include "writeVTU.h"

void transp_solve(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar);

#endif