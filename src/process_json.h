

#ifndef PROCESS_JSONH_INCLUDED
#define PROCESS_JSONH_INCLUDED

#include <string>
#include <fstream>
#include <iostream>
#include "jnlohmann/json.h"
#include "global.h"

using json = nlohmann::json;

void read_json_configfile(const std::string& configjson_file);


#endif