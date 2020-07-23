#include "gamma/pch.h"
#include "gamma/interpreter.h"
#include "gamma/globals.h"
#include "gamma/view.h"
#include "gamma/buffer.h"
#include "gamma/init.h"


enum TokenType {
  Assign,

  Number,
  Identifier,
  Literal,

  Keyword_Set,
  Keyword_Open,
  End
};

struct Token {
  TokenType type = End;
  union {
    char *identifier_name;
    int   integer_value;
    char *string_literal;
  };
};

enum AST_Type {
  AST_SetDirective,
  AST_OpenDirective,
  AST_None
};

struct AST_Expression {
  AST_Type type = AST_None;
};

struct AST_Set: public AST_Expression {
  char *identifier_name = nullptr;
  int   integer_value;
};

struct AST_Open: public AST_Expression {
  char *string_literal = nullptr;
};



// @Incomplete:
// Add size checks in get_name & get_literal.
#define MAX_IDENTIFIER_NAME_SIZE 64
#define MAX_STRING_LITERAL_SIZE MAX_IDENTIFIER_NAME_SIZE 
#define pair(A) {#A, A}


static Token current_token;
static Token *next_token = nullptr;
static const char *cursor = nullptr;

// @Incomplete:
// We are handling only one directive while parsing.
static AST_Expression *current_expr = nullptr;

// Map of all modifyable variables from script(with their default values}.
static settings_map map = {
  pair(tabstop),
  pair(Width),
  pair(Height)
};


// @Incomplete: 
// Need another way to put error messages.
static void report_error(const char *msg) {
  fprintf(stderr, "%s", msg);
}

static void set_directive(AST_Set *expr) {
  map[expr->identifier_name] = expr->integer_value;
}

static void open_directive(AST_Open *expr) {
  auto &buffer = get_buffer();
  char *filename = expr->string_literal;

  buffer.clear();
  bool success = load_buffer_from_file(filename);
  if(!success) {
    char msg[128];
    sprintf(msg, "Error opening file: <%s>.\n", filename);
    report_error(msg);
  }
  // Do not free filename for now, cause in the end we will run through all AST nodes to free memory.
}

static void destruct(AST_Expression *e) {
  if(!e) return;
  switch(e->type) {
    case AST_SetDirective: {
      auto expr = static_cast<AST_Set *>(e);
      free(expr->identifier_name);
      free(expr);
    } break;

    case AST_OpenDirective: {
      auto expr = static_cast<AST_Open *>(e);
      free(expr->string_literal);
      free(expr);
    } break;

    default: {
    } break;
  }
}

static void destruct(Token *t) {
  assert(t);
  switch(t->type) {
    case Identifier: {
      free(t->identifier_name);
      t->identifier_name = nullptr;
    } break;

    case Literal: {
      free(t->string_literal);
      t->string_literal = nullptr;
    } break;

    default: {
    } break;
  }
}

static void set_token(TokenType type) {
  current_token.type = type;
}

static void set_token(TokenType type, int value) {
  current_token.type = type;
  current_token.integer_value = value;
}

static void set_token_identifier(TokenType type, char *name) {
  current_token.type = type;
  current_token.identifier_name = name;
}

static void set_token_literal(TokenType type, char *name) {
  current_token.type = type;
  current_token.string_literal = name;
}

static void set_end_token() {
  current_token.type = End;
}

static void end_processing() {
  puts("HERE");
  next_token = nullptr;
  cursor = nullptr;
  destruct(current_expr);
}

static int get_int(const char *&cursor) {
  assert(isdigit(*cursor));
  const char *copy = cursor;
  while(isdigit(*cursor)) {
    cursor++;
  }
  return atoi(copy);
}

 
// @Incomplete:
// Invalid read of size 1, if the given pattern doesn't end up with `"`.
static char *get_literal(const char *&cursor) {
  assert(*cursor == '\"');
  cursor++;
  char *ret = (char *)malloc(sizeof(char) * MAX_STRING_LITERAL_SIZE);

  int count = 0;
  while(*cursor != '\"') {
    ret[count++] = *cursor;
    cursor++;

    if(*cursor == '\0') { // Means there is no closing quote so cursor is '\0' last char.

      // @Incomplete:
      // If ret string is correct path to filename it will open file.
      return ret;
    }
  }
  cursor++;
  ret[count] = '\0';
  return ret;
}

static char *get_name(const char *&cursor) {
  assert(isalpha(*cursor));
  char *name = (char *)malloc(sizeof(char) * MAX_IDENTIFIER_NAME_SIZE);
  int count = 0;
  while(isalpha(*cursor)) { 
    name[count++] = *cursor;
    cursor++;
  }
  name[count] = '\0';
  return name;
}
/*
static void free_tokens() {
  int count = 0;
  Token *tok = &toks[count];
  while(tok->type != End) {
    tok->~Token();
    tok = &toks[++count];
  }
}
*/

static bool is_keyword(const char *keyword_name, int keyword_size, const char *cursor) {
  for(int i = 0; i < keyword_size ; i++) {
    if(cursor[i] != keyword_name[i]) {
      return false;
    }
  }
  return true;
}

void advance(const char *&cursor, int t) {
  for(int i = 0; i < t; i++) {
    cursor++;
  }
}


static Token *get_next_token() {
  if(!next_token) {
    while(*cursor != '\0') {
      char c = *cursor;
      if(isdigit(c)) {
        set_token(Number, get_int(cursor));
        return &current_token;

      } else if(c == '=') {
        cursor++;
        set_token(Assign);
        return &current_token;

      } else if(c == '\"') {
        set_token_literal(Literal, get_literal(cursor));
        return &current_token;

      } else if(is_keyword("set", sizeof("set")-1, cursor)) {
        set_token(Keyword_Set);
        advance(cursor, sizeof("set")-1);
        return &current_token;

      } else if(is_keyword("open", sizeof("open")-1, cursor)) {
        set_token(Keyword_Open);
        advance(cursor, sizeof("open")-1);
        return &current_token;
       
      } else if(isalpha(c)) {
        set_token_identifier(Identifier, get_name(cursor));
        return &current_token;

      } else {
        cursor++;
      }
    }
    set_end_token();
    return &current_token;
  } else {
    current_token = *next_token;
    next_token = nullptr;
    return &current_token;
  }
}

static Token *peek_next_token() {
  assert(!next_token);
  next_token = get_next_token();
  return next_token;
}

static AST_Expression *is_expression(Token *tok) {
  if(tok->type == Keyword_Set) {
    AST_Set *expr = (AST_Set *)malloc(sizeof(AST_Set));
    assert(expr);

    tok = peek_next_token();
    if(tok->type != Identifier) {
      report_error("Error: Expected identifier after `set` keyword\n");
      goto ast_set_failed;
    }
    assert(tok->type == Identifier);
    tok = get_next_token();

    expr->identifier_name = tok->identifier_name;

    tok = peek_next_token();
    if(tok->type != Assign) {
      report_error("Error: Expected `=` after indentifier\n");
      goto ast_set_failed;
    }
    assert(tok->type == Assign);
    tok = get_next_token();

    tok = peek_next_token();
    if(tok->type != Number) {
      report_error("Error: Expected constant in assign expression\n");
      goto ast_set_failed;
    }
    assert(tok->type == Number);
    tok = get_next_token();

    expr->integer_value = tok->integer_value;
    expr->type = AST_SetDirective;
    return expr;

ast_set_failed:
  destruct(tok);
  destruct(expr);
  return nullptr;

  } else if(tok->type == Keyword_Open) {
    AST_Open *expr = (AST_Open *)malloc(sizeof(AST_Open));
    assert(expr);

    tok = peek_next_token();
    if(tok->type != Literal) {
      report_error("Error: Expected string literal after `open` keyword\n");
      goto ast_open_failed;
    }
    assert(tok->type == Literal);
    tok = get_next_token();

    expr->string_literal = tok->string_literal;
    expr->type = AST_OpenDirective;
    return expr;

ast_open_failed:
  destruct(tok);
  destruct(expr);
  return nullptr;

  } else {
    report_error("Error: Expression doesn't start with keyword `set` | `open`\n");
    return nullptr;
  }
}

static bool parse() {
  Token *tok = get_next_token();
  if(tok->type == End) {
    return false;
  }

  AST_Expression *expr = is_expression(tok);
  current_expr = expr;

  if(!expr) {
    report_error("Parsing failed, Aborting.\n");
    return false;
  }


  // Interpret AST.
  if(expr->type == AST_SetDirective) {
    set_directive(static_cast<AST_Set *>(expr));
    return true;

  } else if(expr->type == AST_OpenDirective) {

    // @Incomplete:
    // Handle directoeries & empty literals.
    open_directive(static_cast<AST_Open *>(expr));
    return true;

  } else {
    report_error("Parsing failed, Aborting.\n");
    return false;
  }
}

#define set_interpreted(variable_name) variable_name = map[#variable_name]
static void update_settings() {
  // All possible values must be here.
  set_interpreted(tabstop);
  set_interpreted(Width);
  set_interpreted(Height);

  // @Temporary:
  // Need better way to detect whether update all of variables or not.
  SDL_SetWindowSize(get_win(), Width, Height);
}

void exec_command(const char *s) {
  cursor = s;
  if(parse()) {
    update_settings();
  }
  end_processing();
}
 

// @Incomplete:
// Need to load entire settings_file to c_string,
// and then interpret it. Now it interprets each string separately.
static void interp_settings_file() {
  const char *filename = "settings.txt";
  FILE *settings = fopen(filename, "r");
  if(!settings) {
    char msg[128];
    sprintf(msg, "Error opening file: <%s>.\n", filename);
    report_error(msg);
    return;
  }

  unsigned buffer_length = 256;
  char tmp[buffer_length];
  while(fgets(tmp, buffer_length, settings)) {
    exec_command(tmp);
  }
}

static bool settings_out_of_date = true;

void interp_settings() {
  if(settings_out_of_date) {
    interp_settings_file();

    SDL_SetWindowSize(get_win(), Width, Height);
    settings_out_of_date = false;
  }
}
