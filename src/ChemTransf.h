
#ifndef CHEMTRANSFH_INCLUDED
#define CHEMTRANSFH_INCLUDED

#include "DEMOS_OpenWQ_global.h"
#include "exprtk.hpp"

#include <cstdio>
#include <string>
#include <string>

void ChemTransf(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, int icmp);

void Transf(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, int icmp, int transi);

void ChemCompExchange(DEMOS_OpenWQ_json& JSONfile, DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, int source, std::string kinetics, 
    std::vector<std::string> parameter_names, std::vector<double> parameter_values,
    std::array<double,7> & linedata, int & index_chem);

#endif