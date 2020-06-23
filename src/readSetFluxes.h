

#ifndef READSETFLUXESH_INCLUDED
#define READSETFLUXESH_INCLUDED

#include "global.h"

void readSetFluxes(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar,
    std::vector<int> mobileCompt,std::string& fluxes_filenamesExtention);

#endif