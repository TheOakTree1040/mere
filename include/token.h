#pragma once

#ifndef TOKEN_H
#define TOKEN_H

#include "object.h"
#include "src.h"

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
				__keywd_beg__,
				k_struct, k_this, k_for, k_if, k_else,
				k_return, k_do,
				k_while, k_case, k_switch, k_break,
				k_default, k_array, k_set,
				k_enum, k_assert, k_define, k_var,
				k_print, k_println, k_fn, k_match, k_matches, k_run,
				__keywd_end__,

				__type_beg__,
				t_char, t_string, t_real, t_bool,
				__type_end__,

				done, eof // End of file
			};
		private:
			tok_type m_ty;
			QString m_lexeme;
			Object* m_literal;
			srcloc_t m_loc;
		public:
			Token();
			Token(const Token& tok);
			Token(tok_type type, const QString& lex, const Object& lit, srcloc_t loc);

			~Token(){ delete m_literal; }

			Token& operator=(const Token& tok);

			QString to_string() const;

			bool is_bin_op() const noexcept;
			bool is_type() const noexcept;
			bool is_keyword() const noexcept;

			tok_type type() const noexcept { return m_ty; }
			QString lexeme() const noexcept { return m_lexeme; }
			const Object& literal() const noexcept { return *m_literal; }
			uint line() const noexcept { return m_loc.line; }
			uint col() const noexcept { return m_loc.col; }
			const srcloc_t& loc() const noexcept { return m_loc; }
	};

	class Tokens : public std::vector<Token>{
		public:
			Tokens(){}

			Tokens(const std::vector<Token>& toks) :
			std::vector<Token>(toks){}

			Tokens(const Tokens& toks):
			std::vector<Token>(toks){}

			QString to_string(QString delim = "") const;
	};

	typedef Token Tok;
	typedef Tok::tok_type Tokty;
}
#endif // TOKEN_H
