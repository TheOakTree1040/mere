
#include "parser.h"
#include "mere_math.h"
#include "data_storage.hpp"
#include "merecallable.h"
#include <QVector>

Parser::Parser(QVector<Token>& toks):tokens(toks){
#if T_DBG
	Log << "Parser()";
	TLogHelper::indent();
#endif
}
Parser::~Parser(){
#if T_DBG
	TLogHelper::reset();
	TLogHelper::outdent();
#endif
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
	return peek().ty == Tok::_eof_;
}

Token& Parser::peek(int i){
	if ((current + i) > (tokens.size()-1))
		return tokens[tokens.size()-1];
	return tokens[current + i];
}

Token& Parser::prev(){
	return tokens[(current - 1)<0?0:current-1];
}

Token& Parser::advance(){
	if (!is_at_end()){
#if T_DBG
		Log << "adv: Type" << (int)peek().ty;
#endif
		current++;
	}
	return prev();
}

Token& Parser::expect(Tok ty, const TString& errmsg){
	LFn;
#if T_DBG
	Log << "Checking" << errmsg << TString::number(static_cast<int>(ty));
	Log << TString::number(static_cast<int>(peek().ty));
#endif
	if (peek().ty == ty)
        LRet advance();
	LThw error(peek(),errmsg);
}

ParseError Parser::error(const Token& tok, const TString& errmsg){
	MereMath::error(tok,errmsg);
	return ParseError();
}

void Parser::synchronize(){
	advance();
	while(!is_at_end()){
		if (prev().ty == Tok::semi_colon)
			return;
		switch(peek().ty){
			case Tok::_struct:
			case Tok::_var:
			case Tok::_for:
			case Tok::_if:
			case Tok::_while:
			case Tok::_return:
			case Tok::_assert:
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
		bool m = match(Tok::semi_colon);
		while(match(Tok::semi_colon));
		if (m){
			LRet nullptr;
		}
		Stmt s = nullptr;
		if (expected_block && peek().ty == Tok::l_brace){
			s = block(false);
		}
		else if (match(Tok::_if		)){
			s = if_stmt();
		}
		else if (match(Tok::_for	)){
			s = for_stmt();
		}
		else if (match(Tok::_print	)){
            s = PrintStmt(expression());
			expect(Tok::semi_colon,"Expected a ';' [println_sc]");
		}
		else if (match(Tok::_println)) {
			s = PrintlnStmt(expression());
			expect(Tok::semi_colon, "Expected a ';' [println_sc]");
		}
		else if (match(Tok::_while	)){
			s = while_stmt();
		}
		else if (match(Tok::_return	)){
			s = ret_stmt();
		}
		else if (match(Tok::_assert	)){
			s = assert_stmt();
		}
		else if (match(Tok::_match	)){
			s = match_stmt();
		}
		else {
			s = (peek().ty == Tok::dollar && peek(1).ty == Tok::l_brace)?block(true):decl_stmt();
		}
		LRet s;
	}
	catch(ParseError& pe){
		Q_UNUSED(pe)
	}
	catch(std::exception& ex){
		MereMath::error(prev().ln,TString("Exception caught @ Parser::stmt(): ").append(ex.what()));
	}
	catch(...){
		MereMath::error(prev().ln,TString("Exception caught @ Parser::stmt()."));
	}
	synchronize();
	LRet nullptr;
}

Stmt Parser::block(bool is_unexpected){
	LFn;
	if (is_unexpected){
		expect(Tok::dollar,"Expected a '$'.");
	}
	expect(Tok::l_brace, "Expected a '{'");
	Stmts blk;
	for(;!is_at_end();){
		if (match(Tok::r_brace)){
			LRet BlockStmt(blk);
		}
		blk.append(stmt());
	}
	LThw error(prev(),"Expected a statement.");
}

Stmt Parser::decl_stmt(){
	LFn;
	Stmt s = match(Tok::_var)?var_decl_stmt():
							  match(Tok::_fn)?fn_def_stmt():
											  expr_stmt();
	LRet s;
}

Stmt Parser::var_decl_stmt(bool eaten){
	LFn;
	if (!eaten){
		expect(Tok::_var,"Expected keyword 'var'.");
	}
	Token& name = expect(Tok::identifier,"Expected an identifier.");
	Expr initializer = nullptr;
	if (match(Tok::assign)){
		initializer = expression();
	}
	else{
		initializer = LitExpr(Object());
	}
	expect(Tok::semi_colon, "Expected a ';' after declaration.");
	LRet VarDeclStmt(name,initializer,nullptr);
}

Stmt Parser::if_stmt(){
	LFn;
	Expr expr = expression();
	if (match(Tok::_matches))
		LRet finish_match(expr);
	Stmt i_blk = stmt(true);
	Stmt e_blk = nullptr;
	if (match(Tok::_else)){
		e_blk = stmt(true);
	}
	LRet IfStmt(expr,i_blk,e_blk);
}

Stmt Parser::match_stmt(){
	LFn;
	LRet finish_match(expression());
}

Stmt Parser::finish_match(Expr m){
	LFn;
	expect(Tok::l_brace, "Expected a '{' [match_lbrace]");
	Expr e = nullptr; // branch temp
	Stmt s = nullptr; // "   ""    "
	QVector<Branch*> br;
	while(!check(Tok::r_brace)){
		e = match(Tok::star)?LitExpr(Object()):expression(); // * (wildcard) -> void literal, else call expression()
		s = match(Tok::colon)?stmt():block(); // : -> one stmt, else call block()
		br.push_back(new Branch(e,s)); // create a branch
	}
	expect(Tok::r_brace, "Expected a '}' [match_rbrace]");
	LRet MatchStmt(m,br);
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
	expect(Tok::semi_colon,"Expected statement termination with ';'.");
	LRet ExprStmt(expr);
}

Stmt Parser::for_stmt(){
	LFn;
	expect(Tok::l_paren, "Expected a parenthesis.");
	Stmt for_init = match(Tok::semi_colon)?nullptr:
										   match(Tok::_var)?var_decl_stmt():
															expr_stmt();
	Expr for_cond = check(Tok::semi_colon)?LitExpr(Object("bool",true)):
										   expression();
	expect(Tok::semi_colon,"Expected a ';'.");
	Expr for_act = check(Tok::r_paren)?nullptr:expression();
	expect(Tok::r_paren,"Expected a ')'.");
	Stmt body = stmt(true);
	if (for_act){
		body = BlockStmt({body,ExprStmt(for_act)});
	}
	body = WhileStmt(for_cond,body);
	if (for_init){
		body = BlockStmt({for_init,body});
	}
	LRet body;
}

Stmt Parser::fn_def_stmt(){
	Token& name = expect(Tok::identifier, "Expected an identifier [FnName].");
	QVector<Token> params;
	expect(Tok::l_paren,"Expected a '('. [FnLParen]");
	if (!check(Tok::r_paren)){
		do{
			params.append(advance());
		} while(match(Tok::comma));
	}
	expect(Tok::r_paren,"Expected a ')'. [FnRParen]");
	Stmt body;
	body = block(false);
	QVector<Stmt> stmts = *(body->block);
	body->block->clear();
	delete body;
	return FnStmt(name,params,stmts);
}

Stmt Parser::ret_stmt(){
	Token& keywd = prev();
	Expr expr;
	expr = check(Tok::semi_colon)?LitExpr(new Object()):expression();
	expect(Tok::semi_colon,"Expected a ';' [RetSColon]");
	return RetStmt(keywd,expr);
}

Stmt Parser::assert_stmt(){
	int ln = peek(-1).ln + 1;
	Expr expr = expression();
	int code = 0xFF;
	TString msg = "[Ln ";
	msg.append(TString::number(ln)).append("] ");
	if (match(Tok::colon)){
		bool has_code = false;
		if(match(Tok::_real)){
			has_code = true;
			code = peek(-1).literal->as<double>();
			if (!match(Tok::comma))
				goto WRAP_UP;
		}
		if (!has_code){
			msg += expect(Tok::_string, "Expected an exit message [AssertMsg]").literal->to_string();
		}
		else if (match(Tok::_string)){
			msg += peek(-1).literal->to_string();
		}
		else msg += "Assertion failed.";
	}
	else msg += "Assertion failed.";
	WRAP_UP:
	expect(Tok::semi_colon, "Expected a ';' [AssertSColon]");
	return AssertStmt(expr,code,msg);
}

//expr
Expr Parser::expression(bool disable){
	LFn;
	Expr exp = conditional();//goto next precedence
	if (!disable && check(Tok::comma)){
		QVector<Expr> cex;
		cex.append(exp);
		while(match(Tok::comma)){
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
	if (match({Tok::assign,Tok::mult_asgn,Tok::div_asgn,Tok::plus_asgn,Tok::minus_asgn,Tok::exp_asgn})){
		Token& op = prev();
		Expr right = conditional();
		expr = AssignExpr(expr,op,right);
	}
	else if (match(Tok::ques_mk)){
		Expr l = logical_or();
		expect(Tok::colon, "Expected a ':' in a conditional expression.");
		Expr r = logical_or();
		expr = CndtnlExpr(expr,l,r);
	}
	LRet expr;
}

Expr Parser::logical_or(){
	LFn;
	Expr expr = logical_and();
	while (match(Tok::vert_vert)){
		Token& op = prev();
		Expr right = logical_and();
		expr = LogicalExpr(expr,op,right);
	}
	LRet expr;
}

Expr Parser::logical_and(){
	LFn;
	Expr expr = equality();
	while (match(Tok::amp_amp)){
		Token& op = prev();
		Expr right = equality();
		expr = LogicalExpr(expr,op,right);
	}
	LRet expr;
}

Expr Parser::equality(){
	LFn;
	Expr expr = comparison();

	while (match({Tok::equal,Tok::bang_equal})){
		Token& op = prev();
		Expr right = comparison();
		expr = BinExpr(expr,op,right);
	}

	LRet expr;
}

Expr Parser::comparison(){
	LFn;
	Expr expr = terms();

	while (match({Tok::greater,Tok::greater_equal,
				 Tok::less,Tok::less_equal})){
		Token& op = prev();
		Expr right = terms();
		expr = BinExpr(expr,op,right);
	}

	LRet expr;
}

Expr Parser::terms(){
	LFn;
	Expr expr = term();

	while(match({Tok::plus, Tok::minus})){
		Token& op = prev();
		Expr right = term();
		expr = BinExpr(expr,op,right);
	}

	LRet expr;
}

Expr Parser::term(){
	LFn;
	Expr expr = refer();

	while (match({Tok::slash,Tok::star})){
		Token& op = prev();
		Expr right = refer();
		expr = BinExpr(expr,op,right);
	}

	LRet expr;
}

Expr Parser::refer() {
	LFn;
	Expr expr = exponent();
	if (match(Tok::fat_arrow)) {
		Token& op = prev();
		expr = RefExpr(expr, op, conditional());
	}
	LRet expr;
}

Expr Parser::exponent(){
	Expr expr = unary();
	while (match(Tok::caret)){
		Token& op = prev();
		Expr right = unary();
		expr = BinExpr(expr,op,right);
	}
	return expr;
}

Expr Parser::unary(){
	LFn;
	if		(match({Tok::bang,Tok::minus}			)){
		Token& op = prev();
		Expr right = unary();
		LRet PrefxExpr(op,right);
	}
	else if (match(Tok::plus						)){
		Token& op = prev();
		MereMath::error(peek(),TString("Unary operator+").append(" not supported."));
		Expr right = unary();
		LRet PrefxExpr(op,right);
	}
	else if (match({Tok::incr,Tok::decr}			)){
		Token& op = prev();
		Expr right = unary();
		LRet PrefxExpr(op,right);
	}
	else if (peek().is_bin_op(						)){
		Token& op = advance();
		MereMath::error(peek(),TString("Binary operator").append(op.lexeme).append(" used w/o left operand."));

		Expr right = expression(true);
		LRet PrefxExpr(op,right);
	}
	Expr rval = rvalue();
	if		(!rval->is(ExprTy::VarAcsr				)){
		LRet rval;
	}
	if (check(Tok::incr)||check(Tok::decr			)){
		while	(match({Tok::incr,Tok::decr}			)){
			Token& op = prev();
			rval = PstfxExpr(rval, op);
		}
	}

	LRet rval;
}

Expr Parser::rvalue(){
	LFn;
	int tmp_curr = current;
	try{
		Expr ex = call();
		LRet ex;
	}
	catch(ParseUnwind&){
		current = tmp_curr;
#if T_DBG
		Log << (TString)"Unwinded@" + TString::number(current) << "Ty:" << (int)peek().ty;
#endif
		Expr e = primary();
		LRet e;
	}
}


Expr Parser::call(){
	LFn;
	Expr expr = primary();

    while(true){
		if (match(Tok::l_paren)){
            expr = finish_call(expr);
		} else {
			break;
		}
	}
	LRet expr;
}

Expr Parser::finish_call(Expr callee){
	LFn;
	QVector<Expr> args;
	Expr arg = nullptr;
	if (!check(Tok::r_paren)){
		do {
			arg = expression(true);
			args.append(arg);
#if T_DBG
			Log << "FINISH_CALL_ARG_TY" << t_cast<int>(arg->type());
#endif
		} while (match(Tok::comma));
	}
	Token& paren = expect(Tok::r_paren, "Expected a ')'.");
	LRet FnCallExpr(callee,args,paren);
}

Expr Parser::primary(){
	LFn;
	Expr ex = nullptr;
	if		(match(Tok::_null							)){
		LRet LitExpr(Object(Trait("null"),QVariant(0)));
	}
	else if	(check(Tok::identifier						)){
		ex = VarAcsrExpr(advance());
		LRet ex;
	}
	else if (match(Tok::_true							)){
		LRet LitExpr(Object(Trait("bool"),QVariant(true)));
	}
	else if (match(Tok::_false							)){
		LRet LitExpr(Object(Trait("bool"),QVariant(false)));
	}
	else if (match(Tok::l_brace							)){
		ex = array();
		LRet ex;
	}
	else if (match({Tok::_string,Tok::_real,Tok::_char}	)){
		LRet LitExpr(*(prev().literal));
	}
	else if (match(Tok::dollar							)){
		ex = spec_data();
		LRet ex;
	}
	else if (check(Tok::l_paren							)){
		ex = group();
		LRet ex;
	}
	else									  {
		MereMath::error(peek(),"Expected an expression.");
		LRet InvalidExpr();
	}
}

constexpr qulonglong
h(const char* string)
{
	qulonglong hash = 0xEEF8UL;
	while (*string)
	{
		hash ^= (qulonglong)(*string++);
		hash *= 0xAE0DUL;
	}

	return hash;
}

Expr Parser::spec_data(){
	LFn;
	//assumes '$' is eaten
	Token& t = expect(Tok::identifier,"Expected an identifier");
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
	expect(Tok::l_brace,"Expected a '{'.");
	if (!check(Tok::r_brace)){
		do{
			Expr x = expression(true);
			expect(Tok::colon,"Expected a ':'.");
			Expr y = expression(true);
			inits.append(pair<Expr,Expr>(x,y));
		}while (!is_at_end() && peek().ty != Tok::r_brace && match(Tok::comma));
	}
	expect(Tok::r_brace, "Expected a '}'.");
	LRet AssocExpr(inits);
}

Expr Parser::map(){
	LFn;
	QVector<pair<Expr,Expr>> inits;
	expect(Tok::l_brace,"Expected a '{'.");
	if (!check(Tok::r_brace)){
		do{
			Expr x = expression(true);
			expect(Tok::colon,"Expected a ':'.");
			Expr y = expression(true);
			inits.append(pair<Expr,Expr>(x,y));
		}while (!is_at_end() && peek().ty != Tok::r_brace && match(Tok::comma));
	}
	expect(Tok::r_brace, "Expected a '}'.");
	LRet HashExpr(inits);
}

Expr Parser::array(){
	LFn;
	Token& t = prev();
	QVector<Expr> inits{};
	if (match(Tok::r_brace))
		LRet ArrayExpr(inits);
	inits.append(expression(true));
	for (;!is_at_end();){
		if (match(Tok::r_brace))
			LRet ArrayExpr(inits);
		inits.append((expect(Tok::comma, "Expected a ','."),expression(true)));
	}
	LThw error(t,"Expected termination.");
}

Expr Parser::accessor(bool unwind){
    LFn;
	if ((peek().ty!=Tok::identifier) && unwind){
		LThw ParseUnwind();
	}
	LRet VarAcsrExpr(expect(Tok::identifier,"Expected an identifier."));
}

Expr Parser::group(){
	LFn;
	if (!check(Tok::l_paren)){
		LRet expression();
	}
	advance();
	Expr expr = expression();
	if (expr->is(ExprTy::Group)){
		expr = expr->expr;
	}
	expect(Tok::r_paren, "Expected a ')'.");
	LRet GroupExpr(expr);
}
