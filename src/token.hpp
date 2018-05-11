#ifndef TOKEN_H
#define TOKEN_H

#include "object.h"
enum class Tok{
	invalid,

	// Characters.
	l_paren, r_paren, l_brace, r_brace, l_sq_brkt, r_sq_brkt,
	comma, dot, semi_colon, scope, at_symbol, ques_mk, dollar,

	//UnaryOp
	incr, decr,
	minus, plus, slash, star, colon,
	amp, caret/* ^ */, mod, bang/* ! */, vert,

	//BinOp
	arrow,
	assign, equal, bang_equal,
	greater, greater_equal,
	less, less_equal,
	amp_amp, vert_vert,
	mod_asgn,
	minus_asgn, plus_asgn, div_asgn,
	mult_asgn, exp_asgn,
	amp_asgn, vert_asgn,

	fat_arrow,

	// Literals.
	identifier, _char, _string, _real,

	// Keywords.
	_struct, _this, _for, _if, _else,
	_return, _true, _false, _do,
	_while, _case, _switch, _break,
	_default, _array, _set,
	_enum, _assert, _define, _var, _null, _print, _fn, _match, _matches,

	_eof_
};

class Token{
	public:
		Tok ty;
		TString lexeme;
		Object* literal;
		int ln;
	public:

		Token(){
			ln = 0;
			lexeme = "";
			literal = new Object();
			ty = Tok::_eof_;
		}

		Token(Tok type,
			  const TString& lex,
			  const Object& lit,
			  int line):
		ty(type),
		lexeme(lex),
		literal(new Object(lit)),
		ln(line){}

//		Token(Tok type,
//			  const TString& lex,
//			  Object* lit,
//			  int line):
//		ty(type),
//		lexeme(TString(lex)),
//		literal(lit),
//		ln(line){}

		Token(const Token& tok){
			LFn;
			ty = tok.ty;
#if _DEBUG
			Log << (lexeme = tok.lexeme);
#else
            lexeme = tok.lexeme;
#endif

			literal = tok.literal?new Object(*(tok.literal)):new Object();
			ln = tok.ln;
			LVd;
		}

		Token& operator=(const Token& tok){
			LFn;
			ty = tok.ty;
#if _DEBUG
			Log << (lexeme = tok.lexeme);
#else
			lexeme = tok.lexeme;
#endif
			literal = tok.literal?new Object(*(tok.literal)):new Object();
			ln = tok.ln;
			LRet *this;
		}

		~Token(){
			delete literal;
		}

		TString to_string() {
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

		operator TString()   {
			return to_string();
		}

		bool is_bin_op()const{
			int i = static_cast<int>(ty);
			return (int)Tok::arrow <= i && i <= (int)Tok::vert_asgn;
		}

};
typedef QVector<Token> Tokens;
//Q_DECLARE_METATYPE(Token)

#endif // TOKEN_H
