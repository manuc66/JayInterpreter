#include "ConcreteSyntax.hh"
#include "AbstractSyntax.hh"
#include "TokenStream.hh"
#include "StaticTypeCheck.hh"
#include "Semantics.hh"

#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
  if (argc == 2) {
    
    /* Massage the output of your program until it matches exactly
       the sample file provided (out.txt).                       */
    
    
    /* 
       .Create a ConcreteSyntax object and use it to parse a Jay
       program whose file name is given on the command line. Call the
       display() method of the resultant AbstractSyntax object. 
       
       .Create a StaticTypeCheck and a TypeMap object and use them
       to verify the typing of a program (AbstractSyntax object). 
     Call the display() method on both objects. 
    */
    
    
    TokenStream ts(argv[1]);
    ConcreteSyntax concreteSyntax(ts);

    cout << "Parsing " << argv[1] << ": ";
    Program* program = concreteSyntax.program();
    //program->display();
    cout << "No syntax error" << endl;

    cout << "Type checking: ";
    StaticTypeCheck stc;
    TypeMap* tm_main = stc.typing(program->decpart);

    if (stc.V(program, tm_main)) {
      cout << "No type errors" << endl;
      cout << "Type map = ";
      tm_main->display();
      //program->display();

      cout << endl << "Interpreting:" << endl;
      Semantics semantics;
      State* sigma = semantics.M(program);
      cout << "\t" << "Final state sigma = ";
      sigma->display();
      delete sigma;
    }
    else cout << "Type error!" << endl;
    delete tm_main;
    delete program;
  }                       
  else {

  }
}

