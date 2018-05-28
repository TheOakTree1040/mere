
#include "parser.h"
#include "data_storage.h"
#include "merecallable.h"

namespace mere {
	Parser::Parser(IntpUnit unit):
		unit(unit),
		tokens(unit->tokens()){
		Logp("Parser()");
		LIndt;
	}
	Parser::~Parser(){
		LRst;
	}

	void Parser::parse(){
		LFn;
		AST& ast = unit->ast();
		if (tokens.empty() || !unit->success()){
			LVd;
		}
		while(!is_at_end()){
			Stmt s = stmt();
			ast.push_back(s);
		}
		LVd;
	}

	//Helpers

	bool Parser::match(const std::vector<Tokty>& toks){
		for (uint i = 0u; i != toks.size(); i++){
			if (check(toks[i])){
				advance();
				return true;
			}
		}
		return false;
	}

	bool Parser::match(Tokty ty){
		if (check(ty)){
			advance();
			return true;
		}
		return false;
	}

	bool Parser::check(Tokty ty){
		if (is_at_end())
			return false;
		return peek().type() == ty;
	}

	bool Parser::is_at_end(){
		return peek().type() == Tok::eof;
	}

	Token& Parser::peek(short i){
		if (unsigned(current + i) > (tokens.size()-1))
			return tokens[tokens.size()-1];
		return tokens[current + i];
	}

	Token& Parser::prev(){
		return tokens[(current - 1)<0?0:current-1];
	}

	Token& Parser::advance(){
		if (!is_at_end()){
			Log ls("adv: Type") ls((int)peek().type());
			current++;
		}
		return prev();
	}

	Token& Parser::expect(Tokty ty, const QString& errmsg){
		LFn;
		Log ls("Checking") ls(errmsg) ls(QString::number(static_cast<int>(ty)));
		Log ls(QString::number(static_cast<int>(peek().type())));
		if (peek().type() == ty)
			LRet advance();
		LThw make_error(peek(),errmsg);
	}

	ParseError Parser::make_error(const Token& tok, const QString& errmsg){
		error(tok,errmsg);
		return ParseError();
	}

	void Parser::error(const Token& tok, const QString& errmsg) const {
		unit->report(tok.loc(), "parse-error", errmsg);
	}

	void Parser::synchronize(){
		advance();
		while(!is_at_end()){
			if (prev().type() == Tok::semi_colon)
				return;
			switch(peek().type()){
				case Tok::k_struct:
				case Tok::k_var:
				case Tok::k_for:
				case Tok::k_if:
				case Tok::k_while:
				case Tok::k_return:
				case Tok::k_assert:
				case Tok::k_run:
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
			if (match(Tok::semi_colon)){
				while(match(Tok::semi_colon));
				LRet NullStmt();
			}
			else if (expected_block && check(Tok::l_brace)){
				LRet block(false);
			}
			else if (!peek().is_keyword()) {
				LRet
				(peek().type() == Tok::dollar && peek(1).type() == Tok::l_brace)?
							block(true):expr_stmt();
			}
			else if (match(Tok::k_var)){
				LRet var_decl_stmt();
			}
			else if (match(Tok::k_if		)){
				LRet if_stmt();
			}
			else if (match(Tok::k_for	)){
				LRet for_stmt();
			}
			else if (match(Tok::k_return	)){
				LRet ret_stmt();
			}
			else if (match(Tok::k_while	)){
				LRet while_stmt();
			}
			else if (match(Tok::k_match	)){
				LRet match_stmt();
			}
			else if (match(Tok::k_print	)){
				Stmt s = PrintStmt(expression());
				expect(Tok::semi_colon,"expected a ';'");
				LRet s;
			}
			else if (match(Tok::k_println)) {
				Stmt s = PrintlnStmt(expression());
				expect(Tok::semi_colon, "expected a ';'");
				LRet s;
			}
			else if (match(Tok::k_fn)){
				LRet fn_def_stmt();
			}
			else if (match(Tok::k_assert	)){
				LRet assert_stmt();
			}
			else if (match(Tok::k_run)){
				LRet run_stmt();
			}
		}
		catch(ParseError&) {}
		catch(std::exception& ex){
			unit->report(prev().loc(),
						 "exception (Parser::stmt)",
						 QString(ex.what()));
		}
		catch(...){
			unit->report(prev().loc(),
						 "exception (Parser::stmt)",
						 QString("unknown exception"));
		}
		synchronize();
		LRet NullStmt();
	}

	Stmt Parser::block(bool is_unexpected){
		LFn;
		if (is_unexpected){
			expect(Tok::dollar,"expected a '$'");
		}
		expect(Tok::l_brace, "expected a '{'");
		AST blk;
		for(;!is_at_end();){
			if (match(Tok::r_brace)){
				LRet BlockStmt(blk);
			}
			blk.push_back(stmt());
		}
		LThw make_error(prev(),"expected a '}'");
	}

	Stmt Parser::decl_stmt(){
		LFn;
		Stmt s = match(Tok::k_var)?var_decl_stmt():
								   match(Tok::k_fn)?fn_def_stmt():
													expr_stmt();
		LRet s;
	}

	Stmt Parser::var_decl_stmt(bool eaten){
		LFn;
		if (!eaten){
			expect(Tok::k_var,"expected keyword 'var'");
		}
		Token& name = expect(Tok::identifier,"expected an identifier");
		Expr initializer = match(Tok::assign)?expression():LitExpr(Object());
		expect(Tok::semi_colon, "expected a ';' after declaration");
		LRet VarDeclStmt(initializer,name);
	}

	Stmt Parser::run_stmt(){
		LFn;
		QString str = expect(Tokty::l_string, "expected a filename").lexeme();
		expect(Tokty::semi_colon,"expected a ';'");
		LRet RunStmt(str);
	}

	Stmt Parser::if_stmt(){
		LFn;
		Expr expr = expression();
		if (match(Tok::k_matches))
			LRet finish_match(expr);
		Stmt i_blk = stmt(true);
		Stmt e_blk = match(Tok::k_else)?stmt(true):NullStmt();
		LRet IfStmt(expr,i_blk,e_blk);
	}

	Stmt Parser::match_stmt(){
		LFn;
		LRet finish_match(expression());
	}

	Stmt Parser::finish_match(Expr m){
		LFn;
		expect(Tok::l_brace, "Expected a '{'");
		std::vector<Branch> br;
		while(!check(Tok::r_brace)){
			Expr e = match(Tok::star)?LitExpr(Object()):expression(); // * (wildcard) -> void literal, else call expression()
			Stmt s = match(Tok::colon)?stmt():block(); // ':'  --  one stmt, else call block()
			br.push_back(Branch(e,s)); // create a branch
		}
		expect(Tok::r_brace, "Expected a '}'");
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
		expect(Tok::semi_colon,"expected a ';'");
		LRet ExprStmt(expr);
	}

	Stmt Parser::for_stmt(){
		LFn;
		expect(Tok::l_paren, "expected a '('");
		Stmt for_init = match(Tok::semi_colon)?NullStmt():
											   match(Tok::k_var)?var_decl_stmt():
																 expr_stmt();
		Expr for_cond = check(Tok::semi_colon)?LitExpr(Object("bool",true)):
											   expression();
		expect(Tok::semi_colon,"expected a ';'");
		Expr for_act = check(Tok::r_paren)?NullExpr():expression();
		expect(Tok::r_paren,"expected a ')'");
		Stmt body = stmt(true);
		if (for_act.type() != Expr::Empty){
			body *= BlockStmt(std::vector<Stmt>({body,ExprStmt(for_act)}));
		}
		body *= WhileStmt(for_cond,body);
		if (for_init.type() != Stmt::Empty){
			body *= BlockStmt(std::vector<Stmt>({for_init,body}));
		}
		LRet body;
	}

	Stmt Parser::fn_def_stmt(){
		Token& name = expect(Tok::identifier, "expected an identifier");
		std::vector<Token> params;
		expect(Tok::l_paren,"expected a '('");
		if (!check(Tok::r_paren)){
			do{
				params.push_back(advance());
			} while(match(Tok::comma));
		}
		expect(Tok::r_paren,"expected a ')'");
		Stmt body = block(false);
		return FnStmt(name,params,body);
	}

	Stmt Parser::ret_stmt(){
		Token& keywd = prev();
		Expr expr = check(Tok::semi_colon)?LitExpr(new Object()):expression();
		expect(Tok::semi_colon,"expected a ';'");
		return RetStmt(expr,keywd);
	}

	Stmt Parser::assert_stmt(){
		Token& tok = peek(-1);
		Expr expr = expression();
		int code = 0xFF;
		QString msg = QString("(%2:%3) ")
					  .arg(unit->filename())
					  .arg(tok.line())
					  .arg(tok.col());
		if (match(Tok::colon)){
			bool has_code = false;
			if(match(Tok::l_real)){
				has_code = true;
				code = peek(-1).literal().as<double>();
				if (!match(Tok::comma))
					goto WRAP_UP;
			}
			if (!has_code){
				msg += expect(Tok::l_string, "expected an exit message").literal().to_string();
			}
			else if (match(Tok::l_string)){
				msg += peek(-1).literal().to_string();
			}
			else msg += "assertion failed";
		}
		else msg += "assertion failed";
WRAP_UP:
		expect(Tok::semi_colon, "expected a ';'");
		return AssertStmt(expr,code,msg);
	}

	//expr
	Expr Parser::expression(bool disable){
		LFn;
		Expr exp = conditional();//goto next precedence
		if (!disable && check(Tok::comma)){
			std::vector<Expr> cex;
			cex.push_back(exp);
			while(match(Tok::comma)){
				cex.push_back(conditional()); //goto next precedence
			}
			exp *= CSExpr(cex);
		}
		LRet exp;
	}

	Expr Parser::conditional(){
		LFn;
		Expr expr = logical_or();
		if (match({Tok::assign,Tok::mult_asgn,Tok::div_asgn,Tok::plus_asgn,Tok::minus_asgn,Tok::exp_asgn})){
			Token& op = prev();
			Expr right = conditional();
			expr *= AssignExpr(expr,op,right);
		}
		else if (match(Tok::ques_mk)){
			Expr l = logical_or();
			expect(Tok::colon, "expected a ':'");
			Expr r = logical_or();
			expr *= CndtnlExpr(expr,l,r);
		}
		LRet expr;
	}

	Expr Parser::logical_or(){
		LFn;
		Expr expr = logical_and();
		while (match(Tok::vert_vert)){
			Token& op = prev();
			Expr right = logical_and();
			expr *= LogicalExpr(expr,op,right);
		}
		LRet expr;
	}

	Expr Parser::logical_and(){
		LFn;
		Expr expr = equality();
		while (match(Tok::amp_amp)){
			Token& op = prev();
			Expr right = equality();
			expr *= LogicalExpr(expr,op,right);
		}
		LRet expr;
	}

	Expr Parser::equality(){
		LFn;
		Expr expr = comparison();
		while (match({Tok::equal,Tok::bang_equal})){
			Token& op = prev();
			Expr right = comparison();
			expr *= BinExpr(expr,op,right);
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
			expr *= BinExpr(expr,op,right);
		}
		LRet expr;
	}

	Expr Parser::terms(){
		LFn;
		Expr expr = term();
		while(match({Tok::plus, Tok::minus})){
			Token& op = prev();
			Expr right = term();
			expr *= BinExpr(expr,op,right);
		}
		LRet expr;
	}

	Expr Parser::term(){
		LFn;
		Expr expr = refer();

		while (match({Tok::slash,Tok::star})){
			Token& op = prev();
			Expr right = refer();
			expr *= BinExpr(expr,op,right);
		}

		LRet expr;
	}

	Expr Parser::refer() {
		LFn;
		Expr expr = exponent();
		if (match(Tok::fat_arrow)) {
			Token& op = prev();
			expr *= RefExpr(expr, op, conditional());
		}
		LRet expr;
	}

	Expr Parser::exponent(){
		Expr expr = unary();
		while (match(Tok::caret)){
			Token& op = prev();
			Expr right = unary();
			expr *= BinExpr(expr,op,right);
		}
		return expr;
	}

	Expr Parser::unary(){
		LFn;
		if		(match({Tok::bang,Tok::minus}			)){
			Token& op = prev();
			Expr right = unary();
			LRet PrefxExpr(right,op);
		}
		else if (match(Tok::plus						)){
			Token& op = prev();
			error(peek(),QString("unary operator `+`")
						.append(" not supported"));
			Expr right = unary();
			LRet PrefxExpr(right,op);
		}
		else if (match({Tok::incr,Tok::decr}			)){
			Token& op = prev();
			Expr right = unary();
			LRet PrefxExpr(right,op);
		}
		else if (peek().is_bin_op(						)){
			Token& op = advance();
			error(peek(),QString("binary operator `")
						.append(op.lexeme()).append("` requires 2 operands"));

			Expr right = unary();
			LRet PrefxExpr(right,op);
		}
		Expr rval = rvalue();
		if		(!rval.is(Expr::VarAcsr				)){
			LRet rval;
		}
		if (check(Tok::incr)||check(Tok::decr			)){
			while	(match({Tok::incr,Tok::decr}			)){
				Token& op = prev();
				rval *= PstfxExpr(rval, op);
			}
		}

		LRet rval;
	}

	Expr Parser::rvalue(){
		LFn;
		LRet call();
	}


	Expr Parser::call(){
		LFn;
		Expr expr = primary();

		while(true){
			if (match(Tok::l_paren)){
				expr *= finish_call(expr);
			} else {
				break;
			}
		}
		LRet expr;
	}

	Expr Parser::finish_call(Expr callee){
		LFn;
		std::vector<Expr> args;
		if (!check(Tok::r_paren)){
			do {
				Expr arg = expression(true);
				args.push_back(arg);
				Log ls("FINISH_CALL_ARG_TY") ls(t_cast<int>(arg.type()));
			} while (match(Tok::comma));
		}
		Token& paren = expect(Tok::r_paren, "expected a ')'");
		LRet FnCallExpr(callee,args,paren);
	}

	Expr Parser::primary(){
		LFn;
		if		(match(Tok::l_null							)){
			LRet LitExpr(Object(Trait("null"),QVariant(0)));
		}
		else if	(check(Tok::identifier						)){
			Expr ex = VarAcsrExpr(advance());
			LRet ex;
		}
		else if (match(Tok::l_true							)){
			LRet LitExpr(Object(Trait("bool"),QVariant(true)));
		}
		else if (match(Tok::l_false							)){
			LRet LitExpr(Object(Trait("bool"),QVariant(false)));
		}
		else if (match(Tok::l_brace							)){
			Expr ex = array();
			LRet ex;
		}
		else if (match({Tok::l_string,Tok::l_real,Tok::l_char})){
			LRet LitExpr(prev().literal());
		}
		else if (match(Tok::dollar							)){
			Expr ex = spec_data();
			LRet ex;
		}
		else if (match(Tok::l_paren							)){
			Expr ex = group();
			LRet ex;
		}
		else									  {
			error(peek(),"expected an expression");
			LRet NullExpr();
		}
	}

	constexpr qulonglong
	h(const char* string)
	{
		qulonglong hash = 0xEEF7UL;
		while (*string)
		{
			hash ^= (qulonglong)(*string++);
			hash *= 0xAE03UL;
		}

		return hash;
	}

	Expr Parser::spec_data(){
		LFn;
		//assumes '$' is eaten
		Token& t = expect(Tok::identifier,"expected an identifier");
		switch(h(t.lexeme().toStdString().c_str())){
			case h("a"):
				LRet assoc();
				break;
			case h("h"):
				LRet map();
			default:
				LThw make_error(t,"invalid data specifier");
		}
	}

	Expr Parser::assoc(){
		LFn;
		std::vector<pair<Expr,Expr>> inits;
		expect(Tok::l_brace,"expected a '{'");
		if (!check(Tok::r_brace)){
			do{
				Expr x = expression(true);
				expect(Tok::colon,"expected a ':'");
				Expr y = expression(true);
				inits.push_back(pair<Expr,Expr>(x,y));
			}while (!is_at_end() && peek().type() != Tok::r_brace && match(Tok::comma));
		}
		expect(Tok::r_brace, "expected a '}'");
		LRet AssocExpr(inits);
	}

	Expr Parser::map(){
		LFn;
		std::vector<pair<Expr,Expr>> inits;
		expect(Tok::l_brace,"expected a '{'");
		if (!check(Tok::r_brace)){
			do{
				Expr x = expression(true);
				expect(Tok::colon,"expected a ':'");
				Expr y = expression(true);
				inits.push_back(pair<Expr,Expr>(x,y));
			}while (!is_at_end() && peek().type() != Tok::r_brace && match(Tok::comma));
		}
		expect(Tok::r_brace, "expected a '}'");
		LRet HashExpr(inits);
	}

	Expr Parser::array(){
		LFn;
		Token& t = prev();
		std::vector<Expr> inits{};
		if (match(Tok::r_brace))
			LRet ArrayExpr(inits);
		inits.push_back(expression(true));
		for (;!is_at_end();){
			if (match(Tok::r_brace))
				LRet ArrayExpr(inits);
			inits.push_back((expect(Tok::comma, "expected a ','"),expression(true)));
		}
		LThw make_error(t,"expected a '}'");
	}

	Expr Parser::group(){
		LFn;
		Expr expr = expression();
		expect(Tok::r_paren, "expected a ')'");
		LRet GroupExpr(expr);
	}
}
