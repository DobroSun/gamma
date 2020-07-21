#include "gamma/pch.h"
#include "gamma/interpreter.h"
#include "gamma/globals.h"
#include "gamma/view.h"
#include "gamma/buffer.h"


enum TokenType {
  Assign,

  Number,
  Identifier,

  Keyword_Set,
  Keyword_Open,
  End
};

struct Token {
  TokenType type = End;
  union {
    char *identifier_name;
    int   integer_value;
  };

  ~Token() {
    if(type == Identifier) {
      free(identifier_name);
    }
  }
};



#define MAX_IDENTIFIER_NAME_SIZE 256
#define MAX_TOKENS 100
#define pair(A) {#A, A}
// @Temporary: Just use array<Token>;
static Token toks[MAX_TOKENS];
static int current_token = 0;


// Map of all modifyable variables from script(with their default values}.
static settings_map map = {
  pair(tabstop)
};



// @Incomplete: 
// Need another way to put error messages.
static void report_error(const char *msg) {
  fprintf(stderr, "%s", msg);
}

static void set_token(TokenType type) {
  Token *tok = &toks[current_token++];
  tok->type = type;
}

static void set_token(TokenType type, int value) {
  Token *tok = &toks[current_token++];
  tok->type = type;
  tok->integer_value = value;
}

static void set_token(TokenType type, char *name) {
  Token *tok = &toks[current_token++];
  tok->type = type;
  tok->identifier_name = name;
}

static void set_end_token() {
  toks[current_token].type = End;
  current_token = 0;
}

static Token *get_next_token() {
  return &toks[current_token++];
}

static Token *peek_token(int times) {
  return &toks[current_token+times];
}

static int get_int(const char *&cursor) {
  assert(isdigit(*cursor));
  const char *copy = cursor;
  while(isdigit(*cursor)) {
    cursor++;
  }
  return atoi(copy);
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

static void free_ident_names() {
  int count = 0;
  Token *tok = &toks[count];
  while(tok->type != End) {
    if(tok->type == Identifier) {
      free(tok->identifier_name);
    }
    tok = &toks[++count];
  }
  current_token = 0;
}

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

static void lex(const char *s) {
  const char *cursor = s;

  while(*cursor != '\0') {
    if(isdigit(*cursor)) {
      set_token(Number, get_int(cursor));

    } else if(*cursor == '=') {
      cursor++;
      set_token(Assign);

    } else if(is_keyword("set", sizeof("set")-1, cursor)) {
      set_token(Keyword_Set);
      advance(cursor, sizeof("set")-1);

    } else if(is_keyword("open", sizeof("open")-1, cursor)) {
      set_token(Keyword_Open);
      advance(cursor, sizeof("open")-1);

    } else if(isalpha(*cursor)) {
      set_token(Identifier, get_name(cursor));

    } else {
      cursor++;
    }
  }
  set_end_token();
}

static Token *is_expression(Token *tok) {
  Token *copy = tok;
  if(tok->type == Keyword_Set) {
    tok = peek_token(0);
    if(tok->type != Identifier) {
      report_error("Error: Expected identifier after `set` keyword\n");
      return NULL;
    }
    assert(tok->type == Identifier);
    tok = peek_token(1);
    if(tok->type != Assign) {
      report_error("Error: Expected `=` after indentifier\n");
      return NULL;
    }
    assert(tok->type == Assign);
    tok = peek_token(2);
    if(tok->type != Number) {
      report_error("Error: Expected constant in assign expression\n");
      return NULL;
    }
    assert(tok->type == Number);
    return copy;

  } else if(tok->type == Keyword_Open) {
    tok = peek_token(0);
    if(tok->type != Identifier) {
      report_error("Error: Expected identifier after `open` keyword\n");
      return NULL;
    }
    assert(tok->type == Identifier);
    return copy;

  } else {
    report_error("Error: Expression doesn't start with keyword `set` | `open`\n");
    return NULL;
  }
}

static void parse() {
  Token *tok = get_next_token();

  tok = is_expression(tok);
  if(!tok) {
    report_error("Parsing failed, Aborting.\n");
    exit(0);
  }

  // Interpret tokens.
  if(tok->type == Keyword_Set) {
    Token *variable = peek_token(0);
    Token *value    = peek_token(2);
    map[variable->identifier_name] = value->integer_value;

  } else if(tok->type == Keyword_Open) {
    auto &buffer = get_buffer();
    Token *file = peek_token(0);

    buffer.clear();
    bool success = load_buffer_from_file(file->identifier_name);
    if(!success) {
      fprintf(stderr, "Error opening file: \"%s\".\n", file->identifier_name);
    }

  } else {
  }
}

#define set_interpreted(variable_name) variable_name = map[#variable_name]
static void update_settings() {
  // All possible values must be here.
  set_interpreted(tabstop);
}

void exec_command(const char *s) {
  free_ident_names();

  lex(s);
  parse();
  update_settings();
}
