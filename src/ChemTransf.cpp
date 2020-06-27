
#include "ChemTransf.h"

void Transf(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, int icmp, int transi){

    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double>     expression_t;
    typedef exprtk::parser<double>             parser_t;

    std::string chemname;
    int index_cons, index_prod;
    int index_i;
    std::vector<int> index_transf;
    double chemass_consumed, chemass_produced;
    std::vector<double> chemass_transf;
    
    // Get transformation transi info
    std::string consumed_spec =  JSONfiles.BGC[std::to_string(transi+1)]["consumed"];
    std::string produced_spec =  JSONfiles.BGC[std::to_string(transi+1)]["produced"];
    std::string expression_string = JSONfiles.BGC[std::to_string(transi+1)]["kinetics"];
    std::string expression_string_modif = expression_string;
    
    // Get chemical species in compartment
    std::vector<std::string> chem_species = JSONfiles.WQ["compartments"][std::to_string(icmp+1)]["chem_species"];
     
    // Find species indexes: consumed, produced and in the expression
    for(int chemi=0;chemi<chem_species.size();chemi++){
        chemname = chem_species[chemi];

        // Consumed
        index_i = chemname.find(consumed_spec);
        if (index_i!=-1 && !consumed_spec.empty()) 
            index_cons = chemi; // index

        // Produced
        index_i = chemname.find(produced_spec);
        if (index_i!=-1 && !produced_spec.empty())
            index_prod = chemi; // index

        // In expression
        index_i = expression_string.find(chemname);
        if (index_i!=-1 && !expression_string.empty()){
            index_transf.push_back(chemi); // index
            expression_string_modif.replace(index_i,chemname.size()-1,"chemass_transf[i]");
        }

    }

    std::string expression_string_example = "clamp(-1.0,sin(2 * pi * x) + cos(x / 2 * pi),+1.0)";

    // Add variables
    symbol_table_t symbol_table;
    symbol_table.add_variable(chem_species[index_cons],chemass_consumed);
    symbol_table.add_variable(chem_species[index_prod],chemass_produced);
    for (int i=0;i<index_transf.size();i++){
        symbol_table.add_variable(chem_species[index_transf[i]],chemass_transf[i]);
    }
    // symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    parser.compile(expression_string,expression);

    //for (conc = T(-5); conc <= T(+5); conc += T(0.001))
    //{
    //    T y = expression.value();
        //printf("%19.15f\t%19.15f\n",x,y);
    //}


}


void ChemTransf(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, int icmp){

    std::string expression_string; // expression string
    
    // Get chem transformations
    int num_transf = JSONfiles.BGC["transformations"].size();

    // Looping over transformations
    for (int transi=0;transi<num_transf;transi++){
 
        Transf(JSONfiles,Prj_StateVar, icmp,transi); // calling exprtk: parsing expression
        
    }
}

