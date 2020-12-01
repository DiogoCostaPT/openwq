
#include "ChemTransf.h"

// Compute chemical transformations
void ChemTransf(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, int icmp){

    std::string expression_string; // expression string
    
    // Get chem transformations
    int num_transf = DEMOS_OpenWQ_json.BGC["list_transformations"].size();

    // Looping over transformations
    for (int transi=0;transi<num_transf;transi++){
 
        Transf(DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars, icmp,transi); // calling exprtk: parsing expression
        
    }
}

// Compute each chemical transformation
void Transf(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json,DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, int icmp, int transi){


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
void ChemCompExchange(DEMOS_OpenWQ_json& DEMOS_OpenWQ_json, DEMOS_OpenWQ_vars& DEMOS_OpenWQ_vars, int source, std::string kinetics, 
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


