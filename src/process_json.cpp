// Process config json file

#include "process_json.h"

void jsonconfig_dim(const std::string& configjson_file)
{

    std::string str, msg, count_str;
    bool cmpt_on,dim_on,chem_on;

    unsigned count = 0,
             cmt_num = 0, 
             n_x = 0,
             n_y = 0,
             n_z = 0,
             chem_num = 0;
    
    std::ifstream file(configjson_file);
    
    int i = 0;
    cmpt_on = false;
    while (std::getline(file, str)) 
    {

         if(str.find("compartments") != std::string::npos){
             cmpt_on = true;
             dim_on = false;
             chem_on = false;
             count = 0;
             continue;
             }; 
         if(str.find("dimensions") != std::string::npos){
             cmpt_on = false;
             dim_on = true;
             chem_on = false;
             count = 0;
             continue;
             }; 
         if(str.find("chemical_species") != std::string::npos){
             cmpt_on = false;
             dim_on = false;
             chem_on = true;
             count = 0;
             continue;
             }; 
         
         if (cmpt_on){
             count++;
             count_str = '"' + std::to_string(count) +'"';
             if(str.find(count_str) != std::string::npos){cmt_num++;continue;};
         }

    }

}