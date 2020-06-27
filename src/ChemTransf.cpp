
#include "ChemTransf.h"

template <typename T>
void Transf(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, int icmp, int transi){

    typedef exprtk::symbol_table<T> symbol_table_t;
    typedef exprtk::expression<T>     expression_t;
    typedef exprtk::parser<T>             parser_t;

    std::string chemname;
    int index_cons, index_prod;
    int index_i;
    std::vector<int> index_transf;
    double chemass_consumed, chemass_produced, chemass_transf;

    // Get transformation transi info
    std::string consumed_spec =  JSONfiles.BGC[std::to_string(transi+1)]["consumed"];
    std::string produced_spec =  JSONfiles.BGC[std::to_string(transi+1)]["produced"];
    std::string expression_string = JSONfiles.BGC[std::to_string(transi+1)]["kinetics"];
    
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
        if (index_i!=-1 && !expression_string.empty()) 
            index_transf.push_back(chemi); // index

    }

    std::string expression_string_example = "clamp(-1.0,sin(2 * pi * x) + cos(x / 2 * pi),+1.0)";

    T conc;
    symbol_table_t symbol_table;
    symbol_table.add_variable(chemname,conc);
    symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    parser.compile(expression_string,expression);

    for (conc = T(-5); conc <= T(+5); conc += T(0.001))
    {
        T y = expression.value();
        //printf("%19.15f\t%19.15f\n",x,y);
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

