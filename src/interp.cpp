#include "pch.h"
#include "interp.h"
#include "buffer.h"
#include "console.h"


inline bool ensure_space(const char *c, size_t n) {
  for(size_t i = 0; i < n; i++) {
    if(*c == '\0') { return false; }
    ++c;
  }
  return true;
}

inline void INC(const char *&c, s32 &nl, s32 &nc) { // these are references for no reason!
  if(*c == '\n') {
    ++nl;
    nc = 0;
  } else {
    ++nc;
  }
  assert(*c != '\0');
  ++c;
}

inline void ADVANCE(const char *&c, size_t n, s32 &nl, s32 &nc) {
  assert(ensure_space(c, n));
  for(size_t i = 0; i < n; i++) { INC(c, nl, nc); }
}



static const u16 MAX_ERROR_STRING_SIZE = 2048;

void report_error(const char *fmt, va_list args) {
  char r[MAX_ERROR_STRING_SIZE] = {0};
  vsnprintf(r, MAX_ERROR_STRING_SIZE, fmt, args);
  console_put_text(r);
}

void report_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char r[MAX_ERROR_STRING_SIZE] = {0};
  vsnprintf(r, MAX_ERROR_STRING_SIZE, fmt, args);
  console_put_text(r);
  va_end(args);
}

static void report_lexer_error(Token *tok, const char *fmt, ...) {
  tok->type = TOKEN_ERROR;
  va_list args;
  va_start(args, fmt);
  report_error(fmt, args);
  va_end(args);
}

static void report_lexer_error(Token *tok, const char *fmt, va_list args) {
  tok->type = TOKEN_ERROR;
  report_error(fmt, args);
}


static void report_parser_error(const Token *tok, const char *fmt, ...) {
  if(tok->type == TOKEN_ERROR) {
  } else {
    va_list args;
    va_start(args, fmt);
    report_error(fmt, args);
    va_end(args);
  }
}

static void report_parser_error(const Token *tok, const char *fmt, va_list args) {
  if(tok->type == TOKEN_ERROR) {
  } else {
    report_error(fmt, args);
  }
}

static bool check_token(const Token *tok, TokenType t, const char *fmt, ...) {
  if(tok->type != t) {
    va_list args;
    va_start(args, fmt);
    report_parser_error(tok, fmt, args);
    va_end(args);
    return false;
  }
  return true;
}
#define REPORT_ERROR_IF_NOT_TOKEN(tok, t, ...) \
  if(!check_token(tok, (TokenType)t, __VA_ARGS__)) { return; }

#define REPORT_ERROR_IF_NOT_TOKEN_C(tok, t, ...) \
  if(!check_token(tok, (TokenType)t, __VA_ARGS__)) { continue; }



template<class T>
static const char* name_from_tok(T t) { // supposed to be called with char or TokenType.
  switch(t) {
    case '(' : return "(";
    case ')' : return ")";
    case ',' : return ",";
    case ':' : return ":";
    case '/' : return "/";
    case TOKEN_NUMBER :         return "number";
    case TOKEN_STRING_LITERAL : return "string literal";
    case TOKEN_IDENT  :         return "identifier";
    default : return "(unrecognized)"; // @Incomplete: Handle all types.
  }
}

static const char* type_from_tok(TokenType t) {
  switch(t) {
    case TOKEN_NUMBER  : return "int";
    case TOKEN_BOOLEAN : return "bool";
    case TOKEN_STRING_LITERAL : return "string literal";
    case TOKEN_COLOR :   return "color"; // @ReName:
    default : return "(unrecognized)"; // @Incomplete: Handle all types.
  }
}





#define push_to_table(val, member, type_t) \
  { \
    Var r; \
    r.type   = (type_t); \
    r.member = (val); \
    literal l = to_literal(#val); \
    attach_table.literals.add(l); \
    attach_table.rvalues .add(r); \
  }

#define push_int(val)    push_to_table(val, s64_, TOKEN_NUMBER)
#define push_bool(val)   push_to_table(val, bool_, TOKEN_BOOLEAN)
#define push_string(val) push_to_table(val, string_, TOKEN_STRING_LITERAL)
#define push_color(val)  push_to_table(val, color_, TOKEN_COLOR) // @ReName: @Vector: 
  

struct Hotloaded_Variables { // @Speed: Hashtable.
  array<literal> literals;
  array<Var>     rvalues;
};
static Hotloaded_Variables attach_table = {};

static void attach_value(bool *val, literal name) {
  size_t index;
  if(attach_table.literals.find(name, &index)) {
    Var *r = &attach_table.rvalues[index];
    if(r->type == TOKEN_BOOLEAN) {
      *val = r->bool_;
    } else {
      static_string_from_literal(s, name);
      report_error("Error: `%s` expected to have type boolean, but got %s.\n", s, type_from_tok(r->type));
    }
  } else {
    assert(0); // @ShouldNeverHapen: This error must be caught in parser. 
  }
}

static void attach_value(int *val, literal name) {
  size_t index;
  if(attach_table.literals.find(name, &index)) {
    Var *r = &attach_table.rvalues[index];
    if(r->type == TOKEN_NUMBER) {
      *val = r->s64_;
    } else {
      static_string_from_literal(s, name);
      report_error("Error: `%s` expected to have type int, but got %s.\n", s, type_from_tok(r->type));
    }
  } else {
    assert(0); // @ShouldNeverHappen:
  }
}

static void attach_value(const char **val, literal name) {
  size_t index;
  if(attach_table.literals.find(name, &index)) {
    Var *r = &attach_table.rvalues[index];
    if(r->type == TOKEN_STRING_LITERAL) {
      *val = r->string_; // @MemoryLeak:
    } else {
      static_string_from_literal(s, name);
      report_error("Error: `%s` expected to have type string literal, but got %s.\n", s, type_from_tok(r->type));
    }
  } else {
    assert(0); // @ShouldNeverHappen:
  }
}

static void attach_value(SDL_Color *val, literal name) {
  size_t index;
  

  if(attach_table.literals.find(name, &index)) {
    Var *r = &attach_table.rvalues[index];
    if(r->type == TOKEN_COLOR) { // @ReName:
      *val = r->color_;
    } else {
      static_string_from_literal(s, name);
      report_error("Error: `%s` expected to have type color, but got %s.\n", s, type_from_tok(r->type));
    }
  } else {
    assert(0); // @ShouldNeverHappen:
  }
}


void init_variable_table() {
  push_bool(show_fps);
  push_string(font_name);
  push_int(font_size);
  push_color(text_color);
  push_color(background_color);
  push_color(cursor_text_color);
  push_color(cursor_color);
  push_color(console_text_color);
  push_color(console_color);
  push_color(searched_text_color);
  push_color(searched_color);
}

#define attach_from_table(name) attach_value(&name, to_literal(#name))
void update_variables() {
  attach_from_table(show_fps);
  attach_from_table(font_name);
  attach_value((int*)&font_size, to_literal("font_size"));
  attach_from_table(text_color);
  attach_from_table(background_color);
  attach_from_table(cursor_text_color);
  attach_from_table(cursor_color);
  attach_from_table(console_text_color);
  attach_from_table(console_color);
  attach_from_table(searched_text_color);
  attach_from_table(searched_color);
}




Token *Lexer::peek_token(s32 i = 0)  {
  return &tokens[current_token_index + i];
}

void Lexer::eat_token(s32 i = 0) {
  current_token_index += i+1;
}

Token *Lexer::peek_than_eat_token(s32 i = 0) {
  Token *tok = peek_token(i);
  eat_token(i);
  return tok;
}


const Keyword_Def *Lexer::maybe_get_keyword(const char *c) {
  for(auto &keyword : keywords_table) {
    if(ensure_space(c, keyword.name.size) && c == keyword.name) {
      c += keyword.name.size;

      if(!isalpha(*c) && *c != '_' && !isdigit(*c)) {
        return &keyword;
      } else {
        continue;
      }
    }
  }
  return NULL;
}

bool Lexer::is_comment(const char *c, Comment_Helper *comment) {
  if(tokenize_comments) { // user defined comments.
    if(single_line_comment.size > start_multi_line.size) {
      if(c == single_line_comment) {
        *comment = COMMENT_SINGLE_LINE;
        return true;
      } else if(c == start_multi_line) {
        *comment = COMMENT_MULTI_LINE;
        return true;
      } else {
        return false;
      }
    } else {
      if(c == start_multi_line) {
        *comment = COMMENT_MULTI_LINE;
        return true;
      } else if(c == single_line_comment) {
        *comment = COMMENT_SINGLE_LINE;
        return true;
      } else {
        return false;
      }
    }
  } else {
    if(*c == '#') { // our comments.
      *comment = COMMENT_SINGLE_LINE;
      return true;
    } else if(c == to_literal("---")) {
      *comment = COMMENT_MULTI_LINE;
      return true;
    } else {
      return false;
    }
  }
}


void Lexer::process_input(const char *cursor) {
  assert(cursor);

  s32 nline = 0, nchar = 0;
  Comment_Helper comment_type = COMMENT_UNINITIALIZED;

  while(*cursor != '\0') {
    Token tok;
    tok.l = nline;
    tok.c = nchar;

    if(const Keyword_Def *k = maybe_get_keyword(cursor)) {
      // Keyword token.
      ADVANCE(cursor, k->name.size, nline, nchar);
      tok.string_literal = k->name;
      tok.type           = k->type;
      tokens.add(tok);
      //

    } else if(*cursor == '\"' || *cursor == '\'') {
      char stop = *cursor;
      INC(cursor, nline, nchar);

      const char *tmp = cursor;
      while(*cursor != '\0') {
        if(*cursor == stop) {
          break;
        } else if(*cursor == '\\') {
          INC(cursor, nline, nchar);
          if(one_of(*cursor, "\"\'\\")) {
            INC(cursor, nline, nchar);
          }
        } else {
          INC(cursor, nline, nchar);
        }
      }
      if(*cursor != stop) { report_lexer_error(&tok, "Error: expected string literal to end up with %c, but got null terminator.\n", stop); tokens.add(tok); continue; } // @NoErrorOnSourceCode: when parsing source code this shouldn't report any error.
      tok.type           = TOKEN_STRING_LITERAL;
      tok.string_literal = to_literal(tmp, cursor-tmp);
      tokens.add(tok);
      INC(cursor, nline, nchar);
      // 

    } else if(cursor == to_literal("true") || cursor == to_literal("false")) {
      // Bool token.
      tok.type = TOKEN_BOOLEAN;
      if(cursor[0] == 't') {
        assert(cursor == to_literal("true"));
        ADVANCE(cursor, 4, nline, nchar);
        tok.var.type  = TOKEN_BOOLEAN;
        tok.var.bool_ = true;
        tok.string_literal = to_literal("true");

      } else if(cursor[0] == 'f') {
        assert(cursor == to_literal("false"));
        ADVANCE(cursor, 5, nline, nchar);
        tok.var.type  = TOKEN_BOOLEAN;
        tok.var.bool_ = false;
        tok.string_literal = to_literal("false");

      } else {
        assert(0);
      }
      tokens.add(tok);
      // 
  
    } else if(isalpha(*cursor) || *cursor == '_') {
      // Identifier token.
      const char *tmp = cursor;
      size_t count = 1;
      INC(cursor, nline, nchar);
      while(isalpha(*cursor) || *cursor == '_' || isdigit(*cursor)) {
        INC(cursor, nline, nchar);
        count++;
      }

      tok.type           = TOKEN_IDENT;
      tok.string_literal = to_literal(tmp, count);
      tokens.add(tok);
      //

    } else if(isdigit(*cursor) || *cursor == '.') {
      // Number token.
      const char *tmp = cursor;

      size_t count = 0;
      bool is_floating_point_number = false;

      if(*cursor == '.') {
        is_floating_point_number = true;
        count++;
        INC(cursor, nline, nchar);

        if(!isdigit(*cursor)) { continue; } // `.` is not a number.

      } else {
        count++;
        INC(cursor, nline, nchar);
      }

      while(1) {
        if(isdigit(*cursor)) {
          count++;
          INC(cursor, nline, nchar);

        } else if(*cursor == '.') {
          if(is_floating_point_number) {
            //exit_lexer_with_error("Too many decimal points in a number\n"); @Incomplete:
            tokens.add(tok);
          } else {
            is_floating_point_number = true;
          }
          count++;
          INC(cursor, nline, nchar);

        } else {
          break;
        }
      }

      tok.type           = TOKEN_NUMBER;
      tok.string_literal = to_literal(tmp, count);

      if(is_floating_point_number) {
        tok.var.f64_ = atof(tmp);
        tok.var.type = TOKEN_FLOATING_POINT_NUMBER;
      } else {
        tok.var.s64_ = atoi(tmp);
        tok.var.type = TOKEN_SIGNED_NUMBER;
      }
      tokens.add(tok);
      //
    

    } else if(is_comment(cursor, &comment_type)) { // @Speed: we can pass tok.type that use TOKEN_SINGLE_LINE_COMMENT & TOKEN_MULTI_LINE_COMMENT.

      // Comment.
      if(comment_type == COMMENT_SINGLE_LINE) {
        if(tokenize_comments) { // user defined comments.
          const char *tmp = cursor;
          ADVANCE(cursor, single_line_comment.size, nline, nchar);
          while(*cursor != '\0') {
            if     (cursor  == to_literal("\\\n")) { INC(cursor, nline, nchar); }
            else if(*cursor == '\n')            { break; }
            INC(cursor, nline, nchar);
          }
          tok.type           = TOKEN_SINGLE_LINE_COMMENT;
          tok.string_literal = to_literal(tmp, cursor-tmp);
          tokens.add(tok);
          INC(cursor, nline, nchar);

        } else {  // syntax.m `#` comments.
          INC(cursor, nline, nchar);
          while(*cursor != '\0') {
            if(*cursor == '\n') { break; }
            INC(cursor, nline, nchar);
          }
          INC(cursor, nline, nchar);
        }

      } else if(comment_type == COMMENT_MULTI_LINE) {
        if(tokenize_comments) { // user defined comments.
          const char *tmp = cursor;
          ADVANCE(cursor, start_multi_line.size, nline, nchar);
          s32 depth = 1;
          while(*cursor != '\0') {
            if(depth == 0) { break; }
            INC(cursor, nline, nchar);
            if     (cursor == end_multi_line)   { depth--; }
            else if(cursor == start_multi_line) { depth++; }
          }
          // @EnsureSpace:
          if(ensure_space(cursor, end_multi_line.size)) {
            ADVANCE(cursor, end_multi_line.size, nline, nchar);
          }
          tok.type           = TOKEN_MULTI_LINE_COMMENT;
          tok.string_literal = to_literal(tmp, cursor-tmp);
          tokens.add(tok);

        } else { // our comments.
          literal l = to_literal("---");
          ADVANCE(cursor, l.size, nline, nchar);
          while(*cursor != '\0') {
            if(cursor == l) { break; }
            INC(cursor, nline, nchar);
          }
          // @EnsureSpace:
          if(ensure_space(cursor, l.size)) {
            ADVANCE(cursor, l.size, nline, nchar);
          }
        }
      } else {
        assert(0);
      }
      // 

    } else if(one_of(*cursor, "():/,")) {
      tok.string_literal = to_literal(cursor, 1);
      tok.type           = (TokenType)*cursor;
      INC(cursor, nline, nchar);
      tokens.add(tok);

    } else {
      INC(cursor, nline, nchar);
    }
  }

  assert(*cursor == '\0');
  Token tok;
  tok.type = TOKEN_END_OF_INPUT;
  tokens.add(tok);
}


static Syntax_Settings settings = {};
static size_t keyword_count = 1000; // @ResetOnFileChange: every time we change a syntax file this should be 1000.


Language_Syntax_Struct* get_language_syntax(literal file_extension) {
  if(file_extension.size) {
    size_t index;
    bool found = settings.extensions.find(file_extension, &index);
    return found ? settings.syntax[index] : NULL;
  } else {
    return NULL;
  }
}

static bool end_of_block(Lexer &lexer) {
  return lexer.peek_token()->type == ':' && lexer.peek_token(1)->string_literal == "end"; // @Incomplete: ensure_space(1)
}

static void attach_var_if_found(literal ident, Var var) {
  size_t index;
  if(attach_table.literals.find(ident, &index)) {
    attach_table.rvalues[index] = var;
  } else {
    static_string_from_literal(s, ident);
    report_error("Error: variable `%s` is not defined.\n", s);
  }
}

static void parse_color(Lexer &lexer, SDL_Color *c) {
  Token *tok = lexer.peek_than_eat_token();
  REPORT_ERROR_IF_NOT_TOKEN(tok, '(', "Error: expected `%s` in a color expression.\n", name_from_tok('('));
  
  tok = lexer.peek_than_eat_token();
  REPORT_ERROR_IF_NOT_TOKEN(tok, TOKEN_NUMBER, "Error: expected `%s` in a color expression.\n", name_from_tok(TOKEN_NUMBER));
  c->r = tok->var.s64_;

  tok = lexer.peek_than_eat_token();
  REPORT_ERROR_IF_NOT_TOKEN(tok, ',', "Error: expected `%s` in a color expression.\n", name_from_tok(','));

  tok = lexer.peek_than_eat_token();
  REPORT_ERROR_IF_NOT_TOKEN(tok, TOKEN_NUMBER, "Error: expected `%s` in a color expression.\n", name_from_tok(TOKEN_NUMBER));
  c->g = tok->var.s64_;

  tok = lexer.peek_than_eat_token();
  REPORT_ERROR_IF_NOT_TOKEN(tok, ',', "Error: expected `%s` in a color expression.\n", name_from_tok(','));

  tok = lexer.peek_than_eat_token();
  REPORT_ERROR_IF_NOT_TOKEN(tok, TOKEN_NUMBER, "Error: expected `%s` in a color expression.\n", name_from_tok(TOKEN_NUMBER));
  c->b = tok->var.s64_;

  tok = lexer.peek_than_eat_token();
  if(tok->type == ')') {
    c->a = 255;
  } else if(tok->type == ',') {
    tok = lexer.peek_than_eat_token();
    REPORT_ERROR_IF_NOT_TOKEN(tok, TOKEN_NUMBER, "Error: expected `%s` in a color expression.\n", name_from_tok(TOKEN_NUMBER));

    c->a = tok->var.s64_;
    tok  = lexer.peek_than_eat_token();

    REPORT_ERROR_IF_NOT_TOKEN(tok, ')', "Error: expected `%s` in a color expression.\n", name_from_tok(')'));
  } else {
    report_parser_error(tok, "Error: expected `%s` in a color expression.\n", name_from_tok(')'));
  }
}

static void parse_syntax_command(Lexer &lexer, Language_Syntax_Struct *syntax) {
  while(lexer.peek_token()->type == TOKEN_STRING_LITERAL) {
    Token *tok = lexer.peek_than_eat_token();
    settings.extensions.add(to_string(tok->string_literal));
    settings.syntax    .add(syntax);
  }

  while(!end_of_block(lexer)) {
    Token *tok = lexer.peek_than_eat_token();
    if(tok->type == ':') {
      tok = lexer.peek_than_eat_token();
      if(tok->string_literal == to_literal("literal")) {
        syntax->defined_color_for_literals = true;
        auto color = &syntax->color_for_literals;
        parse_color(lexer, color);

      } else if(tok->string_literal == to_literal("string")) {
        syntax->defined_color_for_strings = true;
        auto color = &syntax->color_for_strings;
        parse_color(lexer, color);

      } else if(tok->string_literal == to_literal("single_line_comment")) {
        tok = lexer.peek_than_eat_token();
        REPORT_ERROR_IF_NOT_TOKEN_C(tok, TOKEN_STRING_LITERAL, "Error: expected string literal after `single_line_comment` command, but got `%s`.\n", type_from_tok(tok->type));

        syntax->tokenize_comments   = true;
        syntax->single_line_comment = to_string(tok->string_literal);

        auto color = &syntax->color_for_comments;
        parse_color(lexer, color);

      } else if(tok->string_literal == to_literal("multi_line_comment")) {
        tok = lexer.peek_than_eat_token();
        REPORT_ERROR_IF_NOT_TOKEN_C(tok, TOKEN_STRING_LITERAL, "Error: expected string literal after `multi_line_comment` command, but got `%s`.\n", type_from_tok(tok->type));

        syntax->tokenize_comments = true;
        syntax->start_multi_line  = to_string(tok->string_literal);

        tok = lexer.peek_token();
        if(tok->type == TOKEN_STRING_LITERAL) { // optionally parse end_multi_line.
          lexer.eat_token();
          syntax->end_multi_line = to_string(tok->string_literal);
        } else {
          syntax->end_multi_line = syntax->start_multi_line;
        }
      } else {
        static_string_from_literal(s, tok->string_literal);
        report_parser_error(tok, "Error: unexpected command `%s` inside of syntax block.\n", s);
      }

    } else if(tok->type == TOKEN_IDENT) {
      string      *name  = syntax->names.add();
      Keyword_Def *k     = syntax->keywords.add();
      SDL_Color   *color = syntax->colors.add();

      *name = to_string(tok->string_literal);

      k->name = to_literal(*name);
      k->type = (TokenType)keyword_count++;

      parse_color(lexer, color);

    } else {
      report_parser_error(tok, "Error: unexpected expression `%s` inside of syntax block.\n", name_from_tok(tok->type));
    }
  }
  lexer.eat_token();
  lexer.eat_token();
}

static bool parse_top_level(Lexer &lexer) {
  Token *tok = lexer.peek_than_eat_token();
  if(tok->type == ':') {
    // Commands.
    tok = lexer.peek_than_eat_token();
    if(tok->string_literal == "syntax") {
      Language_Syntax_Struct *syntax = settings.base.add();
      parse_syntax_command(lexer, syntax);

    } else {
      static_string_from_literal(s, tok->string_literal);
      report_parser_error(tok, "Error: `%s` is not a defined command for settings file.\n", s);
    }
    // 

  } else if(tok->type == TOKEN_IDENT) {
    literal ident = tok->string_literal;
    Var var;

    tok = lexer.peek_token();
    if(tok->type == TOKEN_STRING_LITERAL) {
      lexer.eat_token();
      var.type    = tok->type;
      var.string_ = dynamic_string_from_literal(tok->string_literal);

    } else if(tok->type == TOKEN_NUMBER) {
      lexer.eat_token();
      var.type = tok->type;
      var.s64_ = tok->var.s64_;

    } else if(tok->type == TOKEN_BOOLEAN) {
      lexer.eat_token();
      var.type  = tok->type;
      var.bool_ = tok->var.bool_;

    } else if(tok->type == '(') { // color
      var.type = TOKEN_COLOR; // @ReName: 
      parse_color(lexer, &var.color_);

    } else {
      report_error("Error: expected expression of type (%s, %s, %s, %s), but got `%s`.\n", type_from_tok(TOKEN_NUMBER), type_from_tok(TOKEN_BOOLEAN), type_from_tok(TOKEN_STRING_LITERAL), type_from_tok(TOKEN_COLOR), type_from_tok(tok->type));
      return lexer.peek_token()->type == TOKEN_END_OF_INPUT;
    }
    attach_var_if_found(ident, var);
    // Done.


  } else {
    report_parser_error(tok, "Error: expected ':' for command, or variable definition to hotload.\n");
  }

  return lexer.peek_token()->type == TOKEN_END_OF_INPUT;
}

void interp(const char *cursor) {
  if(!cursor) return;

  // @MemoryLeak: Every array, string, everything!!!
  for(auto &syntax : settings.base) {
    syntax.keywords.clear();
    syntax.names.clear();
    syntax.colors.clear();
  }
  settings.base.clear();
  settings.extensions.clear();
  settings.syntax.clear();
    
  Lexer lexer = {};
  lexer.process_input(cursor);

  while(!parse_top_level(lexer)) {}
}

static void parse_single_command(Lexer &lexer) {
  Token *tok = lexer.peek_than_eat_token();
  if(tok->string_literal == "save" || tok->string_literal == "w") { // @Speed: precomputed Hashtable?
    save();

  } else if(tok->string_literal == "quit" || tok->string_literal == "q" || tok->string_literal == "close_buffer") {
    close_buffer(get_current_tab());

  } else if(tok->string_literal == "tab") {
    tok = lexer.peek_than_eat_token(); // @Maybe: we don't need a TOKEN_STRING_LITERAL, just a TOKEN_IDENT.
    REPORT_ERROR_IF_NOT_TOKEN(tok, TOKEN_STRING_LITERAL, "Error: expected `%s` after `tab` command.\n", name_from_tok(TOKEN_STRING_LITERAL));
    open_new_tab(to_string(tok->string_literal));

  } else if(tok->string_literal == "change_tab") {
    tok = lexer.peek_than_eat_token();
    REPORT_ERROR_IF_NOT_TOKEN(tok, TOKEN_NUMBER, "Error: expected `%s` after `change_tab` command.\n", name_from_tok(TOKEN_NUMBER));
    change_tab(tok->var.s64_);

  } else if(tok->string_literal == "split" || tok->string_literal == "sp") {
    tok = lexer.peek_token();
    string s;
    if(tok->type == TOKEN_ERROR) {
      return;

    } else if(tok->type == TOKEN_STRING_LITERAL) {
      lexer.eat_token();
      s = to_string(tok->string_literal);

    } else {
      copy_string(&s, &get_current_buffer()->filename);
    }
    open_new_buffer(s);
    resize_tab(get_current_tab());

  } else if(tok->string_literal == "change_buffer") {
    tok = lexer.peek_than_eat_token();
    REPORT_ERROR_IF_NOT_TOKEN(tok, TOKEN_NUMBER, "Error: expected `%s` after `change_buffer` command.\n", name_from_tok(TOKEN_NUMBER));

    // @Hack: @CleanUp:
    // right = 0, left = 1
    // We can implement variables for a syntax.m file, than assign left = ..., etc.
    // Than we will be able to use this command as follows: `:change_buffer left`.
    // 
    change_buffer(tok->var.s64_);
 
  } else {
    static_string_from_literal(s, tok->string_literal);
    report_parser_error(tok, "Error: `%s` is not a defined command for interactive prompt.\n", s);
  }
}


static void parse_find_command(Lexer &lexer) {
  Token *tok = lexer.peek_than_eat_token();
  REPORT_ERROR_IF_NOT_TOKEN(tok, TOKEN_STRING_LITERAL, "Error: expected `%s` after `search` command.\n", name_from_tok(TOKEN_STRING_LITERAL));

  string search;
  search.data = (char*)tok->string_literal.data;
  search.size =        tok->string_literal.size;
  auto buffer = get_current_buffer();
	find_in_buffer(&buffer->search_component, &buffer->buffer_component, search);
}


void interp_single_command(const char *cursor) {
  if(!cursor) return;

  Lexer lexer = {};
  lexer.process_input(cursor);

  Token *tok = lexer.peek_than_eat_token();
  if(tok->type == ':') {
    parse_single_command(lexer);

  } else if(tok->type == '/') {
    parse_find_command(lexer);

  } else {
    report_parser_error(tok, "Error: expected `:` or `/` at the beginning of command.\n");
  }
}
