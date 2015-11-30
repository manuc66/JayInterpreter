// Abstract syntax for the language Jay with display methods added
#ifndef AbstractSyntax_H
#define AbstractSyntax_H

#include <iostream>
#include <vector>
#include <string>


class Indenter {
 public: 
  Indenter(int nextlevel = 0) : level(nextlevel) {}

  void display(const std::string& message) const;

  const int level;
};


class Type {
  // Type = int | bool | date | undefined
  // Light class; an instance has only the size of an integer

 public:
  enum type_t {UNDEFINED, INTEGER, BOOLEAN, DATE};

  Type(type_t t = UNDEFINED) : id(t) {}
   
  bool isBool()      const { return id == BOOLEAN;   }
  bool isInteger()   const { return id == INTEGER;   }
  bool isDate()      const { return id == DATE;      } 
  bool isUndefined() const { return id == UNDEFINED; } 

  type_t id;			// not const because needs to be copied

  bool operator==(const Type& t) const { return id == t.id; }
  bool operator==(const type_t t) const { return id == t; }
  Type& operator=(const type_t t) { id = t; return *this; }
  
  friend std::ostream& operator<<(std::ostream&, const Type&);
};


class Statement {
  // Statement = Skip | Block | Assignment | Conditional | Loop
 public:
  Statement() {}
  virtual ~Statement() {}
  virtual void display(int level) const;
  virtual const std::string name() const { return "Statement"; }

 private:
  // Prevent forbidden actions:
  Statement(const Statement&);
  Statement& operator=(const Statement&);
};


class Skip : public  Statement {
 public:
  virtual const std::string name() const { return "Skip"; }
};


class Block : public Statement {
  // Block = Statement * (a Vector of members)
 public: 
  ~Block();

  // Vector members = new Vector();
  std::vector<Statement*> members;
  virtual void display(int level) const;
  virtual const std::string name() const { return "Block"; }
};


class Expression {
  // Expression = Variable | Value | Binary | Unary
 public:
  Expression() {}
  virtual ~Expression() {}
  virtual void display(int level) const;
  virtual const std::string name() const { return "Expression"; }

};

class ExpressionProtected : public Expression {
 public:
  ExpressionProtected() {}
  virtual ~ExpressionProtected() {}
  virtual void display(int level) const;
  virtual const std::string name() const { return "Expression"; }

 private:
  // Prevent forbidden actions:
  ExpressionProtected(const ExpressionProtected&);
  ExpressionProtected& operator=(const ExpressionProtected&);
};


class Loop : public Statement {
  // Loop = Expression test; Statement body
 public:
  Expression* test;
  Statement* body;
  
  Loop():test(NULL),body(NULL) {}
  ~Loop();
  virtual void display(int level) const;
  virtual const std::string name() const { return "Loop"; }
};


class Conditional : public Statement {
  // Conditional = Expression test; Statement thenbranch, elsebranch
 public:
  Expression* test;
  Statement *thenbranch, *elsebranch;
  // elsebranch == null means "if... then" statement

  Conditional():test(NULL),thenbranch(NULL),elsebranch(NULL) {}
  ~Conditional();
  virtual void display(int level) const;
  virtual const std::string name() const { return "Conditional"; }
};


class Variable : public  ExpressionProtected {
  // Variable = string id
 public:  
  bool operator==(const Variable& v) const { return id == v.id; }

  virtual void display(int level) const;
  virtual const std::string name() const { return "Variable"; }
  
  std::string id;
};


class Value : public Expression {
  // Value = int intValue | bool boolValue
 public:
  Value(int i) : type(Type::INTEGER), intValue(i) {}
  Value(bool b) : type(Type::BOOLEAN), boolValue(b) {} 
  Value(Expression* d, Expression* m, Expression* y) : 
    type(Type::DATE), day(d), month(m), year(y) {}
  Value() : type(Type::UNDEFINED) {}

  Value(const Value& v): 
    type(v.type), intValue(v.intValue), boolValue(v.boolValue), 
    day(v.day), month(v.month), year(v.year) {} 
  Value& operator=(const Value& v){
    type = v.type;
    intValue = v.intValue;
    boolValue = v.boolValue;
    day = v.day;
    month = v.month;
    year = v.year;
    return *this;
  }

  virtual void display(int level) const;
  virtual const std::string name() const { return "Value"; }

  void normalizeDate();

  Type type;
  int intValue; // for int
  bool boolValue; // for bool
  Expression *day, *month, *year; // for date
};

class Operator {
 public: 
  // Operator = BoolOp | RelationalOp | ArithmeticOp | UnaryOp
  // BoolOp = && | ||
  static const std::string AND;
  static const std::string OR;
  // RelationalOp = < | <= | == | != | >= | >
  static const std::string LT;
  static const std::string LE;
  static const std::string EQ;
  static const std::string NE;
  static const std::string GT;
  static const std::string GE;
  // ArithmeticOp = + | - | * | /
  static const std::string PLUS;
  static const std::string MINUS;
  static const std::string TIMES;
  static const std::string DIV;
  // UnaryOp = !
  static const std::string NOT;

  std::string val;

  Operator(const std::string& s) : val(s) {}
    
  bool BoolOp() const;
  bool RelationalOp() const;
  bool ArithmeticOp() const; 
  bool UnaryOp() const;

  std::string valf() const;

  void display(int level) const;
  const std::string name() const { return "Operator"; }

 private:
  // Prevent forbidden actions:
  Operator(const Operator&);
  Operator& operator=(const Operator&);
};


class Binary : public ExpressionProtected {
  // Binary = Operator op; Expression term1, term2
 public:
  Operator* op;
  Expression *term1, *term2;

  Binary():op(NULL), term1(NULL), term2(NULL) {}
  ~Binary();
  virtual void display(int level) const;
  virtual const std::string name() const { return "Binary"; }
};


class Unary : public ExpressionProtected {
  // Unary = Operator op; Expression term
 public: 
  Operator* op;
  Expression* term;

  Unary(Operator* o = NULL, Expression* t = NULL) : op(o), term(t) {}
  ~Unary();
  virtual void display(int level) const;
  virtual const std::string name() const { return "Unary"; }
};


class Assignment : public Statement {
  // Assignment = Variable target; Expression source
 public:
  Variable *target;
  Expression *source;

  Assignment():target(NULL),source(NULL) {}
  ~Assignment();
  virtual void display(int level) const;
  virtual const std::string name() const { return "Assignment"; }
};


class Declaration {
  // Declaration = Variable v; Type t
 public:  
  Variable* v;
  Type t;
  
  Declaration():v(NULL) {}
  ~Declaration();
  void display() const; 
  const std::string name() const { return "Declaration"; }
};


class Declarations
{
  // Declarations = Declaration *
  //                (a Vector of declarations d1, d2, ..., dn)
 
 public:
  std::vector<Declaration*> vec;
  
  ~Declarations();
  void display(int level) const;
  const std::string name() const { return "Declarations"; }
};


class Program {
  // Program = Declarations decpart ; Block body
 public:
  Declarations* decpart;
  Block* body;

  Program():decpart(NULL),body(NULL) {}
  ~Program();
  void display() const;
  const std::string name() const { return "Program"; }
};

#endif

