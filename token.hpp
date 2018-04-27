#ifndef TOKEN_H
#define TOKEN_H

#include "object.h"
enum class Tok{
	INVALID,

	// Characters.
	LPAREN, RPAREN, LBRACE, RBRACE, LSQBRKT, RSQBRKT,
	COMMA, DOT, SCOLON, SCOPE, AT_SYMB, QUES_MK, DOLLAR,

	//UnaryOp
	INCR, DECR,
	MINUS, PLUS, SLASH, STAR, COLON,
	AMP, CARET/* ^ */, MOD, EXCL/* ! */, VERT,

	//BinOp
	ARROW,
	ASSIGN, EQUAL, N_EQUAL,
	GREATER, GREATER_EQUAL,
	LESS, LESS_EQUAL,
	AMPAMP, VERTVERT,
	MOD_ASGN,
	MINUS_ASGN, PLUS_ASGN, DIV_ASGN,
	MULT_ASGN, EXP_ASGN,
	AMP_ASGN, VERT_ASGN,

	FAT_ARROW,

	// Literals.
	IDENTIFIER, CHAR, STRING, REAL,

	// Keywords.
	STRUCT, THIS, FOR, IF, ELSE, GLOBAL,
	RETURN, TRUE, FALSE, DO,
	WHILE, CASE, SWITCH, BREAK,
	DEFAULT, ARRAY, SET,
	ENUM, ASSERT, DEFINE, VALUED, VAR, NULL_LIT, PRINT, FN,

	END
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
			ty = Tok::END;
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
			TString str("[Tok ");
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
			return (int)Tok::ARROW <= i && i <= (int)Tok::VERT_ASGN;
		}

};
typedef QVector<Token> Tokens;
//Q_DECLARE_METATYPE(Token)

#endif // TOKEN_H
