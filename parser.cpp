
#include "parser.h"
#include "mere_math.h"
#include "data_storage.h"
#include <QVector>

Parser::Parser(QVector<Token>& toks):tokens(toks){
	Logger() << "Parser()";
	Logger::indent++;

}
Parser::~Parser(){
	Logger::indent = 0;
	Logger() << "~Parser()";
}

Stmts Parser::parse(){
	Stmts stmts;
	Stmt temp = nullptr;
	while(!is_at_end()){
		if ((temp = stmt()))
			stmts.append(temp);
	}
	return stmts;
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
		//Logger() << "adv: Type" << (int)peek().ty;
		current++;
	}
	return prev();
}

Token& Parser::expect(Tok ty, const QString& errmsg) throw(ParseError){
	if (peek().ty == ty)
		return advance();
	throw error(peek(),errmsg);
}

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
//stmt
Stmt Parser::stmt(bool expected_block){
	LFn;
	try{
		bool m = match(Tok::SCOLON);
		while(match(Tok::SCOLON));
		if (m){
			LRet nullptr;
		}
		Stmt s = nullptr;
		if (expected_block && peek().ty == Tok::LBRACE){
			s = block(false);
		}
		else if (match(Tok::IF)){
			s = if_stmt();
		}
		else if (match(Tok::WHILE)){
			s = while_stmt();
		}
		else if (match(Tok::FOR)){
			s = for_stmt();
		}
		else {
			s = (peek().ty == Tok::DOLLAR && peek(1).ty == Tok::LBRACE)?block(true):decl_stmt();
		}
		LRet s;
	}
	catch(ParseError&){}
	catch(std::exception& ex){
		MereMath::error(prev().ln,QString("Exception caught during parsing: ").append(ex.what()));
	}
	synchronize();
	LRet nullptr;
}

Stmt Parser::block(bool is_unexpected){
	LFn;
	if (is_unexpected)
		expect(Tok::DOLLAR,"Expected a '$'.");
	expect(Tok::LBRACE, "Expected a '{'");
	Stmts blk;
	for(;!is_at_end();){
		if (match(Tok::RBRACE))
			LRet BlockStmt(blk);
		blk.append(stmt());
	}
	LThw error(prev(),"Expected a statement.");
}

Stmt Parser::decl_stmt(){
	LFn;
	Stmt s = match(Tok::VAR)?var_decl_stmt():expr_stmt();
	LRet s;
}

Stmt Parser::var_decl_stmt(bool eaten){
	LFn;
	if (!eaten)
		expect(Tok::VAR,"Expected keyword 'var'.");
	Token& name = expect(Tok::IDENTIFIER,"Expected an identifier.");
	Expr initializer = nullptr;
	if (match(Tok::ASSIGN))
		initializer = expression();
	expect(Tok::SCOLON, "Expected a ';' after declaration.");
	LRet VarDeclStmt(name,initializer,nullptr);
}

Stmt Parser::if_stmt(){
	LFn;
	Expr condit = expression();
	Stmt i_blk = stmt(true);
	Stmt e_blk = nullptr;
	if (match(Tok::ELSE))
		e_blk = stmt(true);
	LRet IfStmt(condit,i_blk,e_blk);
}

Stmt Parser::while_stmt(){
	LFn;
	Expr c_condit = expression();
	Stmt w_blk = stmt(true);
	LRet WhileStmt(c_condit,w_blk);
}

Stmt Parser::expr_stmt(){
	LFn;
	Expr expr = expression();
	expect(Tok::SCOLON,"Expected statement termination with ';'.");
	LRet ExprStmt(expr);
}

Stmt Parser::for_stmt(){
	LFn;
	expect(Tok::LPAREN, "Expected a parenthesis.");
	Stmt for_init = match(Tok::SCOLON)?nullptr:
									   match(Tok::VAR)?var_decl_stmt():
													   expr_stmt();
	Expr for_cond = check(Tok::SCOLON)?LitExpr(Object("bool",true)):
									   expression();
	expect(Tok::SCOLON,"Expected a ';'.");
	Expr for_act = check(Tok::RPAREN)?nullptr:expression();
	expect(Tok::RPAREN,"Expected a ')'.");
	Stmt body = stmt(true);
	if (for_act)
		body = BlockStmt({body,ExprStmt(for_act)});
	body = WhileStmt(for_cond,body);
	if (for_init)
		body = BlockStmt({for_init,body});
	LRet body;
}

//expr
Expr Parser::expression(bool disable){
	LFn;
	Expr exp = conditional();//goto next precedence
	if (!disable && check(Tok::COMMA)){
		Log << "parsing CSExpr";
		QVector<Expr> cex;
		cex.append(exp);
		while(match(Tok::COMMA)){
			Expr expr = conditional();//goto next precedence
			cex.append(expr);
		}
		exp = CSExpr(cex);
	}
	LRet exp;
}

Expr Parser::conditional(){
	LFn;
	Expr expr = logical_or();
	if (match(Tok::EQUAL)){
		Expr right = conditional();
		expr = AssignExpr(expr,right);
	}
	else if (match(Tok::QUES_MK)){
		Expr l = logical_or();
		expect(Tok::COLON, "Expected a ':' in a conditional expression.");
		Expr r = logical_or();
		expr = CndtnlExpr(expr,l,r);
	}
	LRet expr;
}

Expr Parser::logical_or(){
	LFn;
	Expr expr = logical_and();
	while (match(Tok::VERTVERT)){
		Token& op = prev();
		Expr right = logical_and();
		expr = BinExpr(expr,op,right);
	}
	LRet expr;
}

Expr Parser::logical_and(){
	LFn;
	Expr expr = equality();
	while (match(Tok::AMPAMP)){
		Token& op = prev();
		Expr right = equality();
		expr = BinExpr(expr,op,right);
	}
	LRet expr;
}

Expr Parser::equality(){
	LFn;
	Expr expr = comparison();

	while (match({Tok::EQUAL,Tok::N_EQUAL})){
		Token& op = prev();
		Expr right = comparison();
		expr = BinExpr(expr,op,right);
	}

	LRet expr;
}

Expr Parser::comparison(){
	LFn;
	Expr expr = terms();

	while (match({Tok::GREATER,Tok::GREATER_EQUAL,
				 Tok::LESS,Tok::LESS_EQUAL})){
		Token& op = prev();
		Expr right = terms();
		expr = BinExpr(expr,op,right);
	}

	LRet expr;
}

Expr Parser::terms(){
	LFn;
	Expr expr = term();

	while(match({Tok::PLUS, Tok::MINUS})){
		Token& op = prev();
		Expr right = term();
		expr = BinExpr(expr,op,right);
	}

	LRet expr;
}

Expr Parser::term(){
	LFn;
	Expr expr = unary();

	while (match({Tok::SLASH,Tok::STAR})){
		Token& op = prev();
		Expr right = unary();
		expr = BinExpr(expr,op,right);
	}

	LRet expr;
}

Expr Parser::unary(){
	LFn;
	if		(match({Tok::EXCL,Tok::MINUS}			)){
		Token& op = prev();
		Expr right = unary();
		LRet PrefxExpr(op,right);
	}
	else if (match(Tok::PLUS						)){
		Token& op = prev();
		MereMath::error(peek(),QString("Unary operator+").append(" not supported."));
		Expr right = unary();
		LRet PrefxExpr(op,right);
	}
	else if (match({Tok::INCR,Tok::DECR, Tok::MOD}	)){
		Token& op = prev();
		Expr right = lvalue();
		LRet PrefxExpr(op,right);
	}
	else if (peek().is_bin_op(						)){
		Token& op = prev();
		MereMath::error(peek(),QString("Binary operator").append(op.lexeme).append(" used w/o left operand."));

		Expr right = expression(true);
		LRet PrefxExpr(op,right);
	}
	Expr rval = rvalue();
	if		(!rval->is(ExprTy::LValue) && !rval->is(ExprTy::VarAcsr	)){
		LRet rval;
	}
	if		(match({Tok::INCR,Tok::DECR}							)){
		Token& op = prev();
		rval = PstfxExpr(rval, op);
	}
	else if (rval->is(ExprTy::LValue								)){
		Expr cpy = rval->lval_expr;
		rval->lval_expr = 0;
		delete rval;
		rval = cpy;
	}
	LRet rval;
}

Expr Parser::rvalue(){
	LFn;
	int tmp_curr = current;
	try{
		Expr ex = nullptr;
		if (check(Tok::LPAREN))
			ex = group();
		else
			ex = primary();
		LRet ex;
	}
	catch(ParseUnwind&){
		current = tmp_curr;
		Log << (QString)"Unwinded@" + QString::number(current) << "Ty:" << (int)peek().ty;
		Expr e = primary();
		LRet e;
	}
}

Expr Parser::lvalue(bool unwind){
	LFn;
	Expr e = check(Tok::LPAREN)?group():accessor(unwind);
	LRet e;
}

Expr Parser::primary(){
	LFn;
	Expr ex = nullptr;
	if		(match(Tok::NULL_LIT			)){
		LRet LitExpr(Object("null",0));
	}
	else if	(check(Tok::IDENTIFIER			)){
		ex = VarAcsrExpr(advance());
		LRet ex;
	}
	else if (match(Tok::TRUE				)){
		LRet LitExpr(Object("bool",true));
	}
	else if (match(Tok::FALSE				)){
		LRet LitExpr(Object("bool",false));
	}
	else if (match(Tok::LBRACE				)){
		ex = array();
		LRet ex;
	}
	else if (match({Tok::STRING,Tok::REAL}	)){
		LRet LitExpr(*(prev().literal));
	}
	else if (match(Tok::DOLLAR				)){
		ex = spec_data();
		LRet ex;
	}
	else if (check(Tok::LPAREN				)){
		ex = group();
		LRet ex;
	}
	else									  {
		MereMath::error(peek(),"Expected an expression.");
		LRet InvalidExpr();
	}
}

//Primary Helpers
constexpr long long
h(const char* string)
{
	long long hash = 0xeef8;
	while (*string)
	{
		hash ^= (uint)(*string++);
		hash *= 0xAE9D;
	}

	return hash;
}

Expr Parser::spec_data(){
	LFn;
	//assume '$' is eaten
	Token& t = expect(Tok::IDENTIFIER,"Expected an identifier");
	switch(h(t.lexeme.toStdString().c_str())){
		case h("a"):
			LRet assoc();
			break;
		case h("h"):
			LRet map();
		default:
			LThw error(t,"Invalid data specifier.");
	}
}

Expr Parser::assoc(){
	LFn;
	QVector<pair<Expr,Expr>> inits;
	expect(Tok::LBRACE,"Expected a '{'.");
	if (!check(Tok::RBRACE)){
		do{
			Expr x = expression(true);
			expect(Tok::COLON,"Expected a ':'.");
			Expr y = expression(true);
			inits.append(pair<Expr,Expr>(x,y));
		}while (!is_at_end() && peek().ty != Tok::RBRACE && match(Tok::COMMA));
	}
	expect(Tok::RBRACE, "Expected a '}'.");
	LRet AssocExpr(inits);
}

Expr Parser::map(){
	LFn;
	QVector<pair<Expr,Expr>> inits;
	expect(Tok::LBRACE,"Expected a '{'.");
	if (!check(Tok::RBRACE)){
		do{
			Expr x = expression(true);
			expect(Tok::COLON,"Expected a ':'.");
			Expr y = expression(true);
			inits.append(pair<Expr,Expr>(x,y));
		}while (!is_at_end() && peek().ty != Tok::RBRACE && match(Tok::COMMA));
	}
	expect(Tok::RBRACE, "Expected a '}'.");
	LRet HashExpr(inits);
}

Expr Parser::array(){
	LFn;
	Token& t = prev();
	QVector<Expr> inits{};
	if (match(Tok::RBRACE))
		LRet ArrayExpr(inits);
	inits.append(expression(true));
	for (;!is_at_end();){
		if (match(Tok::RBRACE))
			LRet ArrayExpr(inits);
		inits.append((expect(Tok::COMMA, "Expected a ','."),expression(true)));
	}
	LThw error(t,"Expected termination.");
}

Expr Parser::accessor(bool unwind) throw(ParseUnwind){
	LFn;
	Log << "current:" << QString::number(current) << "type:" << QString::number((int)peek().ty);
	if (((int)(peek().ty)!=44) && unwind)
		LThw ParseUnwind();
	LRet VarAcsrExpr(expect(Tok::IDENTIFIER,"Expected an identifier."));
}

Expr Parser::args_list(bool eaten){
	LFn;
	if (!eaten)
		expect(Tok::LPAREN, "Expected a '('.");
	QVector<Expr> list{};
	if (!check(Tok::RPAREN)){
		do{
			list.append(expression(true));
		}while (match(Tok::COMMA));
	}
	expect(Tok::RPAREN, "Expected a ')'.");
	LRet ArgsLiExpr(list);
}

Expr Parser::group(){
	LFn;
	if (!check(Tok::LPAREN)){
		LRet expression();
	}
	advance();
	Expr expr = expression();
	if (expr->is(ExprTy::Group)){
		expr = expr->expr;
	}
	expect(Tok::RPAREN, "Expected a ')'.");
	LRet GroupExpr(expr);
}
