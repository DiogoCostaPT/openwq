
#include "readSetIC.h"

// Initial conditions (water and chemical mass)
void readSetIC(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar){

    int numcmp = JSONfiles.H2O["compartments"].size();
    int numspec,chem_ii;
    std::string filepath;
    
    for (int i=0;i<numcmp;i++){

        // wmass
        read_file_3Dcoldata(JSONfiles.H2O[std::to_string(i+1)]["IC_file"],
            (*Prj_StateVar.wmass)(i),JSONfiles.H2O[std::to_string(i+1)]["IC_file"]["var_col"],
            JSONfiles.H2O[std::to_string(i+1)]["IC_file"]["file_path"]);
        
        // chemass
        numspec = JSONfiles.BGC["compartments"][std::to_string(i+1)]["chem_species"].size();
        std::vector<int> chemname_nums = 
            JSONfiles.BGC["compartments"][std::to_string(i+1)]["chem_species"]; //chem species # within compartment icmp (from JSON.BGQ)
        
        for (int j=0;j<numspec;j++){
            chem_ii = chemname_nums[j];
            read_file_3Dcoldata(JSONfiles.BGC["compartments"][std::to_string(i+1)]
                [std::to_string(chem_ii)]["IC_file"],(*Prj_StateVar.chemass)(i)(j),
                JSONfiles.BGC["compartments"][std::to_string(i+1)][std::to_string(chem_ii)]["IC_file"]["var_col"],
                JSONfiles.BGC["compartments"][std::to_string(i+1)][std::to_string(chem_ii)]["IC_file"]["file_path"]);
        }
    }
}