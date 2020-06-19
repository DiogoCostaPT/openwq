
#include "readSetFluxes.h"

// Read and assign fluxes: wflux
void readSetFluxes(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar){
    
    int numcmp = JSONfiles.H2O["compartments"].size();
    bool mobile;
    bool multiple_files;
    std::string file_path_i;

    for (int i=0;i<numcmp;i++){

        mobile = JSONfiles.H2O[std::to_string(i+1)]["mobile"];
        if (mobile){
            std::vector<int> icols = JSONfiles.H2O[std::to_string(i+1)]["water_fluxes_files"]["var_col"];
            for (int j=0;j<3;j++){

                multiple_files = JSONfiles.H2O[std::to_string(i+1)]["water_fluxes_files"]["multiple_files"];

                if (multiple_files){
                    //file_path_i
                }else{
                    file_path_i = JSONfiles.H2O[std::to_string(i+1)]["water_fluxes_files"]["file_path_IFNOT_multiplefiles"];
                }
                read_file_3Dcoldata(JSONfiles.H2O[std::to_string(i+1)]["water_fluxes_files"],
                    (*Prj_StateVar.wflux)(i)(j),
                    icols[j],file_path_i);
            }
        }   
    }
}