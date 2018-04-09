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

	// Literals.
	IDENTIFIER, CHAR, STRING, REAL,

	// Keywords.
	STRUCT, THIS, FOR, IF, ELSE, GLOBAL,
	RETURN, TRUE, FALSE, DO,
	WHILE, CASE, SWITCH, BREAK,
	DEFAULT, ARRAY, SET,
	ENUM, ASSERT, DEFINE, VALUED, VAR, NULL_LIT,

	END
};

class Token{
	public:
		Tok ty;
		QString lexeme;
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
			  const QString& lex,
			  const Object& lit,
			  int line):
		ty(type),
		lexeme(QString(lex)),
		literal(new Object(lit)),
		ln(line){}

//		Token(Tok type,
//			  const QString& lex,
//			  Object* lit,
//			  int line):
//		ty(type),
//		lexeme(QString(lex)),
//		literal(lit),
//		ln(line){}

		Token(const Token& tok){
			ty = tok.ty;
			lexeme = tok.lexeme;
			literal = tok.literal?new Object(*(tok.literal)):new Object();
			ln = tok.ln;
		}

		Token& operator=(const Token& tok){
			ty = tok.ty;
			lexeme = tok.lexeme;
			literal = tok.literal?new Object(*(tok.literal)):new Object();
			ln = tok.ln;
			return *this;
		}

		~Token(){
			delete literal;
		}

		QString to_string() const{
			QString str("[Tok ");
			str.append(QString::number(static_cast<int>(ty)));
			if (literal->trait.is("real"))
				str.append(" : ").append(QString::number(literal->dat().toDouble()));
			else if (literal->trait.is("string"))
				str.append(" : \"").append(literal->to_string()).append("\"");
			else if (lexeme.size()){
				str.append(" : '").append(lexeme).append("'");
			}
			return str.append(" ]");
		}

		operator QString() const{
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
