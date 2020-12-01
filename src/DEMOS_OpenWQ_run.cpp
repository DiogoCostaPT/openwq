

// Copyright 2020, Diogo Costa, diogo.pinhodacosta@canada.ca
// This file is part of OpenWQ model.

// This program, openWQ, is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) aNCOLS later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "DEMOS_OpenWQ_run.h"


// Compute chemical transformations
void DEMOS_OpenWQ_run::ChemTransf(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, int icmp){

    std::string expression_string; // expression string
    
    // Get chem transformations
    int num_transf = DEMOS_OpenWQ_json.BGC["list_transformations"].size();

    // Looping over transformations
    for (int transi=0;transi<num_transf;transi++){
 
        Transf(DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars, icmp,transi); // calling exprtk: parsing expression
        
    }
}

// Compute each chemical transformation
void DEMOS_OpenWQ_run::Transf(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, int icmp, int transi){


    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;

    std::string chemname;
    int index_cons, index_prod;
    int index_i;
    double param_val;
    std::vector<int> index_transf;
    //double chemass_consumed, chemass_produced;
    std::vector<double> chemass_transf;
    
    // Get transformation transi info
    std::string consumed_spec =  DEMOS_OpenWQ_json.BGC[std::to_string(transi+1)]["consumed"];
    std::string produced_spec =  DEMOS_OpenWQ_json.BGC[std::to_string(transi+1)]["produced"];
    std::string expression_string = DEMOS_OpenWQ_json.BGC[std::to_string(transi+1)]["kinetics"];
    std::vector<std::string> parameter_names = DEMOS_OpenWQ_json.BGC[std::to_string(transi+1)]["parameter_names"];
    std::string expression_string_modif = expression_string;
    
    // Get chemical species in compartment
    std::vector<std::string> chem_species = DEMOS_OpenWQ_json.WQ["compartments"][std::to_string(icmp+1)]["chem_species"];
     
    // Find species indexes: consumed, produced and in the expression
    for(int chemi=0;chemi<chem_species.size();chemi++){
        chemname = chem_species[chemi];

        // Consumedchemass_consumed, chemass_produced;ty()) 
        index_i = expression_string.find(chemname);
        if (index_i!=-1 && !consumed_spec.empty())
            index_cons = chemi; // index

        // Produced
        index_i = chemname.find(produced_spec);
        if (index_i!=-1 && !produced_spec.empty())
            index_prod = chemi; // index

        // In expression
        index_i = expression_string.find(chemname);
        int ii = 0;
        if (index_i!=-1 && !expression_string.empty()){
            index_transf.push_back(chemi); // index
            expression_string_modif.replace(index_i,index_i + chemname.size(),"chemass_transf["+std::to_string(ii)+"]");
            ii++;
        }

    }

    // Parmeters
    for (int i=0;i<parameter_names.size();i++){
        index_i = expression_string_modif.find(parameter_names[i]);
        param_val = DEMOS_OpenWQ_json.BGC[std::to_string(transi+1)]["parameter_values"][parameter_names[i]];
        expression_string_modif.replace(index_i,index_i + parameter_names[i].size(),std::to_string(param_val));
    }

    // Add variables to symbol_table
    symbol_table_t symbol_table;
    //symbol_table.add_variable(chem_species[index_cons],chemass_consumed);
    //symbol_table.add_variable(chem_species[index_prod],chemass_produced);
    for (int i=0;i<index_transf.size();i++){
        chemass_transf.push_back(0); // creating the vector
    }
    symbol_table.add_vector("chemass_transf",chemass_transf);
    // symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    parser.compile(expression_string_modif,expression);

    // Loop over space
    int nx = DEMOS_OpenWQ_json.H2O[std::to_string(icmp+1)]["nx"];
    int ny = DEMOS_OpenWQ_json.H2O[std::to_string(icmp+1)]["ny"];
    int nz = DEMOS_OpenWQ_json.H2O[std::to_string(icmp+1)]["nz"];

    for (int ix=0;ix<nx;ix++){
        for (int iy=0;iy<ny;iy++){
            for (int iz=0;iz<nz;iz++){
                
                // loop to get all the variables inside the expression
                for (int chem=0;chem<chemass_transf.size();chem++){
                    chemass_transf[chem] = (*DEMOS_OpenWQ_vars.chemass)(icmp)(index_transf[chem])(ix,iy,iz);
                }

                // mass transfered
                double transf_mass = expression.value(); 

                // mass consumed
                (*DEMOS_OpenWQ_vars.chemass)(icmp)(index_cons)(ix,iy,iz) -= transf_mass;

                // mass prod
                (*DEMOS_OpenWQ_vars.chemass)(icmp)(index_prod)(ix,iy,iz) += transf_mass;

            }
        }
     }

}

// Mass exchange between compartments
void DEMOS_OpenWQ_run::ChemCompExchange(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json, DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, int source, std::string kinetics, 
    std::vector<std::string> parameter_names, std::vector<double> parameter_values,
    std::array<double,7> & linedata, int & index_chem){

    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;

    double wmass_exchange;
    std::string kinetics_modif = kinetics;
    std::string chemname;
    int index_i;
    int index_transf;
    double chemass_transf;

    
    // Get chemical species in the source compartment
    std::vector<std::string> chem_species = DEMOS_OpenWQ_json.WQ["compartments"][std::to_string(source+1)]["chem_species"];

   // Find chem of relevance
    int ii = 0;
    for(int chemi=0;chemi<chem_species.size();chemi++){
        chemname = chem_species[chemi];

        // Consumedchemass_consumed, chemass_produced;ty()) 
        index_i = kinetics.find(chemname);
        if (index_i!=-1 && !kinetics.empty()){

            index_chem = chemi; // index
            kinetics_modif.replace(index_i,index_i + chemname.size(),"chemass_transf");

            break;
            }
        }

    // Parmeters
    for (int i=0;i<parameter_names.size();i++){
        index_i = kinetics_modif.find(parameter_names[i]);
        kinetics_modif.replace(index_i,index_i + parameter_names[i].size(),std::to_string(parameter_values[i]));
    }

    // Add variables to symbol_table
    symbol_table_t symbol_table;

    symbol_table.add_variable("chemass_transf",chemass_transf);
    // symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    parser.compile(kinetics_modif,expression);

    chemass_transf = (*DEMOS_OpenWQ_vars.chemass)(source)(index_chem)(linedata[0],linedata[1],linedata[2]);

    // mass transfered
    linedata[6] = expression.value(); 

}



// main loop
void DEMOS_OpenWQ_run::callrun(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json){

    
    // Create objects
    DEMOS_OpenWQ_vars DEMOS_OpenWQ_vars; 
    DEMOS_OpenWQ_print DEMOS_OpenWQ_print;


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
    DEMOS_OpenWQ_print.writeVTU(DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars,numcmp,0); 
    
    
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
                DEMOS_OpenWQ_print.writeVTU(DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars,numcmp,tmpst_i);

            }
        }
    }

    
}