
#ifndef ADESOLVERH_INCLUDED
#define ADESOLVERH_INCLUDED

#include "global.h"

void ADE_solver(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar,int &icmpMob,int &ichem,
    arma::cube& wfluxC_x,arma::cube& wfluxC_y,arma::cube& wfluxC_z,arma::cube& wmassC);

#endif