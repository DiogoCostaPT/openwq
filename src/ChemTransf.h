
#ifndef CHEMTRANSFH_INCLUDED
#define CHEMTRANSFH_INCLUDED

#include "global.h"
#include "exprtk.hpp"

#include <cstdio>
#include <string>
#include <string>

void ChemTransf(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, int icmp);

void Transf(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, int icmp, int transi);

void ChemCompExchange(JSONfiles& JSONfile, Prj_StateVar& Prj_StateVar, int source, std::string kinetics, 
    std::vector<std::string> parameter_names, std::vector<double> parameter_values,
    std::array<double,7> & linedata, int & index_chem);

#endif