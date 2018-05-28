#include "token.h"
using namespace mere;

Token::Token():
	m_ty(eof),
	m_lexeme(),
	m_literal(new Object()),
	m_loc(){}

Token::Token(Token::tok_type type, const QString& lex, const Object& lit, srcloc_t loc):
	m_ty(type),
	m_lexeme(lex),
	m_literal(new Object(lit)),
	m_loc(loc){}

Token::Token(const Token& tok){
	LFn;
	m_ty = tok.m_ty;
	m_lexeme = tok.m_lexeme;
	Logp(m_lexeme);
	m_literal = tok.m_literal?new Object(*(tok.m_literal)):new Object();
	m_loc = tok.m_loc;
	LVd;
}

Token& Token::operator=(const Token& tok){
	LFn;
	m_ty = tok.m_ty;
	m_lexeme = tok.m_lexeme;
	Logp(m_lexeme);
	m_literal = tok.m_literal?new Object(*(tok.m_literal)):new Object();
	m_loc = tok.m_loc;
	LRet *this;
}

QString Token::to_string() const {
	QString str("[ %1 | %2 | %3:%4 ]");
	str = str.arg(static_cast<int>(m_ty));
	if (m_literal->trait().is("real"))
		str = str.arg(m_literal->data().toDouble());
	else if (m_literal->trait().is("string"))
		str = str.arg("\"" + m_literal->to_string() + "\"");
	else if (m_lexeme.size()){
		str = str.arg("'" + m_lexeme + "'");
	}
	str = str.arg(line()).arg(col());
	return str;
}

bool Token::is_bin_op() const noexcept {
	int i = t_cast<int>(m_ty);
	return (int)__bin_op_beg__ < i && i < (int)__bin_op_end__;
}

bool Token::is_type() const noexcept {
	int i = t_cast<int>(m_ty);
	return (int)__type_beg__ < i && i < (int)__type_end__;
}

bool Token::is_keyword() const noexcept {
	int i = t_cast<int>(m_ty);
	return (int)__keywd_beg__ < i && i < (int)__keywd_end__;
}

QString Tokens::to_string(QString delim) const {
	QString str = "";
	int s = this->size();
	for (int i = 0; i != s - 1; i++) {
		str.append(this->at(i).to_string()).append(delim);
	}
	str.append(this->at(s - 1).to_string());
	return str;
}
