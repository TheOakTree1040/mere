#ifndef TOKEN_H
#define TOKEN_H

#include "object.h"
namespace mere {
	class Token{
		public:
			enum tok_type{
				invalid,

				// Characters.
				l_paren, r_paren, l_brace, r_brace, l_sq_brkt, r_sq_brkt,
				comma, dot, semi_colon, scope, at_symbol, ques_mk, dollar,

				//UnaryOp
				incr, decr,colon,
				bang/* ! */,

				//BinOp
				__bin_op_beg__,
				arrow,
				assign, equal, bang_equal,
				greater, greater_equal,
				less, less_equal,
				amp_amp, vert_vert,
				mod_asgn,
				minus_asgn, plus_asgn, div_asgn,
				mult_asgn, exp_asgn,
				amp_asgn, vert_asgn, amp, caret, mod, vert,
				minus, plus, slash, star, fat_arrow,
				__bin_op_end__,

				// Literals.
				identifier, l_char, l_string, l_real, l_null, l_true, l_false,

				// Keywords.
				k_struct, k_this, k_for, k_if, k_else,
				k_return, k_do,
				k_while, k_case, k_switch, k_break,
				k_default, k_array, k_set,
				k_enum, k_assert, k_define, k_var,
				k_print, k_println, k_fn, k_match, k_matches, k_run,

				__type_beg__,
				t_char, t_string, t_real, t_bool,
				__type_end__,

				eof // End of file
			} ty;
			TString lexeme;
			Object* literal;
			int ln;
		public:

			Token():
				ty(eof),
				lexeme(),
				literal(new Object()),
				ln(0){}

			Token(tok_type type,
				  const TString& lex,
				  const Object& lit,
				  int line):
				ty(type),
				lexeme(lex),
				literal(new Object(lit)),
				ln(line){}

			Token(const Token& tok){
				LFn;
				ty = tok.ty;
				lexeme = tok.lexeme;
				Logp(lexeme);
				literal = tok.literal?new Object(*(tok.literal)):new Object();
				ln = tok.ln;
				LVd;
			}

			Token& operator=(const Token& tok){
				LFn;
				ty = tok.ty;
				lexeme = tok.lexeme;
				Logp(lexeme);
				literal = tok.literal?new Object(*(tok.literal)):new Object();
				ln = tok.ln;
				LRet *this;
			}

			~Token(){
				delete literal;
			}

			TString to_string() const {
				TString str("[ Tok ");
				str.append(TString::number(static_cast<int>(ty)));
				if (literal->trait().is("real"))
					str.append(" : ").append(TString::number(literal->data().toDouble()));
				else if (literal->trait().is("string"))
					str.append(" : \"").append(literal->to_string()).append("\"");
				else if (lexeme.size()){
					str.append(" : '").append(lexeme).append("'");
				}
				return str.append(" ]");
			}

//			operator TString() const {
//				return to_string();
//			}

			bool is_bin_op() const {
				int i = t_cast<int>(ty);
				return (int)__bin_op_beg__ < i && i < (int)__bin_op_end__;
			}

			bool is_type() const {
				int i = t_cast<int>(ty);
				return (int)__type_beg__ < i && i < (int)__type_end__;
			}

	};
	typedef std::vector<Token> Tokens;
	//Q_DECLARE_METATYPE(Token)
	typedef Token Tok;
	typedef Tok::tok_type Tokty;
}
#endif // TOKEN_H
