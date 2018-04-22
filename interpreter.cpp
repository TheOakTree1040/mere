
#include "natives.h"
#include "interpreter.h"
#include <cmath>
#include "mere_math.h"
#include <QMessageBox>
#include "environment.h"
#include <QMessageBox>
#include "merecallable.h"
#include <QDateTime>
#include <ctime>
#include <cmath>
#define GOTO_OP_ASGN(TOK) right_val_op = TOK; goto RET_OP_ASGN;
#define define_native_function(NAME,NAT) globals->define(NAME,Object(Trait("function").as_fn(),Var::fromValue(NAT)))
Interpreter::Interpreter(){
    LFn;
	define_native_function("sin", MereCallable(CALLABLE{
		Q_UNUSED(interpreter);
	    CHECK("sin", QVector<TString>{"real"});
	    return Object(::sin(arguments[0].as<double>()));
    }).set_arity(1));
	define_native_function("cos", MereCallable(CALLABLE{
		Q_UNUSED(interpreter);
	    CHECK("cos",QVector<TString>{"real"});
	    return Object(::cos(arguments[0].as<double>()));
    }).set_arity(1));
	define_native_function("tan", MereCallable(CALLABLE{
		Q_UNUSED(interpreter);
	    CHECK("tan",QVector<TString>{"real"});
	    return Object(::tan(arguments[0].as<double>()));
    }).set_arity(1));
	define_native_function("clock", MereCallable(CALLABLE{
		Q_UNUSED(interpreter);
	    CHECK("clock", QVector<TString>{});
	    return Object(t_cast<double>(::clock()));
    }));
	define_native_function("time", MereCallable(CALLABLE{
		Q_UNUSED(interpreter);
	    CHECK("time", QVector<TString>{});
	    return Object(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
	}));
	LVd;
}


Object Interpreter::eval_pstfx(Expr expr, bool dd){
	LFn;
	Expr rex = expr->expr;
	if (dd){
		expr->expr = nullptr;
	}
	Object r = evaluate(rex,dd);
	Token op(*(expr->op));
	if (!r.trait().is_lvalue()){
		LThw RuntimeError(op,"Expected a lvalue.");
	}
	switch(op.ty){
		case Tok::INCR:
			if(IS_NUM && r.trait().is_lvalue()){
				LRet r.postfix(1);
			}
			break;
		case Tok::DECR:
			if(IS_NUM && r.trait().is_lvalue()){
				LRet r.postfix(-1);
			}
			break;
		default:;
	}
	TString errmsg = "Operand type mismatch: '";
	errmsg.append(r.trait().id()).append("' [");
	errmsg += op.lexeme + "].";
	LThw RuntimeError(op,errmsg);
}
Object Interpreter::eval_prefx(Expr expr, bool dd){
	LFn;
	Expr ex = expr->expr;
	Token* tptr = expr->op;
	if (dd){
		expr->expr = nullptr;
	}
	Object r = evaluate(ex, dd);
	Token op(*tptr);

	switch(op.ty){
		case Tok::MINUS:
			if (IS_NUM){
				LRet -(r.as<double>());
			}
			break;
		case Tok::EXCL:
			if (IS_NUM){
				LRet !r.to_bool();
			}
			break;
		case Tok::INCR:
			if (!r.trait().is_lvalue()){
				LThw RuntimeError(op, "Expected an lvalue.");
			}
			if(IS_NUM && r.trait().is_lvalue()){
				LRet r.prefix(1);
			}
			break;
		case Tok::DECR:
			if (!r.trait().is_lvalue()){
				LThw RuntimeError(op, "Expected an lvalue.");
			}
			if(IS_NUM && r.trait().is_lvalue()){
				LRet r.prefix(-1);
			}
			break;
		default:;
	}
	TString errmsg = "Operand type mismatch: [";
	errmsg.append(op.lexeme).append("] '").append(r.trait().id()).append("'.");
	LThw RuntimeError(op,errmsg);
}
Object Interpreter::eval_lit(Expr expr, bool){
	LFn;
    Object ro(*(expr->lit));
	LRet ro;
}
Object Interpreter::eval_group(Expr expr, bool dd){
	LFn;
	Expr group = expr->expr;
	if (dd){
		expr->expr = nullptr;
	}
	LRet evaluate(group,dd);
}
Object Interpreter::eval_binary(Expr expr, bool dd){
	LFn;
	Expr ex = expr->expr;
	Expr rex = expr->right;
	if (dd){
		expr->expr = nullptr;
		expr->right = nullptr;
	}
	Object l = evaluate(ex, dd);
	Object r = evaluate(rex, dd);
	Token op(*expr->op);
	switch(op.ty){
		case Tok::PLUS:

			if (ARE_NUM){
				LRet OP(double,+);
			}
			if (ARE(Ty::String)){
				LRet OP(TString,+);
			}
			break;
		case Tok::MINUS:
			if (ARE_NUM){
				LRet OP(double,-);
			}
			break;
		case Tok::CARET:
			if (ARE_NUM){
				LRet ::pow(l.as<double>(),r.as<double>());
			}
			break;
		case Tok::STAR:
			if (ARE_NUM){
				LRet OP(double,*);
			}
			break;
		case Tok::SLASH:
			if (ARE_NUM){
				if (r.as<double>() == 0){
					LThw RuntimeError(op,"Division by 0 Error!");
				}
				LRet OP(double,/);
			}
			break;
		case Tok::EQUAL:
			LRet r==l;
		case Tok::N_EQUAL:
			LRet (!(r==l));
		case Tok::GREATER:
			if (ARE_NUM){
				LRet OP(double,>);
			}
			break;
		case Tok::GREATER_EQUAL:
			if (ARE_NUM){
				LRet OP(double,>=);
			}
			break;
		case Tok::LESS:
			if (ARE_NUM){
				LRet OP(double,<);
			}
			break;
		case Tok::LESS_EQUAL:
			if (ARE_NUM){
				LRet OP(double,<=);
			}
			break;
		default:
			LThw RuntimeError(op,"Undefined Binary Operation.");

	}
	TString errmsg = "Operand type mismatch: '";
	errmsg.append(l.trait().id()).append("' ").append(op.lexeme)
			.append(" '").append(r.trait().id()).append("'.");
	LThw RuntimeError(op,errmsg);
}
Object Interpreter::eval_logical(Expr expr, bool dd){
	LFn;
	Expr ex = expr->expr;
	Tok ty = expr->op->ty;
	if (dd){
		expr->expr = nullptr;
	}
	Object l = evaluate(ex, dd);
	if (ty == Tok::VERTVERT){
		if (l.to_bool()){
			LRet l;
		}
	}
	else if (!l.to_bool()){
		LRet l;
	}
	Expr rex = expr->right;
	if (dd){
		expr->right = nullptr;
	}
	Object r = evaluate(rex,dd);
	LRet r;
}
Object Interpreter::eval_var_acsr(Expr expr, bool){
	LFn;
	Object o;
	Object& ref = environment->access(*(expr->var_acsr));//DO NOT TOUCH! Keep this line for catching exceptions
	o.as_acsr_of(ref);
	LRet o;
}
Object Interpreter::eval_asgn(Expr expr, bool dd){
	LFn;
	Expr l = expr->asgn_left, r = expr->asgn_right;
	Tok ty = expr->asgn_op->ty;
	int ln = (expr->asgn_op->ln);
	Token op(*(expr->asgn_op));
	if (dd){
		expr->asgn_left = nullptr;
		expr->asgn_right = nullptr;
	}
	Object ref(evaluate(l,dd));
	Object right(evaluate(r,dd));

	if (!ref.trait().is_lvalue()){
		LThw RuntimeError(op,"Expected an lvalue.");
	}
	if (!ref.trait().is_dynamic() && !ref.trait().has_type_of(right.trait())){
		LThw RuntimeError(*(expr->op), "Attempting to re-type a fixed-type variable.");
	}
	Tok right_val_op = Tok::INVALID;
	switch(ty){
		case Tok::ASSIGN:
			ref.recv(right);
			LRet ref;
		case Tok::MULT_ASGN:
			GOTO_OP_ASGN(Tok::STAR);
		case Tok::DIV_ASGN:
			GOTO_OP_ASGN(Tok::SLASH);
		case Tok::PLUS_ASGN:
			GOTO_OP_ASGN(Tok::PLUS);
		case Tok::MINUS_ASGN:
			GOTO_OP_ASGN(Tok::MINUS);
		case Tok::EXP_ASGN:
			GOTO_OP_ASGN(Tok::CARET);
		default:
			LThw RuntimeError(op,"No such assignment operator.");
	}
	RET_OP_ASGN:;
	Expr lex = LitExpr(ref);
	Expr rex = LitExpr(right);
	LRet ref.recv(evaluate(BinExpr(lex,Token(right_val_op,op.lexeme,Object(),ln),rex),dd));
}
Object Interpreter::eval_refer(Expr expr, bool dd){
	LFn;
	Expr refl = expr->refer_left;
	Expr refr = expr->refer_right;
	if (dd){
		expr->refer_right = nullptr;
		expr->refer_left = nullptr;
	}
	Object rl = evaluate(refl,dd);
	Object rr = evaluate(refr,dd);
	if (!rl.trait().is_lvalue()){
		LThw RuntimeError(*expr->refer_op, "Expected an lvalue.");
	}
	if (!rr.trait().is_lvalue()){
		LThw RuntimeError(*expr->refer_op, "Expected an lvalue (...on the right side, I know.)");
	}
	LRet rl.as_ref_of(rr);
}
Object Interpreter::eval_call(Expr expr, bool dd){
	LFn;
	Expr callee_expr = expr->callee;
	int sz = expr->arguments->size();
    QVector<Object> args(sz);
	Object o;
    for (int i = 0; i != sz; i++){
        o = evaluate(expr->arguments->at(i),dd);
        args[i] = o;
	}
	if (dd){
		expr->callee = nullptr;
		expr->arguments->clear();
	}
	Object callee = evaluate(callee_expr,dd);
	if (!callee.trait().is_fn()){
		LThw RuntimeError(*(expr->call_paren),"Invalid callee! [CalleeType]");
	}
	MereCallable* mc = reinterpret_cast<MereCallable*>(callee.data().data());
	if (sz != mc->arity()){
		LThw RuntimeError(*(expr->call_paren),TString("Expected ") +
						  TString::number(mc->arity()) +
						  " argument(s), but " + TString::number(sz) + " was provided. [CallArity]");
	}

	LRet mc->call(*this,args);
}
Object Interpreter::evaluate(Expr expr, bool dd){
#if _DEBUG
	LFn << "Evaluating expr-" << TString::number((int)expr->type());
#endif
	Object o;
	if (expr){
		try{
			switch(expr->type()){
				case ExprTy::Group:
					o = eval_group(expr,dd);
					break;
				case ExprTy::Binary:
					o = eval_binary(expr,dd);
					break;
				case ExprTy::Postfix:
					o = eval_pstfx(expr,dd);
					break;
				case ExprTy::Prefix:
					o = eval_prefx(expr,dd);
					break;
				case ExprTy::Literal:
					o = eval_lit(expr,dd);
					break;
				case ExprTy::Logical:
					o = eval_logical(expr,dd);
					break;
				case ExprTy::VarAcsr:
					o = eval_var_acsr(expr,dd);
					break;
				case ExprTy::Assign:
					o = eval_asgn(expr,dd);
					break;
				case ExprTy::Refer:
					o = eval_refer(expr,dd);
					break;
				case ExprTy::FuncCall:
					o = eval_call(expr,dd);
					break;
				default:
					LThw RuntimeError(Token(Tok::INVALID,"",Object(),0),
									  "Failed to evaluate expr.");
			}
		} catch(RuntimeError& re){
			delete expr;//not checking do_del 'cause when an error is thrown, it will be del'd by dtor
			LThw re;
		}
		if (dd){
			delete expr;
		}

	}
	LRet o;
}

void Interpreter::exec_expr(Stmt stmt, bool dd){
	LFn;
	Expr ex = stmt->expr;
	if (dd){
		stmt->expr = nullptr;
	}
	evaluate(ex,dd);
	LVoid;
}
void Interpreter::exec_print(Stmt stmt, bool dd){
	LFn;
	Expr ex = stmt->expr;
	if (dd){
		stmt->expr = nullptr;
	}
	Object obj = evaluate(ex,dd);
    TString out = obj.to_string()
              #if _DEBUG
                  + " : " + TString(obj.data().typeName())
              #endif
                  ;
	QMessageBox::information(nullptr,"Info",out);
	LVd;
}
void Interpreter::exec_if(Stmt stmt, bool dd){
	LFn;
	Expr cond = stmt->condition;
	if (dd){
		stmt->condition = nullptr;
	}
	if (evaluate(cond,dd).to_bool()){
		Stmt i_blk = stmt->if_block;
		if (dd){
			stmt->if_block = nullptr;
		}
		execute(i_blk,dd);
	}
	else if (stmt->else_block){
		Stmt e_blk = stmt->else_block;
		if (dd){
			stmt->else_block = nullptr;
		}
		execute(e_blk,dd);
	}
	LVoid;
}
void Interpreter::exec_block(Stmt stmt, bool dd, Environment env){
	LFn;
	exec_block(stmt->block,dd,env);
	LVoid;
}
void Interpreter::exec_block(Stmts *stmts, bool, Environment env){
	LFn;
	int sz = stmts->size();
	Environment outer = environment;
	environment = env?env:new EnvImpl(outer);
	for (int i = 0; i != sz; i++)
		execute(stmts->at(i),false);
	delete environment;
	environment = outer;
	LVd;
}
void Interpreter::exec_while(Stmt stmt, bool){
	LFn;
	while (evaluate(stmt->cont_condit,false).to_bool()){
		execute(stmt->while_block,false);
	}
	LVd;
}
void Interpreter::exec_var_decl(Stmt stmt, bool dd){
	LFn;
	Expr expr = stmt->init;
	if (dd){
		stmt->init = nullptr;
    }
	environment->define(*(stmt->var_name),evaluate(expr,dd));
	LVd;
}
void Interpreter::exec_fn_decl(Stmt stmt, bool){
	LFn;
	MereCallable mc(stmt);
    mc.set_onstack(false);
	environment->define(*(stmt->fn_name),Object(Trait("function").as_fn(),Var::fromValue(mc)));
	LVd;
}
void Interpreter::exec_ret(Stmt stmt, bool dd){
	LFn;
    Object obj = evaluate(stmt->retval,dd);
	LThw Return(obj);
}
void Interpreter::execute(Stmt stmt, bool dd){
	LFn;
#if _DEBUG
	Log << "Interpreting stmt" << t_cast<int>(stmt->type());
#endif
	if (stmt){
		try{
			switch(stmt->type()){
				case StmtTy::Expr:
					exec_expr(stmt,dd);
					break;
				case StmtTy::Print:
					exec_print(stmt,dd);
					break;
				case StmtTy::If:
					exec_if(stmt,dd);
					break;
				case StmtTy::While:
					exec_while(stmt,dd);
					break;
				case StmtTy::Block:
					exec_block(stmt,dd);
					break;
				case StmtTy::VarDecl:
					exec_var_decl(stmt,dd);
					break;
				case StmtTy::Function:
					exec_fn_decl(stmt,dd);
					dd=false;
					break;
                case StmtTy::Return:
					exec_ret(stmt,dd);
					break;
				default:;
			}
		} catch(RuntimeError& re){
			delete stmt;//Same thing applies here (check out interpret(Expr))
			LThw re;
		}
		if (dd){
			delete stmt;
		}

	}
	LVd;
}

void Interpreter::interpret(Stmts stmts){
	LFn;
	try{
		int sz = stmts.size();
		for (int i = 0; i != sz; i++){
			execute(stmts.takeAt(0),true);
		}
	}
	catch(RuntimeError& re){
		MereMath::runtime_error(re);
	}
	catch(Return& ret){
		Q_UNUSED(ret);
		MereMath::error(0,"Invalid Return. [inv_ret]");
	}
	catch(ArgumentError& arg_err){
		QString errmsg = "[";
		errmsg.append(arg_err.callee).append("] ");
		errmsg.append("Expected a ").append(arg_err.expect).append(" instance but a ");
		errmsg.append(arg_err.received).append(" instance was provided. [arg_err]");
		MereMath::error(0,errmsg);
	}
	catch(ArityMismatchError& ame){
		QString errmsg = "[";
		errmsg.append(ame.callee).append("] ");
		errmsg.append("Expected ").append(TString::number(ame.expect)).append(" argument(s) ");
		errmsg.append("but ").append(TString::number(ame.received)).append(" was provided.");
		MereMath::error(0,errmsg);
	}

	catch(std::runtime_error& re){
		MereMath::error(0,TString(re.what()) + " [runtime_err]");
	}
    catch(std::bad_alloc& ba){
        MereMath::error(0,TString(ba.what()) + " [bad_alloc]");
    }
	catch(std::exception& ex){
		MereMath::error(0,TString(ex.what()) + " [exception]");
    }

	LVoid;
}

Environment Interpreter::global(){
	return globals;
}

Interpreter::~Interpreter(){
	reset(nullptr);
}

void Interpreter::reset(Environment envptr){
	if (globals!=environment){
		delete environment;
	}
	delete globals;
	globals = environment = envptr;
}
