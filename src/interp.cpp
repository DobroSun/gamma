#include "gamma/pch.h"
#include "gamma/interp.h"
#include "gamma/console.h"
#include "gamma/buffer.h"
#include "gamma/init.h"
#include "gamma/commands.h"


// @CleanUp: @Temporary:
size_t hash_literal(const literal &l) {
	const size_t prime = 31;
	size_t result = 0;
	for (size_t i = 0; i < l.size; ++i) {
			result = l.data[i] + (result * prime);
	}
	return result;
}

namespace std {
	template<>
	struct hash<literal> {
		size_t operator()(const literal& l) const {
				return hash_literal(l);
		}
	};
} // namespace std
//

enum TokenType {
  IdentiferType  = 256,
  EndOfLineType  = 257,

  IntegerType    = 258,
  BooleanType    = 259,
  Float32Type    = 260,
  Float64Type    = 261,
  IdentifierType = 262,
  LiteralType    = 263,

  QuitCommandType = 400,
  SetCommandType  = 401,
  HSplitCommandType = 402,
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
  TokenType type = EndOfLineType; // @Hack: only need int, bool, ...
  RValue    value;
};


#define push_to_table(val, member, type_t) \
  { \
    Var r; \
    r.type = (type_t); \
    r.value.member = (val); \
    literal l = (#val); \
    attach_table[l] = r; \
  }
#define push_int_value(val) \
  push_to_table(val, integer_value, IntegerType)
#define push_bool_value(val) \
  push_to_table(val, boolean_value, BooleanType)
  



static std::unordered_map<literal, Var> attach_table;
void init_var_table() {
  push_bool_value(show_fps);
}


struct Token {
  TokenType type = EndOfLineType;

  union {
    RValue value;
    literal string_literal;
  };
};


#define NEW_AST(ast_type) new ast_type(ast_type##_Type)
#define DELETE_AST(ast)   delete ast

enum Ast_Type {
  Ast_Error_Type,
  Ast_Quit_Type,
  Ast_Set_Type,
  Ast_Split_Type,
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

struct Ast_Set: public Ast_Expression {
  using Ast_Expression::Ast_Expression;
  literal name;
  Var     var;
};

struct Ast_Split: public Ast_Expression {
  using Ast_Expression::Ast_Expression;
  literal path;
};


static Token current_tok;
static int found_keyword;
static const char *cursor = nullptr;

struct Keyword_Def {
  literal   name;
  TokenType type;
};

static const Keyword_Def table[] = {
  {"quit", QuitCommandType},
  {"q",    QuitCommandType},

  {"set", SetCommandType},

  {"split", HSplitCommandType},
  {"hsp",   HSplitCommandType},
  {"sp",    HSplitCommandType},
};

static void set_keyword_token(const literal &l, const TokenType t) {
  current_tok.string_literal = l;
  current_tok.type = t;
  cursor += l.size;
}

static bool is_boolean(const char *c) {
  return (c == literal{"true"} || c == literal{"false"});
}

static void set_bool_token(const char *&c) {
  current_tok.type = BooleanType;
  if(c[0] == 't') {
    c += 4;
    current_tok.value.boolean_value = true;

  } else if(c[0] == 'f') {
    c += 5;
    current_tok.value.boolean_value = false;

  } else {
    assert(0);
  }
}

static void set_token(const TokenType t) {
  current_tok.type = t;
  cursor++;
}

static void set_num_token(const char *&c) {
  // only ints for now.
  current_tok.value.integer_value = atoi(c);
  current_tok.type = IntegerType;

  while(isdigit(*c)) c++;
}

static void set_literal_token(const char *&c) {
  c++;
  int count = 0;
  while(c[count] != '\0') {
    if(c[count] == '\"' || c[count] == '\'') break;
    count++;
  }
  current_tok.type = LiteralType;
  current_tok.string_literal.data = c;
  current_tok.string_literal.size = count;
  c += count;
  c++;
}

static void set_ident_token(const char *&c) {
  current_tok.string_literal.data = c;
  current_tok.type = IdentifierType;

  size_t count = 0;
  while(isalpha(*c) || *c == '_' || isdigit(*c)) {
    c++;
    count++;
  }
  current_tok.string_literal.size = count;
}

static bool is_keyword(const char *cursor) {
  for(unsigned i = 0; i < arr_size(table); i++) {
    if(cursor == table[i].name) {
      auto tmp = cursor;
      tmp += table[i].name.size;

      if(*tmp == '\0' || *tmp == ' ' || *tmp == '\t') {
        found_keyword = i;
        return true;
      } else {
        continue;
      }
    }
  }
  return false;
}

static bool is_literal(char c) {
  return c == '\"' || c == '\'';
}

static bool is_number(char c) {
  return isdigit(c) || c == '-';
}


static Token *get_next_token() {
  while(*cursor != '\0') {
    if(is_number(*cursor)) {
      set_num_token(cursor);
      return &current_tok;

    } else if(is_literal(*cursor)) {
      set_literal_token(cursor);
      return &current_tok;
  
    } else if(is_boolean(cursor)) {
      set_bool_token(cursor);
      return &current_tok;

    } else if(*cursor == '=') {
      set_token((TokenType)'=');
      return &current_tok;

    } else if(is_keyword(cursor)) {
      Keyword_Def kw = table[found_keyword];
      set_keyword_token(kw.name, kw.type);
      return &current_tok;

    } else if(isalpha(*cursor) || *cursor == '_') {
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
      expr->err_code = tok->value.integer_value;
      return expr;

      tok = get_next_token();
      if(tok->type == EndOfLineType) {
        failed = true;
        return expr;
      }

    } else if(tok->type == EndOfLineType) {
      return expr;

    } else {
      // report error.
      failed = true;
      return expr;
    }

  } else if(tok->type == SetCommandType) {
    auto expr = NEW_AST(Ast_Set);
    defer { if(failed) { DELETE_AST(expr); expr = nullptr; }};

    tok = get_next_token();
    if(tok->type != IdentifierType) {
      // report error.
      failed = true;
      return expr;
    }
    assert(tok->type == IdentifierType);
    expr->name = tok->string_literal;

    tok = get_next_token();
    if(tok->type != '=') {
      // report error.
      failed = true;
      return expr;
    }
    assert(tok->type == '=');

    tok = get_next_token();
    if(tok->type == IntegerType) {
      expr->var.type = IntegerType;
      expr->var.value.integer_value = tok->value.integer_value;

    } else if(tok->type == BooleanType) {
      expr->var.type = BooleanType;
      expr->var.value.boolean_value = tok->value.boolean_value;

    } else if(tok->type == Float32Type) {
      // @Incomplete.
      expr->var.value.float32_value = tok->value.float32_value;

    } else if(tok->type == Float64Type) {
      // @Incomplete.
      expr->var.value.float64_value = tok->value.float64_value;

    } else {
      // @Incomplete: report error.
      failed = true;
      return expr;
    }

    tok = get_next_token();
    if(tok->type != EndOfLineType) {
      failed = true;
      return expr;
    }
    return expr;

  } else if(tok->type == HSplitCommandType) {
    auto expr = NEW_AST(Ast_Split);
    defer { if(failed) DELETE_AST(expr); expr = nullptr; };

    tok = get_next_token();
    if(tok->type == LiteralType) {
      expr->path = tok->string_literal;

    } else if(tok->type == EndOfLineType) {
      expr->path.data = nullptr;
      
    } else {
      // @Incomplete: report error.
      failed = true;
    }
    return expr;
  
  } else if(tok->type == IntegerType) {
    // Don't want to allocate memory for ast node.
    // That's why it's interpreted here.
    go_to_line(tok->value.integer_value);

  } else if(tok->type == IdentifierType) {
    report_error("Oh, no doesn't expect an identifier");
    failed = true;
  }
  return nullptr;
}

#define DEALLOC_JUST(ast_type) \
  case ast_type##_Type: { \
    auto e = static_cast<ast_type *>(ast); \
    DELETE_AST(e); \
  } break;

static void dealloc(Ast_Expression *ast) {
  switch(ast->type) {
    DEALLOC_JUST(Ast_Quit);
    DEALLOC_JUST(Ast_Set);
    DEALLOC_JUST(Ast_Split);
    default: {
    } break;
  }
}


#define attach_to_table(name) \
  attach_value(&name, #name)

static void attach_value(bool *val, const literal &name) {
  Var *r = &attach_table[name];
  if(r->type == BooleanType) {
    *val = r->value.boolean_value;
  } else {
    // @Incomplete: report error.
    r->type = BooleanType;
    r->value.boolean_value = *val;
  }
}

static void attach_value(int *val, const literal &name) {
  Var *r = &attach_table[name];
  if(r->type == IntegerType) {
    *val = r->value.integer_value;
  } else {
    // @Incomplete: report error.
    r->type = IntegerType;
    r->value.integer_value = *val;
  }
}

static void update_variables() {
  attach_to_table(show_fps);
}


void interp(const char *s) {
  cursor = s;

  if(Ast_Expression *ast = parse()) {
    defer { dealloc(ast); };

    switch(ast->type) {
      case Ast_Quit_Type: {
        auto e = static_cast<Ast_Quit *>(ast);
        quit(e->err_code);
      } break;

      case Ast_Set_Type: {
        auto e = static_cast<Ast_Set *>(ast);
        //
        // @SpeadUp: 
        // No need to update all variables, if our command is bad formed
        // for example when, we want assign integer to a bool value, or
        // when trying to reset value with itself.
        // 
        attach_table[e->name] = e->var;
        update_variables();
      } break;

      case Ast_Split_Type: {
        auto e = static_cast<Ast_Split *>(ast);
        split(e->path);

      } break;
      
      default: {
      } break;
    }
  }
  cursor = nullptr;
}
