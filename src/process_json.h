

#ifndef PROCESS_JSONH_INCLUDED
#define PROCESS_JSONH_INCLUDED

#include <string>
#include <fstream>
#include <iostream>
#include "jnlohmann/json.h"
#include "globalgeneral.h"

using json = nlohmann::json;

void read_json_infosize(const std::string& configjson_file);


#endif