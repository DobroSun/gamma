#include "gamma/pch.h"
#include "gamma/interp.h"
#include "gamma/console.h"
#include "gamma/buffer.h"


enum TokenType {
  IdentiferType  = 256,
  EndOfLineType  = 257,

  IntegerType    = 258,
  IdentifierType = 259,

  QuitCommandType = 400,
};


struct Token {
  TokenType type = EndOfLineType;

  union {
    bool    boolean_value;
    int     integer_value;
    float   float_value;
    double  double_value;
    literal string_literal;
  };
};


#define NEW_AST(ast_type) new ast_type(ast_type##_Type)
#define DELETE_AST(ast)   delete ast

enum Ast_Type {
  Ast_Error_Type,
  Ast_Quit_Type,
};

struct Ast_Expression {
  Ast_Type type = Ast_Error_Type;
  Ast_Expression(const Ast_Type t) {
    type = t;
  }
};

struct Ast_Quit: public Ast_Expression {
  using Ast_Expression::Ast_Expression;
  int err_code = 0;
};


static Token current_tok;
static int found_keyword;
static const char *cursor = nullptr;

struct Keyword_Def {
  literal   name;
  TokenType type;
};

static const int N_KEYWORDS = 1;
static const Keyword_Def table[N_KEYWORDS] = {
  {"quit", QuitCommandType},
};

static void set_token(const literal &l, const TokenType t) {
  current_tok.string_literal = l;
  current_tok.type = t;
  cursor += l.size;
}

static void set_token(const TokenType t) {
  current_tok.type = t;
  cursor++;
}

static void set_num_token(const char *&c) {
  // only ints for now.
  current_tok.integer_value = atoi(c);
  current_tok.type = IntegerType;

  // @Incomplete.
  c++; // handles only 1 digit number.
}

static void set_ident_token(const char *&c) {
  current_tok.string_literal.data = c;
  current_tok.type = IdentifierType;

  size_t count = 0;
  while(isalpha(*c)) {
    c++;
    count++;
  }
  current_tok.string_literal.size = count;
}

static bool is_keyword(const char *cursor) {
  for(int i = 0; i < N_KEYWORDS; i++) {
    if(cursor == table[i].name) {
      found_keyword = i;
      return true;
    }
  }
  return false;
}


static Token *get_next_token() {
  while(*cursor != '\0') {
    if(isdigit(*cursor)) {
      set_num_token(cursor);
      return &current_tok;
  
    } else if(*cursor == '=') {
      set_token(literal{cursor, 1}, (TokenType)'=');
      return &current_tok;

    } else if(is_keyword(cursor)) {
      Keyword_Def kw = table[found_keyword];
      set_token(kw.name, kw.type);
      return &current_tok;

    } else if(isalpha(*cursor)) {
      set_ident_token(cursor);
      return &current_tok;

    } else {
      cursor++;
    }
  }

  set_token(EndOfLineType);
  return &current_tok;
}


// @Incomplete.
static void report_error(const char *msg) {
  console_put_text(msg);
}

static Ast_Expression *parse() {
  Token *tok = get_next_token();

  bool failed = false;
  if(tok->type == QuitCommandType) {
    auto expr = NEW_AST(Ast_Quit);
    defer { if(failed) { DELETE_AST(expr); expr = nullptr; }};


    tok = get_next_token();
    if(tok->type == IntegerType) {
      expr->err_code = tok->integer_value;
      return expr;

      tok = get_next_token();
      if(tok->type == EndOfLineType) {
        failed = true;
        return expr;
      }

    } else if(tok->type == EndOfLineType) {
      return expr;

    } else {
      // report_error.
      failed = true;
      return expr;
    }
  } else if(tok->type == IntegerType) {
    // Don't want to allocate memory for ast node.
    // That's why it's here.
    go_to_line(tok->integer_value);

  } else if(tok->type == IdentifierType) {
    report_error("Oh, no doesn't expect an identifier");
    failed = true;
  }
  return nullptr;
}

void dealloc(Ast_Expression *ast) {
  switch(ast->type) {
    case Ast_Quit_Type: {
      auto e = static_cast<Ast_Quit *>(ast);
      DELETE_AST(e);
    } break;

    default: {
    } break;
  }
}


// @Fix: 
// quit0 - works
// quitt - does nothing.
//
void interp(const char *s) {
  cursor = s;

  Ast_Expression *ast = parse();
  defer { if(ast) dealloc(ast); };

  if(ast) {
    switch(ast->type) {
      case Ast_Quit_Type: {
        auto e = static_cast<Ast_Quit *>(ast);
        exit(e->err_code);
      } break;

      case Ast_Error_Type: {
      } break;
      
      default: {
      } break;
    }
  } else {
  }
  cursor = nullptr;
}
