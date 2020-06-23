
#include "readSetFluxes.h"

// Read and set fluxes for all compartments
void readSetFluxes(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar,
    std::vector<int> mobileCompt,std::string& fluxes_filenamesExtention){

    int icmpMobile;
    int num_mobileCompt = mobileCompt.size(); // num of mobile compartments

    // loop over compartmennts: compare all mobile_compartments with the first compartment
    for (int icmp=0;icmp<num_mobileCompt;icmp++){ 

        icmpMobile = mobileCompt[icmp];

        // loop over x-, y- and z-directions
        for (int xyz_i=0;xyz_i<3;xyz_i++){ 

            //read_file_3Dcoldata(JSONfiles.H2O[std::to_string(icmpMobile+1)]["water_fluxes_files"],
            //    (*Prj_StateVar.wflux)(icmpMobile+1)(xyz_i),
            //    icols[j],file_path_i);
        }
    }

}
