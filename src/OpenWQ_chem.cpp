
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

#include "OpenWQ_chem.h"


// Compute chemical transformations
void OpenWQ_chem::Run(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig){
    
    // Local variable for num_HydroComp
    int num_hydrocmp = OpenWQ_hostModelconfig.HydroComp.size();

    // Loop over number of compartments
    for (int icmp=0;icmp<num_hydrocmp;icmp++){
    
        BGC_Transform( // calls exprtk: parsing expression
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            icmp); 

    }
}


// Compute each chemical transformation
void OpenWQ_chem::BGC_Transform(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    int icmp){
    
    // Local variables for expression evaluator: exprtk
    std::string expression_string; // expression string
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;

    // Other local variables
    std::string chemname;
    int index_cons,index_prod;
    int nx, ny, nz, index_i;
    double param_val;
    std::vector<int> index_transf;
    //double chemass_consumed, chemass_produced;
    std::vector<double> chemass_transf;
    
    // Find compartment index in openWQ data structures
    // Get number transformations
    int num_transf = OpenWQ_json.BGCcycling["list_transformations"].size();

    /* ########################################
    // Get cycling_frameworks for compartment icomp
    ######################################## */

    // Get compartment names in openW_config
    std::vector<std::string> CompNamesWithBGCcyling = 
            OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"]; 

    // Find location (loc_comp) of compartment icmp in CompNamesWithBGCcyling (from openWQ_config)
    std::string openWQ_CompName_icmp = std::get<1>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // Compartment icomp name
    std::string json_CompName_n;
    int loc_comp; // location of desired compartment

    for (int n=0;n<CompNamesWithBGCcyling.size();n++){
        
        // Get compartment name n in json file
        json_CompName_n = CompNamesWithBGCcyling.at(loc_comp); 

        // Convert to lower case to match internal description
        std::transform(json_CompName_n.begin(), json_CompName_n.end(), json_CompName_n.begin(),
            [](unsigned char c){ return std::tolower(c); }); // need to iteract through each character        

        if (json_CompName_n.compare(openWQ_CompName_icmp) != 0){ 
                loc_comp = n;
            break;
        }
    }

    // Get cycling frameworks names for compartment icomp
    std::vector<std::string> cyclingFram_icomp = 
            OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"]
                [CompNamesWithBGCcyling.at(loc_comp)]["cycling_framework"]; 


    /* ########################################
    // Loop over biogeochemical cycling frameworks
    ######################################## */

    // Get number of BGC frameworks in comparment icmp
    int num_bgcFramrk = cyclingFram_icomp.size();
    std::string cyclingFrame_i;
    
    for (int bgcfrmi=0;bgcfrmi<num_bgcFramrk;bgcfrmi++){

        // Get framework name i of compartment icmp
        cyclingFrame_i = cyclingFram_icomp.at(bgcfrmi);

        /* ########################################
        // Looping over transformations
        ######################################## */
        for (int transi=0;transi<num_transf;transi++){

            // Get transformation transi info
            std::string consumed_spec =  OpenWQ_json.BGCcycling["CYCLING_FRAMEWORKS"][cyclingFrame_i]
                [std::to_string(transi+1)]["consumed"];
            std::string produced_spec =  OpenWQ_json.BGCcycling["CYCLING_FRAMEWORKS"][cyclingFrame_i]
                [std::to_string(transi+1)]["produced"];
            std::string expression_string = OpenWQ_json.BGCcycling["CYCLING_FRAMEWORKS"][cyclingFrame_i]
                [std::to_string(transi+1)]["kinetics"];
            std::vector<std::string> parameter_names = OpenWQ_json.BGCcycling["CYCLING_FRAMEWORKS"][cyclingFrame_i]
                [std::to_string(transi+1)]["parameter_names"];
            std::string expression_string_modif = expression_string;
            
            // Get chemical species in compartment
            std::vector<std::string> chem_species = OpenWQ_json.BGCcycling["CHEMICAL_SPECIES"]["list"];
            
            // Find species indexes: consumed, produced and in the expression
            for(int chemi=0;chemi<chem_species.size();chemi++){
                
                chemname = chem_species[chemi];

                // Consumedchemass_consumed, chemass_produced;ty()) 
                index_i = expression_string.find(chemname);
                if (index_i!=-1 && !consumed_spec.empty()){
                    index_cons = chemi; // index
                }

                // Produced
                index_i = chemname.find(produced_spec);
                if (index_i!=-1 && !produced_spec.empty()){
                    index_prod = chemi; // index
                }

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
                param_val = OpenWQ_json.BGCcycling[std::to_string(transi+1)]["parameter_values"][parameter_names[i]];
                expression_string_modif.replace(index_i,index_i + parameter_names[i].size(),std::to_string(param_val));
            }

            // Add variables to symbol_table
            symbol_table_t symbol_table;
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
            nx = std::get<2>(OpenWQ_hostModelconfig.HydroComp.at(icmp));
            ny = std::get<3>(OpenWQ_hostModelconfig.HydroComp.at(icmp));
            nz = std::get<4>(OpenWQ_hostModelconfig.HydroComp.at(icmp));

            for (int ix=0;ix<nx;ix++){
                for (int iy=0;iy<ny;iy++){
                    for (int iz=0;iz<nz;iz++){
                        
                        // loop to get all the variables inside the expression
                        for (int chem=0;chem<chemass_transf.size();chem++){
                            chemass_transf[chem] = (*OpenWQ_vars.chemass)(icmp)(index_transf[chem])(ix,iy,iz);
                        }

                        // mass transfered
                        double transf_mass = expression.value(); 

                        // mass consumed
                        (*OpenWQ_vars.chemass)(icmp)(index_cons)(ix,iy,iz) -= transf_mass;

                        // mass prod
                        (*OpenWQ_vars.chemass)(icmp)(index_prod)(ix,iy,iz) += transf_mass;

                    }
                }
            }
        }
    }

}