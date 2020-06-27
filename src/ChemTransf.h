
#ifndef CHEMTRANSFH_INCLUDED
#define CHEMTRANSFH_INCLUDED

#include "global.h"
#include "exprtk.hpp"

#include <cstdio>
#include <string>

void ChemTransf(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, int icmp);

void Transf(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, int icmp, int transi);

#endif