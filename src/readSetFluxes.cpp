
#include "readSetFluxes.h"

// Read and set intra fluxes for all compartments
void readSetFluxes(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar,std::vector<int> mobileCompt,
    std::string& filenamesExtention,int tmpst){

    int icmpMobile;
    int num_mobileCompt = mobileCompt.size(); // num of mobile compartments

    // loop over compartmennts: compare all mobile_compartments with the first compartment
    for (int icmp=0;icmp<num_mobileCompt;icmp++){ 

        icmpMobile = mobileCompt[icmp]; // mobile compartment i

        // fluxes file for timestep tmpst
        std::string filepath_i = JSONfiles.H2O[std::to_string(icmpMobile+1)]["water_fluxes_files"]["folder_path"];
        filepath_i.append("/");
        filepath_i.append(std::to_string(tmpst)); // timestep number
        filepath_i.append(filenamesExtention); // file extention

        // Get grid col locations
        std::vector<int> var_col = JSONfiles.H2O[std::to_string(icmpMobile+1)]["water_fluxes_files"]["var_col"];
        
        // loop over x-, y- and z-directions
        for (int xyz_i=0;xyz_i<3;xyz_i++){

            if (var_col[xyz_i] == 0) continue; // skip if var_col[xyz_i] == false 

                read_file_3Dcoldata(JSONfiles.H2O[std::to_string(icmpMobile+1)]["water_fluxes_files"],
                    (*Prj_StateVar.wflux_intra)(icmpMobile)(xyz_i),
                    var_col[xyz_i], filepath_i);
        }
    }

}

// Read and set inter fluxes for all compartments
void readCompInteract(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, std::vector<int> mobileCompt,
    std::string& filenamesExtention,int tmpst){

    int icmpMobile;
    int num_mobileCompt = mobileCompt.size(); // num of mobile compartments

    // loop over compartmennts: compare all mobile_compartments with the first compartment
    for (int icmp=0;icmp<num_mobileCompt;icmp++){ 

        icmpMobile = mobileCompt[icmp]; // mobile compartment i

        // fluxes file for timestep tmpst
        std::string filepath_i = JSONfiles.CMPI[std::to_string(icmpMobile+1)]["mapping_file"]["folder_path"];
        filepath_i.append("/");
        filepath_i.append(std::to_string(tmpst)); // timestep number
        filepath_i.append(filenamesExtention); // file extention

        // Get grid col locations
        std::vector<int> grid_col_send = JSONfiles.CMPI[std::to_string(icmpMobile+1)]["mapping_file"]["grid_col_send"];
        std::vector<int> grid_col_receive = JSONfiles.CMPI[std::to_string(icmpMobile+1)]["mapping_file"]["grid_col_receive"];

        // loop over x-, y- and z-directions
        for (int xyz_i=0;xyz_i<3;xyz_i++){

            if (grid_col_send[xyz_i] == 0) continue; // skip if var_col[xyz_i] == false 

                read_file_3Dcoldata(JSONfiles.H2O[std::to_string(icmpMobile+1)]["water_fluxes_files"],
                    (*Prj_StateVar.wflux_intra)(icmpMobile)(xyz_i),
                    grid_col_send[xyz_i], filepath_i);
        }
    }

}
