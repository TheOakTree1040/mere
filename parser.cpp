#include "parser.h"
#include "mere_math.h"
#include "data_storage.h"
#include <QVector>
#include <QDebug>
Parser::Parser(QVector<Token>& toks):tokens(toks){}

Stmts Parser::parse(){
	try{
		Stmts stmts;
		while(!is_at_end()){
			qDebug() << "pos" << current << "type" << (int) peek().ty;
			stmts.append(stmt());
		}
		return stmts;
	}
	catch(ParseError&){
		return {};
	}
}

//Helpers
bool Parser::match(const QVector<Tok>& toks){
	for (int i = 0; i != toks.size(); i++){
		if (check(toks[i])){
			advance();
			return true;
		}
	}
	return false;
}

bool Parser::match(Tok ty){
	if (check(ty)){
		advance();
		return true;
	}
	return false;
}

bool Parser::check(Tok ty){
	if (is_at_end())
		return false;
	return peek().ty == ty;
}

bool Parser::is_at_end(){
	return peek().ty == Tok::END;
}

Token& Parser::peek(int i){
	if ((current + i) > (tokens.size()-1))
		return const_cast<Token&>(tokens.at(tokens.size()-1));
	return const_cast<Token&>(tokens.at(current + i));
}

Token& Parser::prev(){
	return const_cast<Token&>(tokens.at((current - 1)<0?0:current-1));
}

Token& Parser::advance(){
	if (!is_at_end()){
		qDebug() << "Advancing" << (int)peek().ty;
		current++;
	}
	return prev();
}

Token& Parser::expect(Tok ty, const QString& errmsg) throw(ParseError){
	if (peek().ty == ty)
		return advance();
	throw error(peek(),errmsg);
}

//error-handling
ParseError Parser::error(const Token& tok, const QString& errmsg){
	MereMath::error(tok,errmsg);
	return ParseError();
}

void Parser::synchronize(){
	advance();
	while(!is_at_end()){
		if (prev().ty == Tok::SCOLON)
			return;
		switch(peek().ty){
			case Tok::STRUCT:
			case Tok::VAR:
			case Tok::FOR:
			case Tok::IF:
			case Tok::WHILE:
			case Tok::RETURN:
				return;
			default:;
		}
		advance();
	}
}

//Parse Grammar Rules
Stmt Parser::stmt(){
	try{
		bool m = match(Tok::SCOLON);
		while(match(Tok::SCOLON));
		return m?NullStmt():
				 ((peek() == Tok::DOLLAR && peek(1) == Tok::LBRACE)?block():
																	decl_stmt());
	}catch(ParseError& err){
		synchronize();
		return NullStmt();
	}catch(std::exception& ex){
		synchronize();
		MereMath::error(prev().ln,QString("Exception caught during parsing: ").append(ex.what()));
		return NullStmt();
	}
}

Stmt Parser::block(bool is_unexpected){
	if (is_unexpected)
		expect(Tok::DOLLAR,"Expected a '$'.");
	expect(Tok::LBRACE, "Expected a '{'");
	Stmts stmts;
	if (){

	}
}

Stmt Parser::decl_stmt(){
	if (match(Tok::VAR))
		stmt = var_decl_stmt();
	else
		return expr_stmt();
}

Stmt Parser::var_decl_stmt(bool eaten){
	if (!eaten)
		expect(Tok::VAR,"Expected keyword 'var'.");
	Token name = expect(Tok::IDENTIFIER,"Expected an identifier.");
	Expr initializer = nullptr;
	if (match(Tok::EQUAL)){
		initializer = expression();
	}
	expect(Tok::SCOLON, "Expected a ';' after declaration.");
	return DeclStmt(name,initializer);
}

Stmt Parser::expr_stmt(){

	Expr expr = expression();
	expect(Tok::SCOLON,"Expected statement termination with ';'.");
	return ExprStmt(expr);
}

Expr Parser::expression(bool disable){
	qDebug() << __PRETTY_FUNCTION__;
	Expr exp = conditional();//goto next precedence
	if (!disable && check(Tok::COMMA)){
		QVector<Expr> cex;
		cex.append(exp);
		while(match(Tok::COMMA)){
			Expr expr = conditional();//goto next precedence
			cex.append(expr);
		}
		exp = CSExpr(cex);
	}
	return exp;
}

Expr Parser::conditional(){
	Expr expr = logical_or();
	if (match(Tok::QUES_MK)){
		Expr l = expression();
		expect(Tok::COLON, "Expected a colon in a conditional expression.");
		Expr r = expression(true);
		expr = CndtnlExpr(expr,l,r);
	}
	return expr;
}

Expr Parser::logical_or(){
	Expr expr = logical_and();
	while (match(Tok::VERTVERT)){
		Token& op = prev();
		Expr right = logical_and();
		expr = BinExpr(expr,op,right);
	}
	return expr;
}

Expr Parser::logical_and(){
	Expr expr = equality();
	while (match(Tok::AMPAMP)){
		Token& op = prev();
		Expr right = equality();
		expr = BinExpr(expr,op,right);
	}
	return expr;
}

Expr Parser::equality(){
	qDebug() << __PRETTY_FUNCTION__;
	Expr expr = comparison();

	while (match({Tok::EQUAL,Tok::N_EQUAL})){
		Token& op = prev();
		Expr right = comparison();
		expr = BinExpr(expr,op,right);
	}

	return expr;
}

Expr Parser::comparison(){
	qDebug() << __PRETTY_FUNCTION__;
	Expr expr = terms();

	while (match({Tok::GREATER,Tok::GREATER_EQUAL,
				 Tok::LESS,Tok::LESS_EQUAL})){
		Token& op = prev();
		Expr right = terms();
		expr = BinExpr(expr,op,right);
	}

	return expr;
}

Expr Parser::terms(){
	qDebug() << __PRETTY_FUNCTION__;
	Expr expr = term();

	while(match({Tok::PLUS, Tok::MINUS})){
		Token& op = prev();
		Expr right = term();
		expr = BinExpr(expr,op,right);
	}

	return expr;
}

Expr Parser::term(){
	qDebug() << __PRETTY_FUNCTION__;
	Expr expr = unary();

	while (match({Tok::SLASH,Tok::STAR})){
		Token& op = prev();
		Expr right = unary();
		expr = BinExpr(expr,op,right);
	}

	return expr;
}

Expr Parser::unary(){
	qDebug() << __PRETTY_FUNCTION__;
	if (match({Tok::EXCL,Tok::MINUS})){
		Token& op = prev();
		Expr right = unary();
		return PrefxExpr(op,right);
	}
	else if (match(Tok::PLUS)){
		Token& op = prev();
		MereMath::error(peek(),QString("Unary operator+").append(" not supported."));
		Expr right = unary();
		return PrefxExpr(op,right);
	}
	else if (match({Tok::INCR,Tok::DECR, Tok::MOD})){
		Token& op = prev();
		Expr right = lvalue();
		return PrefxExpr(op,right);
	}
	else if (peek().is_bin_op()){
		Token& op = prev();
		MereMath::error(peek(),QString("Binary operator").append(op.lexeme).append(" used w/o left operand."));

		Expr right = expression(true);
		return PrefxExpr(op,right);
	}

	Expr rval = rvalue();
	if (!rval->is(ExprTy::LValue) && !rval->is(ExprTy::Variable))
		return rval;
	if (match({Tok::INCR,Tok::DECR})){
		Token& op = prev();
		rval = PstfxExpr(rval, op);
	}
	else if (rval->is(ExprTy::LValue)){
		Expr cpy = rval->lval_expr;
		rval->lval_expr = 0;
		delete rval;
		rval = cpy;

	}
	return rval;
}

Expr Parser::rvalue(){
	qDebug() << __PRETTY_FUNCTION__;
	int tmp_curr = current;
	try{
		return lvalue(true);
	}
	catch(ParseUnwind&){
		current = tmp_curr;
		qDebug() << "Unwinded; pos" << current << "type" << (int)peek().ty;
		return primary();
	}
}

Expr Parser::lvalue(bool unwind){
	qDebug() << __PRETTY_FUNCTION__;
	if (check(Tok::LPAREN)){
		return LValExpr(group());
	}
	Expr acsr = member_acs(unwind);
	if (acsr->member_accessor->size() == 0 &&
		acsr->scope_accessor->size() == 1){
		Expr exp = VarExpr(acsr->scope_accessor->at(0),acsr->at_global);
		delete acsr;
		return exp;
	}
	return LValExpr(acsr);
}

Expr Parser::primary(){
	qDebug() << __PRETTY_FUNCTION__;
	if (match(Tok::TRUE)){
		return LitExpr(Object("bool",true));
	}
	else if (match(Tok::FALSE)){
		return LitExpr(Object("bool",false));
	}
	else if (match(Tok::LBRACE)){
		return array();
	}
	else if (match({Tok::STRING,Tok::REAL})){
		qDebug() << "	Detected Literal." << current-1;
		return LitExpr(prev().literal);
	}
	else if (match(Tok::DOLLAR))
		return spec_data();
	else if (check(Tok::LPAREN)){
		return group();
	}

	qDebug() << "	Literal Not found pos" << current << "type" << (int)peek().ty;
	MereMath::error(peek(),"Expected an expression.");
	return InvalidExpr();
}

//Primary Helpers

//just a constexpr wrapper around qHash
constexpr uint h(const QString& str){
	return qHash(str,0xAF);
}

Expr Parser::spec_data(){
	//assume '$' is eaten
	Token& t = expect(Tok::IDENTIFIER,"Expected an identifier");
	switch(qHash(t.lexeme)){
		case h("a"):
			return assoc();
			break;
		case h("h"):
			return hash();
		default:
			throw error(t,"Invalid data specifier.");
	}
}

Expr Parser::assoc(){
	QVector<pair<Expr,Expr>> inits;
	Token& l = expect(Tok::LBRACE,"Expected a '{'.");
	if (match(Tok::RBRACE))
		return AssocExpr(inits);
	{
		Expr x = expression(true);
		expect(Tok::COLON,"Expected a ':'.");
		Expr y = expression(true);
		inits.append(pair(x,y));
	}
	while (!is_at_end){
		if (match(Tok::RBRACE))
			return AssocExpr(inits);
		expect(Tok::COMMA,"Expected a ','.");
		Expr x = expression(true);
		expect(Tok::COLON,"Expected a ':'.");
		Expr y = expression(true);
		inits.append(pair(x,y));
	}
	throw error(l,"Expected termination.");
}

Expr Parser::hash(){
	QVector<pair<Expr,Expr>> inits;
	Token& l = expect(Tok::LBRACE,"Expected a '{'.");
	if (match(Tok::RBRACE))
		return HashExpr(inits);
	{
		Expr x = expression(true);
		expect(Tok::COLON,"Expected a ':'.");
		Expr y = expression(true);
		inits.append(pair(x,y));
	}
	while (!is_at_end){
		if (match(Tok::RBRACE))
			return HashExpr(inits);
		expect(Tok::COMMA,"Expected a ','.");
		Expr x = expression(true);
		expect(Tok::COLON,"Expected a ':'.");
		Expr y = expression(true);
		inits.append(pair(x,y));
	}
	throw error(l,"Expected termination.");
}

Expr Parser::array(){
	qDebug() << __PRETTY_FUNCTION__;
	Token& t = prev();
	QVector<Expr> inits{};
	if (match(Tok::RBRACE))
		return ArrayExpr(inits);
	inits.append(expression(true));
	while (!is_at_end()){
		if (match(Tok::RBRACE))
			return ArrayExpr(inits);
		expect(Tok::COMMA, "Expected a ','.");
		inits.append(expression(true));
	}
	throw error(t,"Expected termination.");
}

Expr Parser::member_acs(bool unwind) throw(ParseUnwind){
	qDebug() << __PRETTY_FUNCTION__;
	Expr scope = nullptr;
	try{
		scope = scope_acs(unwind);
	}
	catch(ParseUnwind&){
		throw ParseUnwind();
	}

	QVector<Expr> list;
	if (check(Tok::LPAREN)){
		Expr args = args_list();
		Expr fncall = FnCallExpr(scope,args);
		list.append(fncall);
	}
	while (match(Tok::DOT)){
		if (unwind && !check(Tok::IDENTIFIER)){
			delete scope;
			throw ParseUnwind();
		}
		Token& name = expect(Tok::IDENTIFIER,"Expected an identifier.");
		if (check(Tok::LPAREN)){
			Expr args = args_list();
			Expr fncall = FnCallExpr(ScopeAcsExpr({name}),args);
			list.append(fncall);
		}
		else
			list.append(AItemExpr(name));
	}
	return MemberAcsExpr(scope,list);
}

Expr Parser::scope_acs(bool unwind) throw(ParseUnwind){
	qDebug() << __PRETTY_FUNCTION__;
	QVector<Token> acsrs{};
	bool at_glob = match(Tok::SCOPE);
	if (unwind){
		if (!check(Tok::IDENTIFIER))
			throw ParseUnwind();
		acsrs.append(advance());
		while (match(Tok::SCOPE)){
			if (!check(Tok::IDENTIFIER))
				throw ParseUnwind();
			acsrs.append(advance());
		}
	}
	else {
		acsrs.append(expect(Tok::IDENTIFIER,"Expected an identifier."));
		while (match(Tok::SCOPE)){
			acsrs.append(expect(Tok::IDENTIFIER,"Expected an identifier."));
		}
	}
	return ScopeAcsExpr(acsrs,at_glob);
}

Expr Parser::args_list(bool eaten){
	qDebug() << __PRETTY_FUNCTION__;
	if (!eaten)
		expect(Tok::LPAREN, "Expected a '('.");
	QVector<Expr> list{};
	if (!check(Tok::RPAREN)){
		do{
			list.append(expression(true));
		}while (match(Tok::COMMA));
	}
	expect(Tok::RPAREN, "Expected a ')'.");
	return ArgsLiExpr(list);
}

Expr Parser::group(){
	qDebug() << __PRETTY_FUNCTION__;
	if (!check(Tok::LPAREN)){
		Expr expr = expression();
		return expr;
	}
	advance();
	Expr expr = expression();
	if (expr->is(ExprTy::Group)){
		expr = expr->expr;
	}
	expect(Tok::RPAREN, "Expected ')' to terminate expression.");
	return GroupExpr(expr);
}
