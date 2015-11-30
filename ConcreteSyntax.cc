#include "ConcreteSyntax.hh"

using namespace std;
  
void ConcreteSyntax::match(const string& s) {
  if (token.value == s)
    token = input.nextToken();
  else syntaxError(s);
}

  
Program* ConcreteSyntax::program() {
    // Program --> void main ( ) '{' Declarations Statements '}'
  string header[] = {"void", "main", "(", ")"};
  Program* p = new Program();
  for (int i = 0; i < 4; i++)	// bypass "void main ( )"
    match(header[i]);
  match("{");
  p->decpart = declarations();
  p->body = statements();
  match("}");
  return p;
}

  
Declarations* ConcreteSyntax::declarations() {
  // Declarations --> { Declaration }*
  Declarations *ds = new Declarations();  
  while (token.value == "int" || token.value == "boolean" || token.value == "date") {
    declaration(ds);
  }
  return ds;                
}
  

void ConcreteSyntax::declaration(Declarations* ds) {
  // Declaration  --> Type Identifiers ;
  Type t = type();
  identifiers(ds, t);
  match(";");
}

  
Type ConcreteSyntax::type() {
  // Type  -->  int | boolean | date
  Type t;

  if (token.value == "int")
    t = Type::INTEGER;
  else if (token.value == "boolean")
    t = Type::BOOLEAN;
  else if (token.value == "date")
    t = Type::DATE;
  else
    syntaxError("int | boolean | date");

  token = input.nextToken(); // pass over the type
  return t;                
}


void ConcreteSyntax::identifiers(Declarations* ds, Type t) {
  // Identifiers  --> Identifier { , Identifier }*
  Declaration* d = new Declaration(); // first declaration
  d->t = t;			// its type
  if (token.type == "Identifier") {
    d->v = new Variable();       
    d->v->id = token.value;	// its value
    ds->vec.push_back(d);

    token = input.nextToken();
    while (token.value == ",") {
      d = new Declaration();	// next declaration
      d->t = t;			// its type
      token = input.nextToken();
      if (token.type == "Identifier") {
	d->v = new Variable();	// its value
	d->v->id = token.value;
	ds->vec.push_back(d);
	token = input.nextToken(); // get "," or ";"
      }
      else syntaxError("Identifier");
    }
  }
  else syntaxError("Identifier");
}
  

Statement* ConcreteSyntax::statement() {
  // Statement --> ; | Block | Assignment | IfStatement | WhileStatement
  Statement* s = NULL;
  if (token.value == ";") {	// Skip
    token = input.nextToken();
    s = new Skip();               
  }                  
  else if (token.value == "{") { // Block
    token = input.nextToken();
    s = statements();
    match("}");
  }
  else if (token.value == "if")	// IfStatement 
    s = ifStatement();
  else if (token.value == "while") // WhileStatement 
    s = whileStatement();
  else if (token.type == "Identifier") // Assignment
    s = assignment();
  else syntaxError("Statement");
  return s ? s : new Skip();
}

  
Block* ConcreteSyntax::statements() {
  // Block --> '{' Statements '}'
  Block* b = new Block();
  while(! (token.value == "}")) {
    b->members.push_back(statement()); 
  }
  return b;
}

  
Assignment* ConcreteSyntax::assignment() {
  // Assignment --> Identifier = Expression ;
  Assignment* a = new Assignment();

  if (token.type == "Identifier") {
    a->target = new Variable();
    a->target->id = token.value;
    token = input.nextToken();
    match("=");
    a->source = expression();
    match(";");
  }
  else syntaxError("Identifier");
  return a;
}

  
Expression* ConcreteSyntax::expression() {
  // Expression --> Conjunction { || Conjunction }*
  Binary *b; Expression *e;
  e = conjunction();
  while (token.value == "||") {
    b = new Binary();
    b->term1 = e;
    b->op = new Operator(token.value);
    token = input.nextToken();
    b->term2 = conjunction();
    e = b;
  }
  return e;     
}

  
Expression* ConcreteSyntax::conjunction() {
  // Conjunction --> Relation { && Relation }*
  Binary* b; Expression* e;
  e = relation();
  while (token.value == "&&") {
    b = new Binary();
    b->term1 = e;
    b->op = new Operator(token.value);
    token = input.nextToken();
    b->term2 = relation();
    e = b;
  }
  return e;     
}
  

Expression* ConcreteSyntax::relation() {
  // Relation --> Addition [ < | <= | > | >= | == | != ] Addition }*
  Binary* b; Expression* e;
  e = addition();
  while (token.value == "<"  || token.value == "<=" ||
	 token.value == ">"  || token.value == ">=" ||
	 token.value == "==" || token.value == "!="   ) {
    b = new Binary();
    b->term1 = e;
    b->op = new Operator(token.value);
    token = input.nextToken();
    b->term2 = addition();
    e = b;
  }
  return e;
}

  
Expression* ConcreteSyntax::addition() {
  // Addition --> Term { [ + | - ] Term }*
  Binary* b; Expression* e;
  e = term();
  while (token.value == "+" || token.value == "-") {
    b = new Binary();
    b->term1 = e;
    b->op = new Operator(token.value);
    token = input.nextToken();
    b->term2 = term();
    e = b;
  }
  return e;
}
  

Expression* ConcreteSyntax::term() {
  // Term --> date | Negation { [ '*' | / ] Negation }*
  Binary* b; Expression* e;

  e = negation();
  if ((e->name() != "Unary") && token.value == ":") {
    e = date(e);
  }
  else {
    while (token.value == "*" || token.value == "/") {
      b = new Binary();
      b->term1 = e;
      b->op = new Operator(token.value);
      token = input.nextToken();
      b->term2 = negation();
      e = b;
    }
  }
  return e;
}

Expression* ConcreteSyntax::date(Expression* day) {
  // Date --> <factor> : <Factor> : <Factor>
  Value* v;
  Expression* month;
  Expression* year;

  match(":");
  month = factor();
  match(":");
  year = factor();

  v = new Value(day, month, year);

  return v;
}

Expression* ConcreteSyntax::negation() {
  // Negation --> { ! }opt Factor
  Unary* u;
  if (token.value == "!") {
    u = new Unary();
    u->op = new Operator(token.value);
    token = input.nextToken();
    u->term = factor();
    return u;
  }
  else return factor();
}

  
Expression* ConcreteSyntax::factor() {
  // Factor --> Identifier | Literal | ( Expression )
  Expression* e = NULL;
  if (token.type == "Identifier") {
    Variable *v = new Variable();
    v->id = token.value;
    e = v;
    token = input.nextToken();
  }
  else if (token.type == "Literal") {
    Value* v = NULL;

    char buffer[100];
    token.value.copy(buffer, token.value.size(), 0);
    buffer[token.value.size()] = 0;

    if (isInteger(token.value))
      v = new Value(atoi(buffer));
    else if (token.value == "true")
      v = new Value(true);
    else if (token.value == "false")
      v = new Value(false);
    else syntaxError("Literal");
    e = v;
    token = input.nextToken();
  }
  else if (token.value == "(") {
    token = input.nextToken();
    e = expression();       
    match(")");
  }
  else syntaxError("Identifier | Literal | (");
  return e;
}

  
Conditional* ConcreteSyntax::ifStatement() {
  // IfStatement --> if ( Expression ) Statement { else Statement }opt
  Conditional* c = new Conditional();
  match("if");
  c->test = expression();
  c->thenbranch = statement();
  if (token.value == "else"){
    token = input.nextToken();
    c->elsebranch = statement();
  }
  else c->elsebranch = new Skip();
  return c;
}

  
Loop* ConcreteSyntax::whileStatement() {
  // WhileStatement --> while ( Expression ) Statement
  Loop* l = new Loop();
  match("while");
  match("(");
  l->test = expression();
  match(")");
  l->body = statement();
  return l;
}

  
bool ConcreteSyntax::isInteger(string s) const {
  bool result = true;
  for (unsigned int i = 0; i < s.length(); i++)
    if ('0' > s[i] || '9' < s[i])
      result = false;
  return result; 
}
  
void ConcreteSyntax::syntaxError(string tok) {
  cout << "Syntax error: expecting: " << tok 
       << "; saw: " << token.type << " = " <<  token.value << endl;
  exit(0);
}
