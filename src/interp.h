#ifndef GAMMA_INTERPRETER_H
#define GAMMA_INTERPRETER_H

enum TokenType : u16 {
  TOKEN_IDENT        = 256,
  TOKEN_END_OF_INPUT = 257,

  TOKEN_SIGNED_NUMBER         = 258,
  TOKEN_FLOATING_POINT_NUMBER = 259,

  TOKEN_NUMBER  = 261,
  TOKEN_BOOLEAN = 260,

  TOKEN_IF_STATEMENT    = 400,
  TOKEN_ELSE_STATEMENT  = 401,
  TOKEN_FOR_STATEMENT   = 402,
  TOKEN_WHILE_STATEMENT = 403,

  TOKEN_DOUBLE_EQUALS     = 500,
  TOKEN_NOT_EQUALS        = 501,
  TOKEN_LOGICAL_OR        = 502,
  TOKEN_LOGICAL_AND       = 503,
  TOKEN_GREATER_OR_EQUALS = 515,
  TOKEN_LESS_OR_EQUALS    = 516,

  TOKEN_INCREMENT   = 504,
  TOKEN_DECREMENT   = 505,
  TOKEN_ADD_ASSIGN  = 506,
  TOKEN_SUB_ASSIGN  = 507,
  TOKEN_MUL_ASSIGN  = 508,
  TOKEN_DIV_ASSIGN  = 509,
  TOKEN_XOR_ASSIGN  = 510,
  TOKEN_AND_ASSIGN  = 511,
  TOKEN_OR_ASSIGN   = 512,
  TOKEN_RIGHT_SHIFT = 513,
  TOKEN_LEFT_SHIFT  = 514,

  TOKEN_SINGLE_CHARACTER = 515,
  TOKEN_STRING_LITERAL   = 516,

  TOKEN_CONST_KEYWORD = 517,
  TOKEN_STATIC_KEYWORD = 518,
  TOKEN_END_COMMAND = 519,
  TOKEN_SYNTAX_COMMAND = 520,
  TOKEN_SAVE_COMMAND = 521,


  TOKEN_STRUCT_DECLARATION = 665,
  TOKEN_ERROR = 666,
  TOKEN_SINGLE_LINE_COMMENT = 667,
  TOKEN_MULTI_LINE_COMMENT = 668,
};

#define rvalue_type \
  union { \
    bool bool_; \
    s64  s64_; \
    f64  f64_; \
  };


struct Var {
  TokenType type = TOKEN_END_OF_INPUT;
  union {
    bool bool_;
    s64  f64_;
    f64  s64_;
    const char *string_;
  };
};

struct Token {
  TokenType type = TOKEN_END_OF_INPUT;

  Var var;
  literal string_literal;

  int l, c;
};



enum Comment_Helper : u8 {
  COMMENT_UNINITIALIZED,
  COMMENT_SINGLE_LINE,
  COMMENT_MULTI_LINE,
};

struct Keyword_Def {
  literal name;
  TokenType type;
};

struct Language_Syntax_Struct {
  array<Keyword_Def>     keywords;

  array<string>    names;
  array<SDL_Color> colors;

  bool defined_color_for_literals = false;
  SDL_Color color_for_literals;

  bool defined_color_for_strings = false;
  SDL_Color color_for_strings;

  bool      tokenize_comments = false;
  string    single_line_comment;
  SDL_Color color_for_comments;

  string start_multi_line;
  string end_multi_line;
};

struct Syntax_Settings {
  array<Language_Syntax_Struct> base;

  array<string>                  extensions;
  array<Language_Syntax_Struct*> syntax;
};


struct Lexer {
  array<Token> tokens;
  size_t current_token_index = 0;

  array<Keyword_Def> keywords_table;

  bool    tokenize_comments = false;
  literal single_line_comment;
  literal start_multi_line;
  literal end_multi_line;

  

  Token *peek_token(s32);
  void   eat_token(s32);
  Token *peek_than_eat_token(s32);

  const Keyword_Def *maybe_get_keyword(const char *);
  bool is_comment(const char *, Comment_Helper *);
  void process_input(const char *);
};


void interp(const char *);
void interp_single_command(const char *);

Language_Syntax_Struct *get_language_syntax(const literal &);

void init_var_table();
void update_variables();

#endif
