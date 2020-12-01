
#include "main_solver.h"

// main solver
void main_solver(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars){


    int numspec;
    int icmpMob, tmpst_i;
    bool mobile;
    std::vector<std::vector<std::string>> fluxes_filenames, compFluxInt_filenames;
    std::vector<std::vector<double>> fluxes_filenames_num,compFluxInt_filenames_num,all_filenames_num;
    std::string fluxes_fileExtention,compFluxInt_fileExtention;
    std::vector<int> mobileCompt;
    std::string res_folder = DEMOS_OpenWQ_json.Master["export_results_folder"];
    std::vector<int>::iterator is_mobile; // to check if mobile in compartment loop
    
    int numcmp = DEMOS_OpenWQ_json.H2O["compartments"].size();
    double disp_x = DEMOS_OpenWQ_json.WQ["dispersion"]["x-dir"];
    double disp_y = DEMOS_OpenWQ_json.WQ["dispersion"]["y-dir"];
    double disp_z = DEMOS_OpenWQ_json.WQ["dispersion"]["z-dir"];

    // Get fluxes files for each compartment
    GetFluxesFiles(DEMOS_OpenWQ_json,fluxes_filenames);
    
    // Get flux interaction files between compartments
    GetComptInteractFluxesFiles(DEMOS_OpenWQ_json,compFluxInt_filenames);

    // Convert filename strings to numbers: inter and intra fluxes
    ConvertSortFilenames2Double(DEMOS_OpenWQ_json.H2O["compartments"].size(),fluxes_filenames,fluxes_filenames_num);
    ConvertSortFilenames2Double(DEMOS_OpenWQ_json.CMPI["interactions"].size(),compFluxInt_filenames,compFluxInt_filenames_num);

    // Check if time steps match for intra and inter compartment fluxes files
    all_filenames_num = fluxes_filenames_num;
    for (int i=0;i<compFluxInt_filenames_num.size();i++){
        all_filenames_num.push_back(compFluxInt_filenames_num[i]);
    }

    // Identify the mobile compartments
    IdentifyMobileCompt(fluxes_filenames_num,mobileCompt);

    // Check if all fluxes files (intra and inter) match
    bool timeMatch_flag = CheckIfCompTimeStepsMatch(all_filenames_num);

    // Save initial conditions
    for (int j=0;j<numcmp;j++){
        writeVTU(DEMOS_OpenWQ_json,j,DEMOS_OpenWQ_vars,0); 
    }
    
    // Run run ADE_solver and ADE_solver if all mobile_compartment timesteps match, 
    if (timeMatch_flag){

        // Get Extention of Fluxes files: use 1st mobile compartment as reference and [3]-> because the first 2 are allways ".' and "..""
        if (!mobileCompt.empty()){ // if at least one compartment is mobile

            GetFileExtension(fluxes_filenames[mobileCompt[0]][2],fluxes_fileExtention);
            GetFileExtension(compFluxInt_filenames[mobileCompt[0]][2],compFluxInt_fileExtention);
            
                int tmpst_num = fluxes_filenames_num[mobileCompt[0]].size(); // num of elements of the 1st mobile compartment
                            
                for (int tmpst=0;tmpst<tmpst_num;tmpst++){ // time loop
                    
                    tmpst_i = fluxes_filenames_num[mobileCompt[0]][tmpst]; // timestep in file
                    
                    // Exchange between compartments: wflux or chem_exchange
                    readCompInteract(DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars,compFluxInt_fileExtention,tmpst_i); // Get all fluxes at timestep tmpst

                    // Sinks and sources: chem only

                    
                    // ADE SOLVER (intra fluxes)
                    readSetFluxes(DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars,mobileCompt,fluxes_fileExtention,tmpst_i); // Get all fluxes at timestep tmpst

                    // Solve transport -> call ADE_solver
                    for (int icmp=0;icmp<numcmp;icmp++){ // comparment loop

                        // Perform chemical transformations
                        ChemTransf(DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars,icmp);
                        
                        is_mobile = find (mobileCompt.begin(), mobileCompt.end(), icmp);
                        if (is_mobile != mobileCompt.end()){// if mobile
                            // Run ADE_solver
                            icmp = mobileCompt[icmp]; // get mobile compartments
                            std::vector<std::string> chemspec_i = DEMOS_OpenWQ_json.WQ["compartments"][std::to_string(icmp+1)]["chem_species"];
                            numspec = chemspec_i.size();
                            for (int ichem=0;ichem<numspec;ichem++) 
                                ADE_solver_1(DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars,icmp,ichem);                                       
                        }
                    }

                    // Print Results
                for (int j=0;j<numcmp;j++){
                    writeVTU(DEMOS_OpenWQ_json,j,DEMOS_OpenWQ_vars,tmpst_i); // https://lorensen.github.io/VTKExamples/site/Cxx/IO/WriteVTU/
                }

            }
        }
    }

    
}