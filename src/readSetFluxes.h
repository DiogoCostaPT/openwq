

#ifndef READSETFLUXESH_INCLUDED
#define READSETFLUXESH_INCLUDED

#include "DEMOS_OpenWQ_global.h"
#include "utility.h"
#include "string.h"

void readSetFluxes(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars,
    std::vector<int> mobileCompt,std::string& fluxes_filenamesExtention,
    int tmpst);

void readCompInteract(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, std::string& filenamesExtention,
    int tmpst);

#endif