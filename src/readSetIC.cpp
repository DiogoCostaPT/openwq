
#include "readSetIC.h"

// Initial conditions (water and chemical mass)
void readSetIC(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar){

    int numcmp = JSONfiles.H2O["compartments"].size();
    int numspec;
    std::string filepath;
    
    for (int i=0;i<numcmp;i++){

        // wmass
        read_file_3Dcoldata(JSONfiles.H2O[std::to_string(i+1)]["IC_file"],
            (*Prj_StateVar.wmass)(i),JSONfiles.H2O[std::to_string(i+1)]["IC_file"]["var_col"],
            JSONfiles.H2O[std::to_string(i+1)]["IC_file"]["file_path"]);
        
        // chemass
        numspec = JSONfiles.WQ["compartments"][std::to_string(i+1)]["chem_species"].size();
        std::vector<std::string> chemnames = 
            JSONfiles.WQ["compartments"][std::to_string(i+1)]["chem_species"]; //chem species # within compartment icmp (from JSON.WQ)
        
        for (int chem_i=0;chem_i<numspec;chem_i++){
            read_file_3Dcoldata(JSONfiles.WQ["compartments"][std::to_string(i+1)][std::to_string(chem_i+1)]["IC_file"],(*Prj_StateVar.chemass)(i)(chem_i),
                JSONfiles.WQ["compartments"][std::to_string(i+1)][std::to_string(chem_i+1)]["IC_file"]["var_col"],
                JSONfiles.WQ["compartments"][std::to_string(i+1)][std::to_string(chem_i+1)]["IC_file"]["file_path"]);
        }
    }
}