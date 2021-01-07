#include "pch.h"
#include "interp.h"
#include "console.h"
#include "buffer.h"
#include "init.h"


// @CleanUp: @Temporary:
static size_t hash_literal(const literal &l) {
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


static int nline = 1, nchar = 1;



#define push_to_table(val, member, type_t) \
  { \
    Var r; \
    r.type = (type_t); \
    r.value.member = (val); \
    literal l = (#val); \
    attach_table[l] = r; \
  }

#define push_int_value(val)  push_to_table(val, integer_value, IntegerType)
#define push_bool_value(val) push_to_table(val, boolean_value, BooleanType)
  

static std::unordered_map<literal, Var> attach_table;
void init_var_table() {
  push_bool_value(show_fps);
}


#define NEW_AST(ast_type) new ast_type(ast_type##_Type)
#define DELETE_AST(ast)   delete ast

enum Ast_Type {
  Ast_Error_Type,
  Ast_Set_Type,
  Ast_Split_Type,
};

struct Ast_Expression {
  Ast_Type type = Ast_Error_Type;
  Ast_Expression(const Ast_Type t) {
    type = t;
  }
};

struct Ast_Set : public Ast_Expression {
  using Ast_Expression::Ast_Expression;
  literal name;
  Var     var;
};

struct Ast_Split : public Ast_Expression {
  using Ast_Expression::Ast_Expression;
  literal path;
  split_type_t split_type;
};


static Token       current_tok;
static int         found_keyword;
static const char *cursor;

struct Keyword_Def {
  literal   name;
  TokenType type;
};


// @Speed: Actually needs to be a hashtable O(1) lookup.
static const Keyword_Def table[] = {
  {"quit", QuitCommandType},
  {"q",    QuitCommandType},

  {"set",  SetCommandType},

  {"hsplit", HSplitCommandType},
  {"hsp",    HSplitCommandType},
  {"sp",     HSplitCommandType},
  {"vsplit", VSplitCommandType},
  {"vsp",    VSplitCommandType},

  {"int",    IntegerType},
  {"bool",   BooleanType},
  {"void",   VoidType},
  {"const",  ConstType},
  {"static", StaticType},
  {"extern", ExternType},
};

static void set_keyword_token(const literal &l, const TokenType t) {
  current_tok.string_literal = l;
  current_tok.type = t;
  cursor += l.size;
}

static bool is_boolean(const char *c) {
  return (c == "true" || c == "false");
}

static void set_bool_token(const char *&c) {
  current_tok.type = BooleanType;
  if(c[0] == 't') {
    assert(c == "true");
    c += 4;
    current_tok.value.boolean_value = true;
    current_tok.string_literal = "true";

  } else if(c[0] == 'f') {
    assert(c == "false");
    c += 5;
    current_tok.value.boolean_value = false;
    current_tok.string_literal = "false";

  } else {
    assert(0);
  }
}

static void set_token(const char *c) {
  current_tok.type = (TokenType)(*c);
  current_tok.string_literal = literal(c, 1);
  cursor++;
}

static void set_num_token(const char *&c) {
  // only ints for now.
  current_tok.type = IntegerType;
  current_tok.value.integer_value = atoi(c);
  current_tok.string_literal = "";

  do {
    c++; 
    current_tok.string_literal.size++; 
  } while(isdigit(*c));
  
}

static void set_literal_token(const char *&c) {
  assert(*c == '\"' || *c == '\'');
  const char s = *c;

  c++;
  int count = 0;
  while(c[count] != s) {
    if(c[count] == '\0') {
      // report error.
      break;
    }
    count++;
  }
  current_tok.type = LiteralType;
  current_tok.string_literal.data = c;
  current_tok.string_literal.size = count;
  c += count + 1;
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
  for(size_t i = 0; i < array_size(table); i++) {
    if(cursor == table[i].name) {
      auto tmp = cursor;
      tmp += table[i].name.size;

      if(is_one_of(*tmp, " \r\t") || *tmp == '\0') {
        found_keyword = i;
        return true;
      } else {
        continue;
      }
    }
  }
  return false;
}

static bool is_string_literal(const char *c) { return is_one_of(*c, "\"\'"); }
static bool is_number(const char *c)      { return isdigit(*c) || *c == '-'; }
static bool is_identifier(const char *c)  { return isalpha(*c) || *c == '_'; }


Token *get_next_token() {
  defer {
    current_tok.l = nline;
    current_tok.c = nchar;

    // @Incomplete:
    // Multiple strings?
    nchar += current_tok.string_literal.size;
  };

  while(*cursor != '\0') {
    if(is_number(cursor)) {
      set_num_token(cursor);
      return &current_tok;

    } else if(is_string_literal(cursor)) {
      set_literal_token(cursor);
      return &current_tok;
  
    } else if(is_boolean(cursor)) {
      set_bool_token(cursor);
      return &current_tok;

    } else if(is_one_of(*cursor, "(){}=;,.*&[]+-/!<>%?:#|^~")) {
      set_token(cursor);
      return &current_tok;

    } else if(is_keyword(cursor)) {
      const Keyword_Def kw = table[found_keyword];
      set_keyword_token(kw.name, kw.type);
      return &current_tok;

    } else if(is_identifier(cursor)) {
      set_ident_token(cursor);
      return &current_tok;

    } else {

      if(*cursor == '\n') {
        nline++;
        nchar = 1;
      } else {
        nchar++;
      }

      cursor++;
    }
  }
  
  current_tok.type = EndOfLineType;
  assert(*cursor == '\0');
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
    tok = get_next_token();
    if(tok->type == EndOfLineType) {
      close_split(get_current_buffer());
      
    } else {
      // report error.
    }
    return NULL;

  } else if(tok->type == SetCommandType) {
    auto expr = NEW_AST(Ast_Set);
    defer { if(failed) { DELETE_AST(expr); expr = NULL; }};

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
    expr->split_type = hsp_type;

    defer { if(failed) DELETE_AST(expr); expr = NULL; };

    tok = get_next_token();
    if(tok->type == LiteralType) {
      expr->path = tok->string_literal;

    } else if(tok->type == EndOfLineType) {
      expr->path.data = NULL;
      
    } else {
      // @Incomplete: report error.
      failed = true;
    }
    return expr;

  } else if(tok->type == VSplitCommandType) {
    auto expr = NEW_AST(Ast_Split);
    expr->split_type = vsp_type;

    defer { if(failed) DELETE_AST(expr); expr = NULL; };

    tok = get_next_token();
    if(tok->type == LiteralType) {
      expr->path = tok->string_literal;

    } else if(tok->type == EndOfLineType) {
      expr->path.data = NULL;
      
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
  return NULL;
}

#define DEALLOC_JUST(ast_type) \
  case ast_type##_Type: { \
    auto e = static_cast<ast_type *>(ast); \
    DELETE_AST(e); \
  } break;

static void dealloc(Ast_Expression *ast) {
  switch(ast->type) {
    DEALLOC_JUST(Ast_Set);
    DEALLOC_JUST(Ast_Split);
    default: {
    } break;
  }
}


#define attach_to_table(name) attach_value(&name, #name)
  

static void attach_value(bool *val, const literal &name) {
  Var *r = &attach_table[name];
  if(r->type == BooleanType) {
    *val = r->value.boolean_value;
  } else {
    // @Incomplete: report error.
  }
}

static void attach_value(int *val, const literal &name) {
  Var *r = &attach_table[name];
  if(r->type == IntegerType) {
    *val = r->value.integer_value;
  } else {
    // @Incomplete: report error.
  }
}

static void update_variables() {
  attach_to_table(show_fps);
}

void set_interp_state(const char *s) {
  cursor = s; nline = 1, nchar = 1;
}


void interp(const char *s) {
  set_interp_state(s);

  while(Ast_Expression *ast = parse()) {
    switch(ast->type) {
      case Ast_Set_Type: {
        auto e = static_cast<Ast_Set *>(ast);
        //
        // @SpeadUp: 
        // No need to update all variables, if our command is wrong
        // for example when, we want assign integer to a bool value, or
        // when trying to reset value with itself.
        // 
        attach_table[e->name] = e->var;
        update_variables();
      } break;

      case Ast_Split_Type: {
        auto e = static_cast<Ast_Split *>(ast);

        buffer_t *p = get_current_buffer();
        buffer_t *n;

        if(!e->path.data) {
          open_existing_buffer(p);
        } else {
          open_existing_or_new_buffer(e->path);
        }

        n = get_current_buffer();
        do_split(p, n, e->split_type);

      } break;
      
      default: {
      } break;
    }

    dealloc(ast);
  }
}
