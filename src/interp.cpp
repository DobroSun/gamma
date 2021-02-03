#include "pch.h"
#include "interp.h"
#include "buffer.h"


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


inline bool ensure_space(const char *c, size_t n) {
  for(size_t i = 0; i < n; i++) {
    if(*c == '\0') { return false; }
    ++c;
  }
  return true;
}

inline void INC(const char *&c, s32 *nl, s32 *nc) {
  if(*c == '\n') {
    ++(*nl);
    *nc = 0;
  } else {
    ++(*nc);
  }
  assert(*c != '\0');
  ++c;
}

inline void ADVANCE(const char *&c, size_t n) {
  assert(ensure_space(c, n));
  c += n;
}

inline void ADVANCE(const char *&c, size_t n, s32 *nl, s32 *nc) {
  assert(ensure_space(c, n));
  for(size_t i = 0; i < n; i++) { INC(c, nl, nc); }
}

#define push_to_table(val, member, type_t) \
  { \
    Var r; \
    r.type   = (type_t); \
    r.member = (val); \
    literal l = (#val); \
    attach_table.literals.add(l); \
    attach_table.rvalues .add(r); \
  }

#define push_int(val)    push_to_table(val, s64_, TOKEN_NUMBER)
#define push_bool(val)   push_to_table(val, bool_, TOKEN_BOOLEAN)
#define push_string(val) push_to_table(val, string_, TOKEN_STRING_LITERAL)
  

struct Hotloaded_Variables { // @Speed: HashTable.
  array<literal> literals;
  array<Var>     rvalues;
};
static Hotloaded_Variables attach_table;

static void attach_value(bool *val, const literal &name) {
  literal *it; size_t index;
  attach_table.literals.find(name, &it, &index);

  if(it) {
    Var *r = &attach_table.rvalues[index];
    if(r->type == TOKEN_BOOLEAN) {
      *val = r->bool_;
    } else {
      assert(0); // @Incomplete: @ReportError:
    }
  } else {
    assert(0); // This error must be caught in parser. @ShouldNeverHapen:
  }
}

static void attach_value(int *val, const literal &name) {
  literal *it; size_t index;
  attach_table.literals.find(name, &it, &index);

  if(it) {
    Var *r = &attach_table.rvalues[index];
    if(r->type == TOKEN_NUMBER) {
      *val = r->s64_;
    } else {
      assert(0); // @Incomplete: @ReportError:
    }
  } else {
    assert(0); // @ShouldNeverHappen:
  }
}

static void attach_value(const char **val, const literal &name) {
  literal *it; size_t index;
  attach_table.literals.find(name, &it, &index);

  if(it) {
    Var *r = &attach_table.rvalues[index];
    if(r->type == TOKEN_STRING_LITERAL) {
      *val = r->string_; // @MemoryLeak:
    } else {
      assert(0); // @Incomplete: @ReportError:
    }
  } else {
    assert(0); // @ShouldNeverHappen:
  }
}

void init_var_table() {
  push_bool(show_fps);
  push_string(font_name);
  push_int(font_size);
}

#define attach_from_table(name) attach_value(&name, #name)
void update_variables() {
  attach_from_table(show_fps);
  attach_from_table(font_name);
  attach_value((int*)&font_size, "font_size");
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
    if(c == keyword.name) {
      auto tmp = c;
      if(!ensure_space(tmp, keyword.name.size)) { continue; } // @Should never be here, cause we alredy checked that `c` is a keyword. But we still go here and ADVANCE's assert fires up!

      ADVANCE(tmp, keyword.name.size);
      if(!isalpha(*tmp) && *tmp != '_' && !isdigit(*tmp)) {
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
    } else if(c == literal("---")) {
      *comment = COMMENT_MULTI_LINE;
      return true;
    } else {
      return false;
    }
  }
}


void Lexer::process_input(const char *cursor) {
  s32 nline = 0, nchar = 0;
  Comment_Helper comment_type = COMMENT_UNINITIALIZED;

  while(*cursor != '\0') {
    Token tok;
    tok.l = nline;
    tok.c = nchar;

    if(const Keyword_Def *k = maybe_get_keyword(cursor)) {
      // Keyword token.
      ADVANCE(cursor, k->name.size, &nline, &nchar);
      tok.string_literal = k->name;
      tok.type           = k->type;
      tokens.add(tok);
      //

    } else if(*cursor == '\"' || *cursor == '\'') {
      char stop = *cursor;
      INC(cursor, &nline, &nchar);

      const char *tmp = cursor;
      while(*cursor != '\0') {
        if(*cursor == stop) {
          break;
        } else if(*cursor == '\\') {
          INC(cursor, &nline, &nchar);
          if(is_one_of(*cursor, "\"\'\\")) {
            INC(cursor, &nline, &nchar);
          }
        } else {
          INC(cursor, &nline, &nchar);
        }
      }
      tok.type           = TOKEN_STRING_LITERAL;
      tok.string_literal = literal(tmp, cursor-tmp);
      tokens.add(tok);
      if(ensure_space(cursor, 1))  INC(cursor, &nline, &nchar);
      // 

    } else if(cursor == literal("true") || cursor == literal("false")) {
      // Bool token.
      tok.type = TOKEN_BOOLEAN;
      if(cursor[0] == 't') {
        assert(cursor == literal("true"));
        ADVANCE(cursor, 4, &nline, &nchar);
        tok.var.type  = TOKEN_BOOLEAN;
        tok.var.bool_ = true;
        tok.string_literal = "true";

      } else if(cursor[0] == 'f') {
        assert(cursor == literal("false"));
        ADVANCE(cursor, 5, &nline, &nchar);
        tok.var.type  = TOKEN_BOOLEAN;
        tok.var.bool_ = false;
        tok.string_literal = "false";

      } else {
        assert(0);
      }
      tokens.add(tok);
      // 
  
    } else if(isalpha(*cursor) || *cursor == '_') {
      // Identifier token.
      const char *tmp = cursor;
      size_t count = 1;
      INC(cursor, &nline, &nchar);
      while(isalpha(*cursor) || *cursor == '_' || isdigit(*cursor)) {
        INC(cursor, &nline, &nchar);
        count++;
      }

      tok.type           = TOKEN_IDENT;
      tok.string_literal = literal(tmp, count);
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
        INC(cursor, &nline, &nchar);

        if(!isdigit(*cursor)) {
          //exit_lexer_with_error("`.` is an operator. @Incomplete: Not handled yet!\n"); @Incomplete:
          tokens.add(tok);
        }
      } else {
        count++;
        INC(cursor, &nline, &nchar);
      }

      while(1) {
        if(isdigit(*cursor)) {
          count++;
          INC(cursor, &nline, &nchar);

        } else if(*cursor == '.') {
          if(is_floating_point_number) {
            //exit_lexer_with_error("Too many decimal points in a number\n"); @Incomplete:
            tokens.add(tok);
          } else {
            is_floating_point_number = true;
          }
          count++;
          INC(cursor, &nline, &nchar);

        } else {
          break;
        }
      }

      tok.type           = TOKEN_NUMBER;
      tok.string_literal = literal(tmp, count);

      if(is_floating_point_number) {
        tok.var.f64_ = atof(tmp);
        tok.var.type = TOKEN_FLOATING_POINT_NUMBER;
      } else {
        tok.var.s64_ = atoi(tmp);
        tok.var.type = TOKEN_SIGNED_NUMBER;
      }
      tokens.add(tok);
      //
    

    } else if(is_comment(cursor, &comment_type)) { // @Speed: we can pass tok.type that use TOKENSINGLE_LINE_COMMENT & TOKEN_MULTI_LINE_COMMENT.

      // Comment.
      if(comment_type == COMMENT_SINGLE_LINE) {
        if(tokenize_comments) { // user defined comments.
          const char *tmp = cursor;
          ADVANCE(cursor, single_line_comment.size, &nline, &nchar);
          while(*cursor != '\0') {
            if     (cursor  == literal("\\\n")) { INC(cursor, &nline, &nchar); }
            else if(*cursor == '\n')            { break; }
            INC(cursor, &nline, &nchar);
          }
          tok.type           = TOKEN_SINGLE_LINE_COMMENT;
          tok.string_literal = literal(tmp, cursor-tmp);
          tokens.add(tok);
          INC(cursor, &nline, &nchar);

        } else {
          INC(cursor, &nline, &nchar); // syntax.m `#` comments.
          while(*cursor != '\0') {
            if(*cursor == '\n') { break; }
            INC(cursor, &nline, &nchar);
          }
          INC(cursor, &nline, &nchar);
        }

      } else if(comment_type == COMMENT_MULTI_LINE) {
        if(tokenize_comments) { // user defined comments.
          const char *tmp = cursor;
          ADVANCE(cursor, start_multi_line.size, &nline, &nchar);
          s32 depth = 1;
          while(*cursor != '\0') {
            if(depth == 0) { break; }
            INC(cursor, &nline, &nchar);
            if     (cursor == end_multi_line)   { depth--; }
            else if(cursor == start_multi_line) { depth++; }
          }
          // @EnsureSpace:
          if(ensure_space(cursor, end_multi_line.size)) {
            ADVANCE(cursor, end_multi_line.size, &nline, &nchar);
          }
          tok.type           = TOKEN_MULTI_LINE_COMMENT;
          tok.string_literal = literal(tmp, cursor-tmp);
          tokens.add(tok);

        } else { // our comments.
          literal l = literal("---");
          ADVANCE(cursor, l.size, &nline, &nchar);
          while(*cursor != '\0') {
            if(cursor == l) { break; }
            INC(cursor, &nline, &nchar);
          }
          // @EnsureSpace:
          if(ensure_space(cursor, l.size)) {
            ADVANCE(cursor, l.size, &nline, &nchar);
          }
        }
      } else {
        assert(0);
      }
      // 

    } else if(is_one_of(*cursor, "():,")) {
      tok.string_literal = literal(cursor, 1);
      tok.type           = (TokenType)*cursor;
      INC(cursor, &nline, &nchar);
      tokens.add(tok);

    } else {
      INC(cursor, &nline, &nchar);
    }
  }

  assert(*cursor == '\0');
  Token tok;
  tok.type = TOKEN_END_OF_INPUT;
  tokens.add(tok);
}


static Syntax_Settings settings;
static size_t keyword_count = 1000; // @ResetOnFileChange: every time we change a syntax file this should be 1000.


Language_Syntax_Struct* get_language_syntax(const literal &file_extension) {
  if(file_extension.size) {
    string *iter; size_t index;
    settings.extensions.find(file_extension, &iter, &index);
    return iter ? settings.syntax[index] : NULL;
  } else {
    return NULL;
  }
}


static bool end_of_block(Lexer &lexer) {
  return lexer.peek_token()->type == ':' && lexer.peek_token(1)->string_literal == "end"; // @Incomplete: ensure_space(1)
}

static void parse_color(Lexer &lexer, u8 *r, u8 *g, u8 *b) {
  Token *tok = lexer.peek_than_eat_token();
  if(tok->type != '(') { assert(0); } // @ReportError.
  
  tok = lexer.peek_than_eat_token();
  if(tok->type != TOKEN_NUMBER) { assert(0); } // @ReportError.
  *r = tok->var.s64_;

  tok = lexer.peek_than_eat_token();
  if(tok->type != ',') { assert(0); } // @ReportError.

  tok = lexer.peek_than_eat_token();
  if(tok->type != TOKEN_NUMBER) { assert(0); } // @ReportError.
  *g = tok->var.s64_;

  tok = lexer.peek_than_eat_token();
  if(tok->type != ',') { assert(0); } // @ReportError.

  tok = lexer.peek_than_eat_token();
  if(tok->type != TOKEN_NUMBER) { assert(0); } // @ReportError.
  *b = tok->var.s64_;

  tok = lexer.peek_than_eat_token();
  if(tok->type != ')') { assert(0); } // @ReportError.
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
      if(tok->string_literal == literal("literal")) {
        syntax->defined_color_for_literals = true;
        auto &c = syntax->color_for_literals;
        parse_color(lexer, &c.r, &c.g, &c.b);

      } else if(tok->string_literal == literal("string")) {
        syntax->defined_color_for_strings = true;
        auto &c = syntax->color_for_strings;
        parse_color(lexer, &c.r, &c.g, &c.b);

      } else if(tok->string_literal == literal("single_line_comment")) {
        tok = lexer.peek_than_eat_token();
        if(tok->type != TOKEN_STRING_LITERAL) { assert(0); } // @ReportError.

        syntax->tokenize_comments   = true;
        syntax->single_line_comment = to_string(tok->string_literal);

        auto &c = syntax->color_for_comments;
        parse_color(lexer, &c.r, &c.g, &c.b);

      } else if(tok->string_literal == literal("multi_line_comment")) {
        tok = lexer.peek_than_eat_token();
        if(tok->type != TOKEN_STRING_LITERAL) { assert(0); } // @ReportError.

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
        assert(0); // @ReportError.
      }

    } else if(tok->type == TOKEN_IDENT) {
      Keyword_Def *k    = &syntax->keywords.add();
      string      *name = &syntax->names.add();
      SDL_Color   *c    = &syntax->colors.add();

      k->name = tok->string_literal;
      k->type = (TokenType)keyword_count++;

      *name = to_string(tok->string_literal);
      parse_color(lexer, &c->r, &c->g, &c->b);

    } else {
      assert(0); // @ReportError.
    }
  }
  lexer.eat_token();
  lexer.eat_token();
}

static void parse_save_command() {
  save();
}

static bool parse_top_level(Lexer &lexer) {
  Token *tok = lexer.peek_than_eat_token();
  if(tok->type == ':') {
    // Commands.
    tok = lexer.peek_than_eat_token();
    if(tok->string_literal == "syntax") {
      Language_Syntax_Struct *syntax = &settings.base.add();
      parse_syntax_command(lexer, syntax);

    } else if(tok->string_literal == "save" || tok->string_literal == "w") {
      parse_save_command();

    } else {
      assert(0); // @ReportError.
    }
    // 

  } else if(tok->type == TOKEN_IDENT) {
    literal ident = tok->string_literal;
    Var var;

    tok = lexer.peek_than_eat_token();
    if(tok->type == TOKEN_STRING_LITERAL) {
      var.type    = tok->type;
      var.string_ = dynamic_string_from_literal(tok->string_literal);

      literal *it; size_t index;
      attach_table.literals.find(ident, &it, &index);
      if(it) {
        attach_table.rvalues[index] = var;
      } else {
        assert(0); // @WrongVariableName: @ReportError.
      }

    } else if(tok->type == TOKEN_NUMBER) {
      var.type = tok->type;
      var.s64_ = tok->var.s64_;

      literal *it; size_t index; // @Copy&Paste:
      attach_table.literals.find(ident, &it, &index);
      if(it) {
        attach_table.rvalues[index] = var;
      } else {
        assert(0); // @WrongVariableName: @ReportError.
      }
    }

  } else {
    assert(0); // @ReportError.
  }



  return lexer.peek_token()->type == TOKEN_END_OF_INPUT;
}

void interp(const char *cursor) {
  // @MemoryLeak: Every array, string, everything!!!
  for(auto &syntax : settings.base) {
    syntax.keywords.clear();
    syntax.names.clear();
    syntax.colors.clear();
  }
  settings.base.clear();
  settings.extensions.clear();
  settings.syntax.clear();
    
  Lexer lexer;
  lexer.process_input(cursor);

  while(!parse_top_level(lexer)) {}
}

void interp_single_command(const char *cursor) {
  Lexer lexer;
  lexer.process_input(cursor);

  parse_top_level(lexer);
}
