#include "resolver.h"

using namespace mere;


void Resolver::error(const Token& tok, const QString& errmsg) const {
	unit->report(tok.loc(), "resolve-error", errmsg);
}

void Resolver::declare(const Token& name){
	if (scopes.empty()) return;
	scopes.top()[name.lexeme()] = false;
}

void Resolver::define(const Token& name){
	if (scopes.empty()) return;
	scopes.top()[name.lexeme()] = true;
}

void Resolver::resolve_block(const Stmt& stmt){
	LFn;
	beg_scope();
	for (const Stmt& s : stmt.block().stmts()){
		resolve(s);
	}
	end_scope();
	LVd;
}

void Resolver::resolve_var_decl(const Stmt& stmt){
	LFn;
	declare(stmt.var().name());
	if (!stmt.var().init().is(Expr::Empty)){
		resolve(stmt.var().init());
	}
	define(stmt.var().name());
	LVd;
}

void Resolver::resolve_fn_decl(const Stmt& stmt){
	LFn;
	define(stmt.fn().name());
	resolve_function(stmt);
	LVd;
}

void Resolver::resolve_expr_stmt(const Stmt& stmt){
	LFn;
	resolve(stmt.expr().expr());
	LVd;
}

void Resolver::resolve_if(const Stmt& stmt){
	LFn;
	auto& if_else = stmt.if_else();
	resolve(if_else.condition());
	resolve(if_else.if_block());
	resolve(if_else.else_block());
	LVd;
}

void Resolver::resolve_print(const Stmt& stmt){
	LFn;
	resolve(stmt.print().expr());
	LVd;
}

void Resolver::resolve_println(const Stmt& stmt){
	LFn;
	resolve(stmt.println().expr());
	LVd;
}

void Resolver::resolve_while(const Stmt& stmt){
	LFn;
	resolve(stmt.while_loop().condition());
	resolve(stmt.while_loop().body());
	LVd;
}

void Resolver::resolve_ret(const Stmt& stmt){
	LFn;
	if (!stmt.ret().value().is(Expr::Empty))
		resolve(stmt.ret().value());
	LVd;
}

void Resolver::resolve_assert(const Stmt& stmt){
	LFn;
	resolve(stmt.assertion().value());
	LVd;
}

void Resolver::resolve_match(const Stmt& stmt){
	LFn;
	resolve(stmt.match().match());
	const std::vector<Branch>& branches = stmt.match().branches();
	for (const Branch& branch : branches){
		resolve(branch.expr());
		resolve(branch.stmt());
	}
	LVd;
}

void Resolver::resolve_function(const Stmt& stmt){
	LFn;
	beg_scope();
	for (const Token& param : stmt.fn().params()){
		Logp(QString( "resolving var decl " + param.lexeme()));
		define(param);
	}
	resolve(stmt.fn().body());
	end_scope();
	LVd;
}

void Resolver::resolve(const Stmt& stmt){
	LFn;
	Log ls("Resolving stmt-") ls(t_cast<int>(stmt.type()));
	if (!stmt.is(Stmt::Empty) && !stmt.is(Stmt::Invalid)){
		switch(stmt.type()){
			case Stmt::ExprStmt:resolve_expr_stmt(stmt);break;
			case Stmt::Print:	resolve_print(stmt);	break;
			case Stmt::Println:	resolve_println(stmt);	break;
			case Stmt::If:		resolve_if(stmt);		break;
			case Stmt::While:	resolve_while(stmt);	break;
			case Stmt::Block:	resolve_block(stmt);	break;
			case Stmt::VarDecl:	resolve_var_decl(stmt);	break;
			case Stmt::Function:resolve_fn_decl(stmt);	break;
			case Stmt::Return:	resolve_ret(stmt);		break;
			case Stmt::Assert:	resolve_assert(stmt);	break;
			case Stmt::Match:	resolve_match(stmt);	break;
			case Stmt::Run:		resolve_run(stmt);		break;
			default: Log1("  > skipping stmt...\n");
		}
	}
	LVd;
}

void Resolver::resolve_var(const Expr& expr){
	LFn;
	auto& var = expr.var();
	if (!scopes.empty() &&
			scopes.top().value(var.accessor().lexeme(), true) == false) {
		error(var.accessor(),
			  "cannot access itself in its own initializer");
	}
	resolve_local(expr,var.accessor());
	LVd;
}



void Resolver::resolve_assign(const Expr& expr) {
	LFn;
	resolve(expr.assign().right());
	resolve(expr.assign().left());
	LVd;
}

void Resolver::resolve_bin(const Expr& expr){
	LFn;
	resolve(expr.bin().left());
	resolve(expr.bin().right());
	LVd;
}

void Resolver::resolve_unary(const Expr& expr){
	LFn;
	resolve(expr.unary().expr());
	LVd;
}

void Resolver::resolve_group(const Expr& expr){
	LFn;
	resolve(expr.group().expr());
	LVd;
}

void Resolver::resolve_logical(const Expr& expr){
	LFn;
	resolve(expr.logical().left());
	resolve(expr.logical().right());
	LVd;
}

void Resolver::resolve_refer(const Expr& expr){
	LFn;
	resolve(expr.ref().right());
	resolve(expr.ref().left());
	LVd;
}

void Resolver::resolve_call(const Expr& expr){
	LFn;
	resolve(expr.call().callee());
	for (const Expr& arg : expr.call().arguments()){
		resolve(arg);
	}
	LVd;
}

void Resolver::resolve_ternary(const Expr& expr){
	LFn;
	auto& ternary = expr.ternary();
	resolve(ternary.condition());
	resolve(ternary.left());
	resolve(ternary.right());
	LVd;
}

void Resolver::resolve_local(const Expr& expr, const Token& name){
	for (int i = scopes.size() - 1; i >= 0; i--) {
		if (scopes[i].contains(name.lexeme())) {
			intp->resolve(expr, scopes.size() - 1 - i);
			return;
		}
	}
}

void Resolver::resolve(const Expr& expr){
	LFn;
	Log ls("Resolving expr-") ls(QString::number((short)expr.type()));
	if (!expr.is(Expr::Empty)){
		switch(expr.type()){
			case Expr::Group:		resolve_group(expr);	break;
			case Expr::Binary:		resolve_bin(expr);		break;
			case Expr::Postfix:	  //resolve_unary(expr);	break; (falls thru)
			case Expr::Prefix:		resolve_unary(expr);	break;
			case Expr::Literal:		resolve_lit(expr);		break;
			case Expr::Logical:		resolve_logical(expr);	break;
			case Expr::VarAcsr:		resolve_var(expr);		break;
			case Expr::Assign:		resolve_assign(expr);	break;
			case Expr::Refer:		resolve_refer(expr);	break;
			case Expr::FuncCall:	resolve_call(expr);		break;
			case Expr::Conditional:	resolve_ternary(expr);	break;
			default:;
		}
	}
	LVd;
}

void Resolver::resolve(const std::vector<Stmt>& stmts){
	for (const Stmt& stmt : stmts){
		resolve(stmt);
	}
}

void Resolver::resolve(IntpUnit u) {
	if (!u->success())
		return;
	unit = u;
	resolve(unit->ast());
}
