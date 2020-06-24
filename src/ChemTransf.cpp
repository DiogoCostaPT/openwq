
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
      printf("%19.15f\t%19.15f\n",x,y);
   }


}


void ChemTransf(JSONfiles& JSONfiles,Prj_StateVar& Prj_StateVar){

   // 
   std::string expression_string = "clamp(-1.0,sin(2 * pi * x) + cos(x / 2 * pi),+1.0)";
    Transf<double>(expression_string);
  
}

