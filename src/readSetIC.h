
#ifndef READSETICH_INCLUDED
#define READSETICH_INCLUDED

#include <armadillo>
#include "global.h"
#include "utility.h"
#include "string.h"
#include <algorithm>

using json = nlohmann::json;

void readSetIC(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar);

#endif