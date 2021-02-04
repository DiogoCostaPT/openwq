
#ifndef OPENWQ_LOADH_INCLUDED
#define OPENWQ_LOADH_INCLUDED

#include <armadillo>
#include "string.h"
#include <algorithm>

#include "exprtk.hpp"

#include <cstdio>

#include "jnlohmann/json.h"
using json = nlohmann::json;

//#include "utility.h"

#include "OpenWQ_global.h"

class OpenWQ_load{

    public:

    void loadinit(
        OpenWQ_json& OpenWQ_json);

    void read_JSON_2class(
        json& jsondata,
        const std::string& jsonfile);

};

#endif

