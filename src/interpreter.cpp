#include "gamma/pch.h"
#include "gamma/interpreter.h"
#include "gamma/globals.h"


/*
  The only handled expression by this programm is:
  `set identifier = value`
    where identifier already exists and value is constant of type `int`.
*/


enum TokenType {
  Number,
  Keyword_Set,
  Identifier,
  Assign,
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


static bool is_set_keyword(const char *c) {
  assert(*c == 's');
  return (c[1] == 'e') && (c[2] == 't');
}

static void lex(const char *s) {
  const char *cursor = s;

  while(*cursor != '\0') {
    if(isdigit(*cursor)) {
      set_token(Number, atoi(cursor));
      cursor++;

    } else if(*cursor == '=') {
      cursor++;
      set_token(Assign);

    } else if(*cursor == 's') {
      if(is_set_keyword(cursor)) {
        set_token(Keyword_Set);
        cursor++; // times strlen("set");
        cursor++;
        cursor++;

      } else {
        set_token(Identifier, get_name(cursor));
      }

    } else if(isalpha(*cursor)) {
      set_token(Identifier, get_name(cursor));

    } else {
      cursor++;
    }
  }
  set_end_token();
}

static bool is_expression(Token *tok) {
  // Expression of type: 
  //   set identifier = value;
  if(tok->type != Keyword_Set) {
    report_error("Error: Expression doesn't start with keyword `set`\n");
    return false;
  }
  assert(tok->type == Keyword_Set);
  tok = peek_token(0);
  if(tok->type != Identifier) {
    report_error("Error: Expected identifier after `set` keyword\n");
    return false;
  }
  assert(tok->type == Identifier);
  tok = peek_token(1);
  if(tok->type != Assign) {
    report_error("Error: Expected `=` after indentifier\n");
    return false;
  }
  assert(tok->type == Assign);
  tok = peek_token(2);
  if(tok->type != Number) {
    report_error("Error: Expected constant in assign expression\n");
    return false;
  }
  assert(tok->type == Number);
  return true;
}

static void parse() {
  Token *tok = get_next_token();

  bool success = is_expression(tok);
  if(!success) {
    report_error("Parsing failed, Aborting.\n");
    exit(0);
  }

  // Here we know that next n_tokens_to_next_expression tokens could
  // be grouped up to struct Ast_Assign.
  // But, we will just use them to put into `map`.

  Token *ident = peek_token(0);
  Token *value = peek_token(2);

  map[ident->identifier_name] = value->integer_value;
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
