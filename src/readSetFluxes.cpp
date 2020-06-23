
#include "readSetFluxes.h"

// Read and set fluxes for all compartments
void readSetFluxes(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar,
    std::vector<int> mobileCompt,std::string& fluxes_filenamesExtention,
    int tmpst){

    int icmpMobile;
    int num_mobileCompt = mobileCompt.size(); // num of mobile compartments

    // loop over compartmennts: compare all mobile_compartments with the first compartment
    for (int icmp=0;icmp<num_mobileCompt;icmp++){ 

        icmpMobile = mobileCompt[icmp]; // mobile compartment i

        // fluxes file for timestep tmpst
        std::string filepath_i = JSONfiles.H2O[std::to_string(icmpMobile+1)]["water_fluxes_files"]["folder_path"];
        filepath_i.append("/");
        filepath_i.append(std::to_string(tmpst)); // timestep number
        filepath_i.append(fluxes_filenamesExtention); // file extention

        // Get grid col locations
        std::vector<int> var_col = JSONfiles.H2O[std::to_string(icmpMobile+1)]["water_fluxes_files"]["var_col"];
        
        // loop over x-, y- and z-directions
        for (int xyz_i=0;xyz_i<3;xyz_i++){

            if (var_col[xyz_i] == 0) continue; // skip if var_col[xyz_i] == false 

                read_file_3Dcoldata(JSONfiles.H2O[std::to_string(icmpMobile+1)]["water_fluxes_files"],
                    (*Prj_StateVar.wflux)(icmpMobile+1)(xyz_i),
                    var_col[xyz_i], filepath_i);
        }
    }

}
