// Abstract syntax for the language Jay with display methods added
#include <iostream>
#include <vector>
#include <string>

#include "AbstractSyntax.hh"

using namespace std;


ostream& operator<<(ostream& out, const Type& t) {
  switch (t.id) {
  case Type::UNDEFINED: out << "undefined"; break;
  case Type::INTEGER:   out << "int";       break;
  case Type::BOOLEAN:   out << "bool";      break;
  case Type::DATE:      out << "date";      break;
  }
  return out;
}


void Indenter::display(const string& message) const {
  // displays a message on the next line at the current level
  cout << endl;
  for (int i = 0; i < level; i++)
    cout << "  ";
  cout << message;
}


void Statement::display(int level) const {
  Indenter indent(level);
  indent.display(string(name()) + ": ");
}

Block::~Block(){
  for (unsigned int i = 0; i < members.size(); i++)
    delete members[i];
}

void Block::display(int level) const {
  Statement::display(level);
  for (unsigned int i = 0; i < members.size(); i++)
    members[i]->display(level+1);
}


void Expression::display(int level) const {
  Indenter indent(level);
  if (name() != "Value")
    indent.display(string(name()) + ": ");
  else {
    const Value* v = dynamic_cast<const Value*>(this);
    if (v->type.isDate())
      indent.display("Date : ");
    else
      indent.display(string(name()) + ": ");
  }
}

void ExpressionProtected::display(int level) const {
  Expression::display(level);
}

Loop::~Loop() {
  delete test; 
  delete body;
}

void Loop::display(int level) const {
  Statement::display(level);
  test->display(level+1);
  body->display(level+1);
}

Conditional::~Conditional() {
  delete test;
  delete elsebranch;
  delete thenbranch;
}

void Conditional::display(int level) const {
  Statement::display(level);
  test->display(level+1);
  thenbranch->display(level+1);
  if (elsebranch != NULL)
    elsebranch->display(level+1);
}


void Variable::display(int level) const {
  Expression::display(level);
  cout << id;
}


void Value::display(int level) const {

  if(level != -1) Expression::display(level);

  if (type.isInteger())
    cout << intValue;
  else if (type.isBool())
    cout << boolValue;
  else if (type.isDate() && ((day->name() != "Value") || (month->name() != "Value") || (year->name() != "Value"))) {
    day->display(level+1);
    month->display(level+1);
    year->display(level+1);
  }
  else if (type.isDate()) {
    cout << "[" << dynamic_cast<Value*>(day)->intValue << ":";
    cout << dynamic_cast<Value*>(month)->intValue << ":";
    cout << dynamic_cast<Value*>(year)->intValue << "]";
  }
}

void Value::normalizeDate() {
  int days[13] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  bool positive;
  Value* d;
  Value* m;
  Value* y;

  if (!type.isDate() && (day->name() != "Value") && (month->name() != "Value") && (year->name() == "Value")) {
    return;
  }
  d = dynamic_cast<Value*>(day);
  m = dynamic_cast<Value*>(month);
  y = dynamic_cast<Value*>(year);

  int mm = m->intValue;

  if ((m->intValue > 0) && (m->intValue > 12)) {
    m->intValue = m->intValue % 12;
    y->intValue += mm / 12;
  }
  else if ((m->intValue < 0) && (m->intValue < -12)) {
    m->intValue = m->intValue % 12 + 13;
    y->intValue += mm / 12;
  }
  else if (m->intValue < 0) {
    m->intValue = 13 + m->intValue;
    y->intValue -= 1;
  }
  else if (m->intValue == 0) {
    m->intValue = 1;
  }


  if (d->intValue == 0) {
    d->intValue = 1;
    return;
  }
  positive = d->intValue > 0;
  int i = m->intValue-1;
  // set number of day in february
  if (((y->intValue % 4 == 0) && (y->intValue % 100 != 0)) || (y->intValue % 400 == 0))
    days[1] = 29; //    "y is bissextile";
  else
    days[1] = 28; //    "y is not bissextile";
    
  // adjust number of day to cooresponding value
  while ((positive && (d->intValue > days[i])) || (!positive && (d->intValue < days[i]))) {
    if (positive) {

      d->intValue = d->intValue - days[i];
      m->intValue += 1;
      ++i;
      if (m->intValue == 13) {
	m->intValue = 1;
	y->intValue += 1;
	i = 0;

	// set number of day in frebruary
	if ((((y->intValue % 4 == 0) && (y->intValue % 100 != 0))) || (y->intValue % 400 == 0))
	  days[1] = 29; //    "y is bissextile";
	else
	  days[1] = 28; //    "y is not bissextile";
      }
    }
    else {
      d->intValue += days[m->intValue];
      m->intValue -= 1;
      if (m->intValue == 0) {
	m->intValue = 12;
	y->intValue -= 1;
	// set number of day in frebruary
	if ((y->intValue % 4 == 0 && y->intValue % 100 != 0) || (y->intValue % 400 == 0))
	  days[1] = 29; //    "y is bissextile";
	else
	  days[1] = 28; //    "y is not bissextile";
      }
    }
  }
}


const string Operator::AND = "&&";
const string Operator::OR = "||";
const string Operator::LT = "<";
const string Operator::LE = "<=";
const string Operator::EQ = "==";
const string Operator::NE = "!=";
const string Operator::GT = ">";
const string Operator::GE = ">=";
const string Operator::PLUS = "+";
const string Operator::MINUS = "-";
const string Operator::TIMES = "*";
const string Operator::DIV = "/";
const string Operator::NOT = "!";


bool Operator::BoolOp() const { return val==AND || val==OR; }

bool Operator::RelationalOp() const { 
  return val==LT || val==LE || val==EQ || val==NE || val==GT || val==GE; 
}

bool Operator::ArithmeticOp() const { 
  return val==PLUS || val==MINUS || val==TIMES || val==DIV; 
}

bool Operator::UnaryOp() const { return val==NOT; }

string Operator::valf() const { return val.substr(0, val.length()-1); }

void Operator::display(int level) const {
  Indenter indent(level);
  indent.display(string(name()) + ": " + val);
}

Binary::~Binary() {
  delete op; 
  delete term1; 
  delete term2;
}

void Binary::display(int level) const {
  Expression::display(level);
  op->display(level+1);
  term1->display(level+1);
  term2->display(level+1);
}

Unary::~Unary() {
  delete op;
  delete term;
}

void Unary::display(int level) const {
  Expression::display(level);
  op->display(level+1);
  term->display(level+1);
}

Assignment::~Assignment() {
  delete target;
  delete source;
}

void Assignment::display(int level) const {
  Statement::display(level);
  target->display(level+1);
  source->display(level+1);
}


Declaration::~Declaration() {
  delete v;
}

void Declaration::display() const {
  cout << "<" << v->id << ", " << t << ">";
}

Declarations::~Declarations() {
  for (unsigned int i = 0; i < vec.size(); i++)
    delete vec[i];
}

void Declarations::display(int level) const {
  Indenter indent(level);
  indent.display("Declarations:");
  indent.display("  Declarations = {");

  for (unsigned int i = 0; i < vec.size(); i++) {
    vec[i]->display();
    if (i < vec.size()-1) cout << ", ";
  }
  cout << "}";
}

Program::~Program() {
  delete decpart;
  delete body;
}

void Program::display() const {
  int level = 0;
  Indenter indent(level);
  indent.display("Program (abstract syntax): ");
  decpart->display(level+1);
  body->display(level+1);
  cout << endl;
}
