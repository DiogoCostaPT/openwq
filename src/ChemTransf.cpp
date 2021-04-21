
#include "ChemTransf.h"

template <typename T>
void Transf(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, int icmp, int transi){

    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double>     expression_t;
    typedef exprtk::parser<double>             parser_t;

    std::string chemname;
    int index_cons, index_prod;
    int index_i;
    double param_val;
    std::vector<int> index_transf;
    //double chemass_consumed, chemass_produced;
    std::vector<double> chemass_transf;
    T m_a,m_b,m_c,m_d;
    
    // Get transformation transi info
    std::string consumed_spec =  JSONfiles.BGC[std::to_string(transi+1)]["consumed"];
    std::string produced_spec =  JSONfiles.BGC[std::to_string(transi+1)]["produced"];
    std::string expression_string = JSONfiles.BGC[std::to_string(transi+1)]["kinetics"];
    std::vector<std::string> parameter_names = JSONfiles.BGC[std::to_string(transi+1)]["parameter_names"];
    std::string expression_string_modif = expression_string;
    
    // Get chemical species in compartment
    std::vector<std::string> chem_species = JSONfiles.WQ["compartments"][std::to_string(icmp+1)]["chem_species"];
     
    // Find species indexes: consumed, produced and in the expression
    for(int chemi=0;chemi<chem_species.size();chemi++){
        chemname = chem_species[chemi];

        // Consumedchemass_consumed, chemass_produced;ty()) 
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
            //expression_string_modif.replace(index_i,index_i + chemname.size(),"chemass_transf["+std::to_string(ii)+"]");
            if (ii == 0) expression_string_modif.replace(index_i,index_i + chemname.size(),"m_a"); // up to 4 variables
            if (ii == 1) expression_string_modif.replace(index_i,index_i + chemname.size(),"m_b");
            if (ii == 2) expression_string_modif.replace(index_i,index_i + chemname.size(),"m_c");
            if (ii == 3) expression_string_modif.replace(index_i,index_i + chemname.size(),"m_d");
            ii++;
        }

    }

    // Parmeters
    for (int i=0;i<parameter_names.size();i++){
        index_i = expression_string_modif.find(parameter_names[i]);
        param_val = JSONfiles.BGC[std::to_string(transi+1)]["parameter_values"][parameter_names[i]];
        expression_string_modif.replace(index_i,index_i + parameter_names[i].size(),std::to_string(param_val));
    }

    // Add variables to symbol_table
    symbol_table_t symbol_table;
    //symbol_table.add_variable(chem_species[index_cons],chemass_consumed);
    //symbol_table.add_variable(chem_species[index_prod],chemass_produced);
    for (int i=0;i<index_transf.size();i++){
        chemass_transf.push_back(0); // creating the vector 
        if (i == 0) symbol_table.add_variable(chem_species[index_transf[i]], m_a); // up to 4 variables
        if (i == 1) symbol_table.add_variable(chem_species[index_transf[i]], m_b);
        if (i == 2) symbol_table.add_variable(chem_species[index_transf[i]], m_c);
        if (i == 3) symbol_table.add_variable(chem_species[index_transf[i]], m_d);
    }
    symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    parser.compile(expression_string_modif,expression);

    // Loop over space
    int nx = JSONfiles.H2O[std::to_string(icmp+1)]["nx"];
    int ny = JSONfiles.H2O[std::to_string(icmp+1)]["ny"];
    int nz = JSONfiles.H2O[std::to_string(icmp+1)]["nz"];
    double transf_mass;
    for (int ix=0;ix<nx;ix++){
        for (int iy=0;iy<ny;iy++){
            for (int iz=0;iz<nz;iz++){
                
                // loop to get all the variables inside the expression
                for (int chem=0;chem<chemass_transf.size();chem++){
                    chemass_transf[chem] = (*Prj_StateVar.chemass)(icmp)(index_transf[chem])(ix,iy,iz);
                    if (chem == 0) m_a = chemass_transf[chem]; // up to 4 variables
                    if (chem == 1) m_b = chemass_transf[chem];
                    if (chem == 2) m_c = chemass_transf[chem];
                    if (chem == 3) m_d = chemass_transf[chem];
                }

                // mass transfered
                transf_mass = expression.value(); 
                std::cout << transf_mass << std::endl;

                // mass consumed
                (*Prj_StateVar.chemass)(icmp)(index_cons)(ix,iy,iz) -= transf_mass;

                // mass prod
                (*Prj_StateVar.chemass)(icmp)(index_prod)(ix,iy,iz) += transf_mass;

            }
        }
     }
}


void ChemTransf(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, int icmp){

    std::string expression_string; // expression string
    
    // Get chem transformations
    int num_transf = JSONfiles.BGC["transformations"].size();

    // Looping over transformations
    for (int transi=0;transi<num_transf;transi++){
 
        Transf<double>(JSONfiles,Prj_StateVar, icmp,transi); // calling exprtk: parsing expression
        
    }
}

