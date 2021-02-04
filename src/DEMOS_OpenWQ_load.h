
#ifndef DEMOS_OPENWQ_LOADH_INCLUDED
#define DEMOS_OPENWQ_LOADH_INCLUDED

#include <armadillo>
#include "string.h"
#include <algorithm>

#include "exprtk.hpp"

#include <cstdio>

#include "jnlohmann/json.h"
using json = nlohmann::json;

//#include "utility.h"

#include "DEMOS_OpenWQ_global.h"

class DEMOS_OpenWQ_load{

    public:

    void loadinit(
        DEMOS_OpenWQ_json& DEMOS_OpenWQ_json);

    void read_JSON_2class(
        json& jsondata,
        const std::string& jsonfile);

};

#endif

