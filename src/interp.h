#ifndef GAMMA_INTERPRETER_H
#define GAMMA_INTERPRETER_H

enum TokenType {
  IdentiferType  = 256,
  EndOfLineType  = 257,

  IntegerType    = 258,
  BooleanType    = 259,
  Float32Type    = 260,
  Float64Type    = 261,
  IdentifierType = 262,
  LiteralType    = 263,
  VoidType       = 264,
  ConstType      = 265,
  StaticType     = 266,
  ExternType     = 267,

  QuitCommandType = 400,
  SetCommandType  = 401,
  HSplitCommandType = 402,
  VSplitCommandType = 403,
};

union RValue {
  bool   boolean_value;
  char   char_value;
  int    integer_value;
  float  float32_value;
  double float64_value;
  unsigned unsigned_value;
};

struct Var {
  TokenType type = EndOfLineType;
  RValue    value;
};

struct Token {
  TokenType type = EndOfLineType;

  RValue  value;
  literal string_literal;

  int l, c;
};

Token *get_next_token();

void set_interp_state(const char *);
void interp(const char *);
void init_var_table();

#endif
