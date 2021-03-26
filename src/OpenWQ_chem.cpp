
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


/* #################################################
// Parse biogeochemical expressions
################################################# */
void OpenWQ_chem::setBGCexpressions(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_vars& OpenWQ_vars){

    // Local variables for expression evaluator: exprtk
    std::string 
        chemname,                   // chemical name
        consumed_spec,
        produced_spec,
        expression_string,          // expression string
        expression_string_modif;    // expression string replaced by variable in code
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;
    
    // Other local variables
    typedef std::tuple<
        std::string,                    // Biogeochemical cycle name
        std::string,                    // Transformation name
        std::string,                    // kinetic equation provided
        unsigned int,                   // index of consumed species       
        unsigned int,                   // index of produced species
        std::vector<unsigned int>       // index of chemical in transformation equation (needs to be here for loop reset)
        > BGCTransfTuple_info;          // Tuple with info and expression for BGC cyling
    unsigned int num_BGCcycles, 
        num_transf,
        index_cons,index_prod,           // indexed for consumed and produced chemical
        index_new_chemass_InTransfEq;    // interactive index to build index_new_chemass_InTransfEq
    int index_i;                         // iteractive index (can be zero because it is determined in a .find())
    std::vector<std::string> BGCcycles_namelist;
    std::string BGCcycles_name, Transf_name;
    double param_val; // prameter value


    // Number of BCG cycles defined    
    num_BGCcycles = OpenWQ_json.BGCcycling["CYCLING_FRAMEWORKS"].size();

    // Get all biogeochemical cycling names
    for (auto it: OpenWQ_json.BGCcycling["CYCLING_FRAMEWORKS"].items())
    {
        BGCcycles_namelist.push_back(it.key());
    }

    /* ########################################
    // Loop over biogeochemical cycling frameworks
    ######################################## */
    for (unsigned int bgci=0;bgci<num_BGCcycles;bgci++){

        /* ########################################
        // Loop over transformations in biogeochemical cycle bgci
        ######################################## */

        // Get BGC cycle name
        BGCcycles_name = BGCcycles_namelist[bgci];

        // Get number of transformations inside BGCcycles_name
        num_transf = OpenWQ_json.BGCcycling
            ["CYCLING_FRAMEWORKS"]
            [BGCcycles_name]
            ["LIST_TRANSFORMATIONS"].size();
        
        for (unsigned int transi=0;transi<num_transf;transi++){

            // Get Transformation name
            Transf_name = OpenWQ_json.BGCcycling
                ["CYCLING_FRAMEWORKS"]
                [BGCcycles_name]
                ["LIST_TRANSFORMATIONS"]
                [std::to_string(transi+1)];

            std::vector<unsigned int> index_transf; // index of chemical in transformation equation (needs to be here for loop reset)

            // Get transformation transi info
            consumed_spec =  OpenWQ_json.BGCcycling
                ["CYCLING_FRAMEWORKS"]
                [BGCcycles_name]
                [std::to_string(transi+1)]
                ["CONSUMED"];
            produced_spec =  OpenWQ_json.BGCcycling
                ["CYCLING_FRAMEWORKS"]
                [BGCcycles_name]
                [std::to_string(transi+1)]
                ["PRODUCED"];
            expression_string = OpenWQ_json.BGCcycling
                ["CYCLING_FRAMEWORKS"]
                [BGCcycles_name]
                [std::to_string(transi+1)]
                ["KINETICS"];
            std::vector<std::string> parameter_names = OpenWQ_json.BGCcycling
                ["CYCLING_FRAMEWORKS"]
                [BGCcycles_name]
                [std::to_string(transi+1)]
                ["PARAMETER_NAMES"];
            expression_string_modif = expression_string;
            
            /* ########################################
            // Find species indexes: consumed, produced and in the expression
            ######################################## */
            // these indexes need to be reset here
            index_cons = -1;
            index_prod = -1; // indexed for consumed and produced chemical
            index_new_chemass_InTransfEq = 0;

            for(unsigned int chemi=0;chemi<(OpenWQ_wqconfig.num_chem);chemi++){
                
                // Get chemical species name
                chemname = (OpenWQ_wqconfig.chem_species_list)[chemi];

                // Consumedchemass_consumed, chemass_produced;ty()) 
                index_i = consumed_spec.find(chemname);
                if (index_i!=-1 && !consumed_spec.empty()){
                    index_cons = chemi; // index
                }

                // Produced
                index_i = produced_spec.find(chemname);
                if (index_i!=-1 && !produced_spec.empty()){
                    index_prod = chemi; // index
                }

                // In expression (replace chemical species name by index)
                index_i = expression_string_modif.find(chemname);
                if (index_i!=-1 && !expression_string_modif.empty()){
                    index_transf.push_back(chemi); // index
                    expression_string_modif.replace(
                        index_i,    // start position
                        chemname.size(),    // length
                        "chemass_InTransfEq["+std::to_string(index_new_chemass_InTransfEq)+"]"); // string to replace by
                    index_new_chemass_InTransfEq ++;    
                }
            }

            // Replace parameter name by value in expression (future: allow for variable e.g., soil moisture)
            for (unsigned int i=0;i<parameter_names.size();i++){
                index_i = expression_string_modif.find(parameter_names[i]);
                param_val = OpenWQ_json.BGCcycling
                    ["CYCLING_FRAMEWORKS"]
                    [BGCcycles_name]
                    [std::to_string(transi+1)]
                    ["PARAMETER_VALUES"]
                    [parameter_names[i]];
                expression_string_modif.replace(
                    index_i,
                    parameter_names[i].size(),
                    std::to_string(param_val));
            }

            // Add variables to symbol_table
            symbol_table_t symbol_table;
            
            OpenWQ_wqconfig.chemass_InTransfEq.clear();
            for (unsigned int i=0;i<index_transf.size();i++){
                OpenWQ_wqconfig.chemass_InTransfEq.push_back(0); // creating the vector
            }
            symbol_table.add_vector("chemass_InTransfEq",OpenWQ_wqconfig.chemass_InTransfEq);
            // symbol_table.add_constants();

            // Create Object
            expression_t expression;
            expression.register_symbol_table(symbol_table);

            // Parse expression and compile 
            parser_t parser;
            parser.compile(expression_string_modif,expression);

            // Save expressions in BGCexpressions_info and BGCexpressions_eq
            OpenWQ_wqconfig.BGCexpressions_info.push_back(
                BGCTransfTuple_info(
                    BGCcycles_name,
                    Transf_name,
                    expression_string,
                    index_cons,
                    index_prod,
                    index_transf));
            OpenWQ_wqconfig.BGCexpressions_eq.push_back(expression);

        }
    }
}


/* #################################################
// Compute chemical transformations
################################################# */
void OpenWQ_chem::Run(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig){
    
    // Local variable for num_HydroComp
   unsigned int num_hydrocmp = OpenWQ_hostModelconfig.HydroComp.size();

    // Loop over number of compartments
    for (unsigned int icmp=0;icmp<num_hydrocmp;icmp++){
    
        BGC_Transform( // calls exprtk: parsing expression
            OpenWQ_json,
            OpenWQ_vars,
            OpenWQ_hostModelconfig,
            OpenWQ_wqconfig,
            icmp); 

    }
}

/* #################################################
// Compute each chemical transformation
################################################# */
void OpenWQ_chem::BGC_Transform(
    OpenWQ_json& OpenWQ_json,
    OpenWQ_vars& OpenWQ_vars,
    OpenWQ_hostModelconfig& OpenWQ_hostModelconfig,
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    unsigned int icmp){
    
    // Local variables for expression evaluator: exprtk
    std::string expression_string; // expression string

    // Other local variables
    std::string chemname; // chemical name
    unsigned int index_cons,index_prod; // indexed for consumed and produced chemical
    unsigned int num_transf; // number of transformation in biogeochemical cycle bgci
    unsigned int num_BGCcycles; // number of BGC frameworks in comparment icmp
    unsigned int nx = std::get<2>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // number of cell in x-direction
    unsigned int ny = std::get<3>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // number of cell in y-direction
    unsigned int nz = std::get<4>(OpenWQ_hostModelconfig.HydroComp.at(icmp)); // number of cell in z-direction
    std::string BGCcycles_name_icmp,BGCcycles_name_list; // BGC Cycling name i of compartment icmp  
    double transf_mass;         // interactive mass to transfer between species

    // Find compartment icmp name from code (host hydrological model)
    std::string CompName_icmp = std::get<1>(
        OpenWQ_hostModelconfig.HydroComp.at(icmp)); // Compartment icomp name

    // Get cycling_frameworks for compartment icomp (name = CompName_icmp) 
    // (compartment names need to match)
    std::vector<std::string> BGCcycles_icmp = 
        OpenWQ_json.Config["BIOGEOCHEMISTRY_CONFIGURATION"]
            [CompName_icmp]["CYCLING_FRAMEWORK"];

    // Get number of BGC frameworks in comparment icmp
    num_BGCcycles = BGCcycles_icmp.size();
        

    /* ########################################
    // Loop over biogeochemical cycling frameworks
    ######################################## */

    for (unsigned int bgci=0;bgci<num_BGCcycles;bgci++){

        // Get framework name i of compartment icmp
        BGCcycles_name_icmp = BGCcycles_icmp.at(bgci);

        // Get number transformations in biogeochemical cycle BGCcycles_name_icmp
        std::vector<unsigned int> transf_index;
        for (unsigned int index_j=0;index_j<OpenWQ_wqconfig.BGCexpressions_info.size();index_j++){
            BGCcycles_name_list = std::get<0>(OpenWQ_wqconfig.BGCexpressions_info.at(index_j)); // get BGC cycle from OpenWQ_wqconfig.BGCexpressions_info
            if(BGCcycles_name_list.compare(BGCcycles_name_icmp) == 0){
                transf_index.push_back(index_j);
            }
        }
        
        num_transf = transf_index.size(); // number of transformation for this BGC cycle (as identified in BGCexpressions_info)

        /* ########################################
        // Loop over transformations in biogeochemical cycle bgci
        ######################################## */
        
        for (unsigned int transi=0;transi<num_transf;transi++){

            index_cons = std::get<3>(OpenWQ_wqconfig.BGCexpressions_info.at(transf_index[transi]));
            index_prod = std::get<4>(OpenWQ_wqconfig.BGCexpressions_info.at(transf_index[transi]));

            // Get indexes of chemicals in transformation equation (needs to be here for loop reset)
            std::vector<unsigned int> index_chemtransf = 
                std::get<5>(
                    OpenWQ_wqconfig.BGCexpressions_info.at(transf_index[transi]));

            /* ########################################
            // Loop over space: nx, ny, nz
            ######################################## */
            for (unsigned int ix=0;ix<nx;ix++){
                for (unsigned int iy=0;iy<ny;iy++){
                    for (unsigned int iz=0;iz<nz;iz++){                    
                        
                        //std::vector<double> chemass_InTransfEq; // chemical mass involved in transformation (needs to be here for loop reset)
                        OpenWQ_wqconfig.chemass_InTransfEq.clear();
                        // loop to get all the variables inside the expression
                        for (unsigned int chem=0;chem<index_chemtransf.size();chem++){
                            OpenWQ_wqconfig.chemass_InTransfEq.push_back(
                                (*OpenWQ_vars.chemass)
                                (icmp)
                                (index_chemtransf[chem])
                                (ix,iy,iz));
                        }

                        // Mass transfered: Consumed -> Produced (using exprtk)
                        transf_mass = OpenWQ_wqconfig.BGCexpressions_eq[transf_index[transi]].value(); 

                        // New mass of consumed chemical
                        (*OpenWQ_vars.chemass)(icmp)(index_cons)(ix,iy,iz) -= transf_mass;

                        // New mass of produced chemical
                        (*OpenWQ_vars.chemass)(icmp)(index_prod)(ix,iy,iz) += transf_mass;

                    }
                }
            }
        }
    }

}