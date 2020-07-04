

#ifndef READSETFLUXESH_INCLUDED
#define READSETFLUXESH_INCLUDED

#include "global.h"
#include "utility.h"
#include "string.h"

void readSetFluxes(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar,
    std::vector<int> mobileCompt,std::string& fluxes_filenamesExtention,
    int tmpst);

void readCompInteract(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, std::string& filenamesExtention,
    int tmpst);

#endif