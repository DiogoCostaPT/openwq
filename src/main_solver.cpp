
#include "main_solver.h"

// main solver
void main_solver(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar){


    int numspec;
    int icmpMob, tmpst_i;
    bool mobile;
    arma::cube wfluxC_x,wfluxC_y,wfluxC_z,wmassC;
    std::vector<std::vector<std::string>> fluxes_filenames;
    std::vector<std::vector<double>> fluxes_filenames_num;
    std::string fluxes_filenamesExtention;
    std::vector<int> mobileCompt;
    std::string res_folder = JSONfiles.Master["export_results_folder"];
    
    int numcmp = JSONfiles.H2O["compartments"].size();
    double disp_x = JSONfiles.WQ["dispersion"]["x-dir"];
    double disp_y = JSONfiles.WQ["dispersion"]["y-dir"];
    double disp_z = JSONfiles.WQ["dispersion"]["z-dir"];

    // Get fluxes files for each compartment
    GetFluxesFiles(JSONfiles,fluxes_filenames);

    // Convert filename strings to numbers
    ConvertSortFluxesFilenames2Double(JSONfiles,fluxes_filenames,fluxes_filenames_num);

    // Check if time steps match between mobile compartments
    bool timeMatch_flag = CheckIfCompTimeStepsMatch(fluxes_filenames_num,mobileCompt);

    // Save initial conditions
    for (int j=0;j<numcmp;j++){
        writeVTU(JSONfiles,j,Prj_StateVar,0); 
    }
    
    // Get Extention of Fluxes files: use 1st mobile compartment as reference and [3]-> because the first 2 are allways ".' and "..""
    if (!mobileCompt.empty()){ // if at least one compartment is mobile

        GetFileExtension(fluxes_filenames[mobileCompt[0]][2],fluxes_filenamesExtention);

        // if all mobile_compartment timesteps match, run ADE_solver
        if (timeMatch_flag){

            int tmpst_num = fluxes_filenames_num[mobileCompt[0]].size(); // num of elements of the 1st mobile compartment
                        
            for (int tmpst=0;tmpst<tmpst_num;tmpst++){ // time loop
                
                tmpst_i = fluxes_filenames_num[mobileCompt[0]][tmpst]; // timestep in file

                // Perform chemical transformations
                ChemTransf(JSONfiles,Prj_StateVar);
                
                readSetFluxes(JSONfiles,Prj_StateVar,mobileCompt,fluxes_filenamesExtention,tmpst_i); // Get all fluxes at timestep tmpst
                
                // Solve transport -> call ADE_solver
                for (int icmp=0;icmp<mobileCompt.size();icmp++){ // comparment loop
                    
                    icmpMob = mobileCompt[icmp]; // get mobile compartments
                    
                    std::vector<std::string> chemspec_i = JSONfiles.WQ["compartments"][std::to_string(icmpMob+1)]["chem_species"];
                    numspec = chemspec_i.size();
                    wfluxC_x = (*Prj_StateVar.wflux)(icmpMob)(0);
                    wfluxC_y = (*Prj_StateVar.wflux)(icmpMob)(1);
                    wfluxC_z = (*Prj_StateVar.wflux)(icmpMob)(2);
                    wmassC = (*Prj_StateVar.wmass)(icmpMob);
                
                    for (int ichem=0;ichem<numspec;ichem++){

                        ADE_solver(JSONfiles,Prj_StateVar,icmpMob,ichem,wfluxC_x,wfluxC_y,wfluxC_z,wmassC);                 
                        
                    }
                }

                // Print Results
            for (int j=0;j<numcmp;j++){
                writeVTU(JSONfiles,j,Prj_StateVar,tmpst_i); // https://lorensen.github.io/VTKExamples/site/Cxx/IO/WriteVTU/
            }

            }
        }
    }

    
}