
#include "ChemTransf.h"

template <typename T>
void Transf(std::string expression_string){

   typedef exprtk::symbol_table<T> symbol_table_t;
   typedef exprtk::expression<T>     expression_t;
   typedef exprtk::parser<T>             parser_t;

   T x;

   symbol_table_t symbol_table;
   symbol_table.add_variable("x",x);
   symbol_table.add_constants();

   expression_t expression;
   expression.register_symbol_table(symbol_table);

   parser_t parser;
   parser.compile(expression_string,expression);

   for (x = T(-5); x <= T(+5); x += T(0.001))
   {
      T y = expression.value();
      //printf("%19.15f\t%19.15f\n",x,y);
   }


}


void ChemTransf(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar, int icmp){

    std::string expression_string; // expression string
    
    // Get chemical species in compartment
    std::vector<std::string> chem_species = JSONfiles.WQ["compartments"][std::to_string(icmp+1)]["chem_species"];

    // Get chem transformations
    int num_transf = JSONfiles.BGC["transformations"].size();

    // Looping over transformations
    for (int transi=0;transi<num_transf;transi++){
 
        expression_string = JSONfiles.BGC[std::to_string(transi+1)]["kinetics"];
        Transf<double>(expression_string); // calling exprtk: parsing expression
        
    }
}

