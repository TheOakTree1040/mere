#include "token.h"

using namespace mere;

Token::Token():
	ty(eof),
	lexeme(),
	literal(new Object()),
	ln(0){}

Token::Token(Token::tok_type type, const TString& lex, const Object& lit, int line):
	ty(type),
	lexeme(lex),
	literal(new Object(lit)),
	ln(line){}

Token::Token(const Token& tok){
	LFn;
	ty = tok.ty;
	lexeme = tok.lexeme;
	Logp(lexeme);
	literal = tok.literal?new Object(*(tok.literal)):new Object();
	ln = tok.ln;
	LVd;
}

Token& Token::operator=(const Token& tok){
	LFn;
	ty = tok.ty;
	lexeme = tok.lexeme;
	Logp(lexeme);
	literal = tok.literal?new Object(*(tok.literal)):new Object();
	ln = tok.ln;
	LRet *this;
}

TString Token::to_string() const {
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

bool Token::is_bin_op() const {
	int i = t_cast<int>(ty);
	return (int)__bin_op_beg__ < i && i < (int)__bin_op_end__;
}

bool Token::is_type() const {
	int i = t_cast<int>(ty);
	return (int)__type_beg__ < i && i < (int)__type_end__;
}
