#include "gamma/pch.h"
#include "gamma/interpreter.h"
#include "gamma/globals.h"
#include "gamma/view.h"
#include "gamma/buffer.h"


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

  ~Token() {
    if(type == Identifier) {
      free(identifier_name);
    }
  }
};


// @Incomplete:
// Add size checks in get_name & get_literal.
#define MAX_IDENTIFIER_NAME_SIZE 64
#define MAX_STRING_LITERAL_SIZE MAX_IDENTIFIER_NAME_SIZE 
#define MAX_TOKENS 100
#define pair(A) {#A, A}

// @Incomplete:
// Use array<Token>;
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

static void set_token_identifier(TokenType type, char *name) {
  Token *tok = &toks[current_token++];
  tok->type = type;
  tok->identifier_name = name;
}

static void set_token_literal(TokenType type, char *name) {
  Token *tok = &toks[current_token++];
  tok->type = type;
  tok->string_literal = name;
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

static int get_int(gap_buffer<char> &b, unsigned &cursor) {
  assert(isdigit(b[cursor]));
  int copy = cursor;
  while(isdigit(b[cursor])) {
    cursor++;
  }
  return atoi(&b[copy]);
}

 
// @Incomplete:
// Invalid read of size 1, if the given pattern doesn't end up with `"`.
static char *get_literal(gap_buffer<char> &b, unsigned &cursor) {
  assert(b[cursor] == '\"');
  cursor++;
  char *ret = (char *)malloc(sizeof(char) * MAX_STRING_LITERAL_SIZE);

  int count = 0;
  while(b[cursor] != '\"') {
    ret[count++] = b[cursor++];

    if(b[cursor] == '\0') {
      // Means there is no closing quote so cursor is '\0' last char.

      // @Incomplete:
      // If ret string is correct path to filename it will open file.
      return ret;
    }
  }
  cursor++;
  ret[count] = '\0';
  return ret;
}

static char *get_name(gap_buffer<char> &b, unsigned &cursor) {
  assert(isalpha(b[cursor]));
  char *name = (char *)malloc(sizeof(char) * MAX_IDENTIFIER_NAME_SIZE);
  int count = 0;
  while(isalpha(b[cursor])) { 
    name[count++] = b[cursor++];
  }
  name[count] = '\0';
  return name;
}

static void free_tokens() {
  int count = 0;
  Token *tok = &toks[count];
  while(tok->type != End) {
    tok->~Token();
    tok = &toks[++count];
  }
  current_token = 0;
}

static bool is_keyword(const char *keyword_name, int keyword_size, gap_buffer<char> &b, unsigned cursor) {
  for(int i = 0; i < keyword_size ; i++) {
    if(b[cursor+i] != keyword_name[i]) {
      return false;
    }
  }
  return true;
}

void advance(unsigned &cursor, int t) {
  for(int i = 0; i < t; i++) {
    cursor++;
  }
}

static void lex(gap_buffer<char> &b) {
  assert(b[b.size()-1] == ' ');
  auto last_index = b.size()-1;
  b[last_index] = '\0';

  unsigned cursor = 0;
  while(b[cursor] != '\0') {
    char c = b[cursor];
    if(isdigit(c)) {
      set_token(Number, get_int(b, cursor));

    } else if(c == '=') {
      cursor++;
      set_token(Assign);

    } else if(c == '\"') {
      set_token_literal(Literal, get_literal(b, cursor));

    } else if(is_keyword("set ", sizeof("set ")-1, b, cursor)) {
      set_token(Keyword_Set);
      advance(cursor, sizeof("set ")-1);

    } else if(is_keyword("open ", sizeof("open ")-1, b, cursor)) {
      set_token(Keyword_Open);
      advance(cursor, sizeof("open ")-1);
     
    } else if(isalpha(c)) {
      set_token_identifier(Identifier, get_name(b, cursor));

    } else {
      cursor++;
    }
  }
  set_end_token();
  b[last_index] = ' ';
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
    if(tok->type != Literal) {
      report_error("Error: Expected string literal after `open` keyword\n");
      return NULL;
    }
    assert(tok->type == Literal);
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
    // @Incomplete:
    // Handle directoeries & empty literals.
    auto &buffer = get_buffer();
    Token *file = peek_token(0);

    char *filename = file->string_literal;

    buffer.clear();
    bool success = load_buffer_from_file(filename);
    if(!success) {
      char msg[128];
      sprintf(msg, "Error opening file: <%s>.\n", filename);
      report_error(msg);
    }
    free(filename);

  } else {
  }
}

#define set_interpreted(variable_name) variable_name = map[#variable_name]
static void update_settings() {
  // All possible values must be here.
  set_interpreted(tabstop);
}

void exec_command(gap_buffer<char> &b) {
  lex(b);
  parse();

  update_settings();
  free_tokens();
}
