
#include "interpreter.h"
#include "natives.h"
#include "core.h"
#include "runtimeerror.h"

#include <QDateTime>
#include <QFile>

#if T_GUI
#include <QMessageBox>
#endif // T_GUI

#include <ctime>
#include <cmath>
#include <iostream>

#define GOTO_OP_ASGN(TOK) right_val_op = TOK; goto RET_OP_ASGN;
#define define_native_function(NAME,NAT) globals->define(NAME,Object(Trait("function").make_function(),Var::fromValue(NAT)))
#define clean_up if(hndl)

using namespace mere;
Interpreter::Interpreter(){
	LFn;
	define_native_function("sin", MereCallable(CALLABLE{
												   Q_UNUSED(interpreter);
												   CHECK("sin", std::vector<QString>{"real"});
												   return Object(::sin(arguments[0].as<double>()));
											   }).set_arity(1));
	define_native_function("cos", MereCallable(CALLABLE{
												   Q_UNUSED(interpreter);
												   CHECK("cos",std::vector<QString>{"real"});
												   return Object(::cos(arguments[0].as<double>()));
											   }).set_arity(1));
	define_native_function("sqrt", MereCallable(CALLABLE{
													Q_UNUSED(interpreter);
													CHECK("sqrt",std::vector<QString>{"real"});
													return Object(::sqrt(arguments[0].as<double>()));
												}).set_arity(1));
	define_native_function("tan", MereCallable(CALLABLE{
												   Q_UNUSED(interpreter);
												   Q_UNUSED(arguments);
												   CHECK("tan",std::vector<QString>{"real"});
												   return Object(::tan(arguments[0].as<double>()));
											   }).set_arity(1));
	define_native_function("clock", MereCallable(CALLABLE{
													 Q_UNUSED(interpreter);
													 Q_UNUSED(arguments);
													 CHECK("clock", std::vector<QString>{});
													 return Object(t_cast<double>(::clock()));
												 }));
	define_native_function("time", MereCallable(CALLABLE{
													Q_UNUSED(interpreter);
													CHECK("time", std::vector<QString>{});
													return Object(QDateTime::currentDateTime().toString("yyyy-MM-hndl hh:mm:ss"));
												}));

	LVd;
}

Object Interpreter::eval_pstfx(C_EXPR_REF expr, bool hndl){
	LFn;
	Object r = evaluate(expr.unary().expr(),hndl);
	if (!r.trait().is_lvalue()){
		LThw RuntimeError(expr.unary().op(),"Expected a lvalue.");
	}
	switch(expr.unary().op().type()){
		case Tok::incr:
			if(IS_NUM && r.trait().is_lvalue()){
				LRet r.postfix(1);
			}
			break;
		case Tok::decr:
			if(IS_NUM && r.trait().is_lvalue()){
				LRet r.postfix(-1);
			}
			break;
		default:;
	}
	QString errmsg = "Operand type mismatch: '";
	errmsg.append(r.trait().id()).append("' [");
	errmsg += expr.unary().op().lexeme() + "].";
	LThw RuntimeError(expr.unary().op(),errmsg);
}
Object Interpreter::eval_prefx(C_EXPR_REF expr, bool hndl){
	LFn;
	Object r = evaluate(expr.unary().expr(), hndl);
	switch(expr.unary().op().type()){
		case Tok::minus:
			if (IS_NUM){
				LRet -(r.as<double>());
			}
			break;
		case Tok::bang:
			if (IS_NUM){
				LRet !r.to_bool();
			}
			break;
		case Tok::incr:
			if (!r.trait().is_lvalue()){
				LThw RuntimeError(expr.unary().op(), "Expected an lvalue.");
			}
			if(IS_NUM && r.trait().is_lvalue()){
				LRet r.prefix(1);
			}
			break;
		case Tok::decr:
			if (!r.trait().is_lvalue()){
				LThw RuntimeError(expr.unary().op(), "Expected an lvalue.");
			}
			if(IS_NUM && r.trait().is_lvalue()){
				LRet r.prefix(-1);
			}
			break;
		default:;
	}
	QString errmsg = "Operand type mismatch: [";
	errmsg.append(expr.unary().op().lexeme()).append("] '").append(r.trait().id()).append("'.");
	LThw RuntimeError(expr.unary().op(),errmsg);
}
Object Interpreter::eval_lit(C_EXPR_REF expr, bool){
	LFn;
	LRet expr.lit().lit();
}
Object Interpreter::eval_group(C_EXPR_REF expr, bool hndl){
	LFn;
	LRet evaluate(expr.group().expr(),hndl);
}
Object Interpreter::eval_binary(C_EXPR_REF expr, bool hndl){
	LFn;
	Object l = evaluate(expr.bin().left(), hndl);
	Object r = evaluate(expr.bin().right(), hndl);
	switch(expr.bin().op().type()){
		case Tok::plus:

			if (ARE_NUM){
				LRet OP(double,+);
			}
			if (ARE(Ty::String)){
				LRet OP(QString,+);
			}
			break;
		case Tok::minus:
			if (ARE_NUM){
				LRet OP(double,-);
			}
			break;
		case Tok::caret:
			if (ARE_NUM){
				LRet ::pow(l.as<double>(),r.as<double>());
			}
			break;
		case Tok::star:
			if (ARE_NUM){
				LRet OP(double,*);
			}
			break;
		case Tok::slash:
			if (ARE_NUM){
				if (r.as<double>() == 0){
					LThw RuntimeError(expr.bin().op(),"Division by 0 Error!");
				}
				LRet OP(double,/);
			}
			break;
		case Tok::equal:
			LRet r==l;
		case Tok::bang_equal:
			LRet (!(r==l));
		case Tok::greater:
			if (ARE_NUM){
				LRet OP(double,>);
			}
			break;
		case Tok::greater_equal:
			if (ARE_NUM){
				LRet OP(double,>=);
			}
			break;
		case Tok::less:
			if (ARE_NUM){
				LRet OP(double,<);
			}
			break;
		case Tok::less_equal:
			if (ARE_NUM){
				LRet OP(double,<=);
			}
			break;
		default:
			LThw RuntimeError(expr.bin().op(),"Undefined Binary Operation.");

	}
	QString errmsg = "Operand type mismatch: '";
	errmsg.append(l.trait().id()).append("' ").append(expr.bin().op().lexeme())
			.append(" '").append(r.trait().id()).append("'.");
	LThw RuntimeError(expr.bin().op(),errmsg);
}
Object Interpreter::eval_logical(C_EXPR_REF expr, bool hndl){
	LFn;
	Tokty ty = expr.logical().op().type();
	Object l = evaluate(expr.logical().left(), hndl);
	if (ty == Tok::vert_vert){
		if (l.to_bool()){
			LRet l;
		}
	}
	else if (!l.to_bool()){
		LRet l;
	}
	LRet evaluate(expr.logical().right(),hndl);
}
Object Interpreter::eval_var_acsr(C_EXPR_REF expr, bool){
	LFn;
	Object o;
	Object& ref = environment->access(expr.var().accessor());//DO NOT TOUCH! Keep this line for catching exceptions
	o.as_acsr_of(ref);
	LRet o;
}
Object Interpreter::eval_asgn(C_EXPR_REF expr, bool hndl){
	LFn;
	Token& op = expr.assign().op();
	Expr l = expr.assign().left(), r = expr.assign().right();
	Tokty ty = op.type();
	Object ref(evaluate(l,hndl));
	Object right(evaluate(r,hndl));

	if (!ref.trait().is_lvalue()){
		LThw RuntimeError(op,"Expected an lvalue.");
	}
	if (!ref.trait().is_dynamic() && !ref.trait().has_type_of(right.trait())){
		LThw RuntimeError(op, "Attempting to re-type a fixed-type variable.");
	}
	Tokty right_val_op = Tok::invalid;
	switch(ty){
		case Tok::assign:
			ref.recv(right);
			LRet ref;
		case Tok::mult_asgn:
			GOTO_OP_ASGN(Tok::star);
		case Tok::div_asgn:
			GOTO_OP_ASGN(Tok::slash);
		case Tok::plus_asgn:
			GOTO_OP_ASGN(Tok::plus);
		case Tok::minus_asgn:
			GOTO_OP_ASGN(Tok::minus);
		case Tok::exp_asgn:
			GOTO_OP_ASGN(Tok::caret);
		default:
			LThw RuntimeError(op,"No such assignment operator.");
	}
RET_OP_ASGN:;
	Expr lex = LitExpr(ref);
	Expr rex = LitExpr(right);
	LRet ref.recv(evaluate(BinExpr(lex,Token(right_val_op,op.lexeme(),Object(),op.loc()),rex),hndl));
}
Object Interpreter::eval_refer(C_EXPR_REF expr, bool hndl){
	LFn;
	Object rl = evaluate(expr.ref().left(),hndl);
	Object rr = evaluate(expr.ref().right(),hndl);
	if (!rl.trait().is_lvalue()){
		LThw RuntimeError(expr.ref().op(), "Expected an lvalue.");
	}
	if (!rr.trait().is_lvalue()){
		LThw RuntimeError(expr.ref().op(), "Expected an lvalue (...on the right side, I know.)");
	}
	LRet rl.as_ref_of(rr);
}
Object Interpreter::eval_call(C_EXPR_REF expr, bool hndl){
	LFn;
	std::vector<Ref<Expr>> args_exprs = expr.call().arguments();
	int sz = args_exprs.size();
	std::vector<Object> args(sz);
	Object o;
	for (int i = 0; i != sz; i++){
		o = evaluate(args_exprs[i].get(),hndl);
		args[i] = o;
	}
	Object callee = evaluate(expr.call().callee(),hndl);
	if (!callee.trait().is_function()){
		LThw RuntimeError(expr.call().op(),"Invalid callee! [CalleeType]");
	}
	MereCallable* mc = t_cast<MereCallable*>(callee.data().data());
	if (sz != mc->arity()){
		LThw RuntimeError(expr.call().op(),QString("Expected ") +
						  QString::number(mc->arity()) +
						  " argument(s), but " + QString::number(sz) + " was provided. [CallArity]");
	}

	LRet mc->call(*this,args);
}
//Object Interpreter::eval_cast(Expr, bool){
//#error Implement CAST
//#error Put cast into the switch in Intp::evaluate
//}
Object Interpreter::eval_ternary(C_EXPR_REF expr, bool hndl){
	bool b = evaluate(expr.ternary().condition(),hndl).to_bool();
	return b?evaluate(expr.ternary().left(),hndl):
			 evaluate(expr.ternary().right(),hndl);
}

Object Interpreter::evaluate(C_EXPR_REF expr, bool hndl){
	LFn;
	Log ls("Evaluating expr-") ls(QString::number((short)expr.type()));
	Object o;
	if (!expr.is(Expr::Empty)){
		try{
			switch(expr.type()){
				case Expr::Group:
					o = eval_group(expr,hndl);
					break;
				case Expr::Binary:
					o = eval_binary(expr,hndl);
					break;
				case Expr::Postfix:
					o = eval_pstfx(expr,hndl);
					break;
				case Expr::Prefix:
					o = eval_prefx(expr,hndl);
					break;
				case Expr::Literal:
					o = eval_lit(expr,hndl);
					break;
				case Expr::Logical:
					o = eval_logical(expr,hndl);
					break;
				case Expr::VarAcsr:
					o = eval_var_acsr(expr,hndl);
					break;
				case Expr::Assign:
					o = eval_asgn(expr,hndl);
					break;
				case Expr::Refer:
					o = eval_refer(expr,hndl);
					break;
				case Expr::FuncCall:
					o = eval_call(expr,hndl);
					break;
				case Expr::Conditional:
					o = eval_ternary(expr,hndl);
					break;
				default:
					LThw RuntimeError(Token(Tok::invalid,"",Object(),srcloc_t()),
									  "Failed to evaluate expr.");
			}
		} catch(RuntimeError& re){
			expr.handle();//not checking do_del 'cause when an error is thrown, it will be del'd by dtor
			LThw re;
		}
		clean_up{
			expr.handle();
		}

	}
	LRet o;
}

void Interpreter::exec_expr(C_STMT_REF stmt, bool hndl){
	LFn;
	evaluate(stmt.expr().expr(),hndl);
	LVoid;
}
void Interpreter::exec_print(C_STMT_REF stmt, bool hndl){
	LFn;
	Object obj = evaluate(stmt.print().expr(),hndl);
	QString out = obj.to_string()
			  #if T_DBG && T_GUI
				  + " : " + QString(obj.data().typeName())
			  #endif
				  ;
#if T_GUI
	QMessageBox::information(nullptr,"Info",out);
#else
	std::cout << out.toStdString();
#endif // T_GUI
	LVd;
}
void Interpreter::exec_println(C_STMT_REF stmt, bool hndl) {
	LFn;
	Object obj = evaluate(stmt.println().expr(), hndl);
	QString out = obj.to_string()
			  #if T_DBG
				  + " : " + QString(obj.data().typeName())
			  #endif
				  ;
#if T_GUI
	QMessageBox::information(nullptr, "Info", out);
#else
	std::cout << (out + "\n").toStdString();
#endif // T_GUI
	LVd;
}
void Interpreter::exec_if(C_STMT_REF stmt, bool hndl){
	LFn;
	if (evaluate(stmt.if_else().condition(),hndl).to_bool()){
		execute(stmt.if_else().if_block(),hndl);
	}
	else {
		execute(stmt.if_else().else_block(),hndl);
	}
	LVd;
}
void Interpreter::exec_block(C_STMT_REF stmt, bool hndl, EnvImpl* env){
	LFn;
	exec_block(stmt.block().block(),hndl,env);
	LVoid;
}
void Interpreter::exec_block(const std::vector<Ref<Stmt>>& stmts, bool, EnvImpl* env){
	LFn;
	int sz = stmts.size();
	Environment outer = environment;
	environment = env?env:new EnvImpl(outer);
	for (int i = 0; i != sz; i++)
		execute(stmts[i],false);
	delete environment;
	environment = outer;
	LVd;
}
void Interpreter::exec_while(C_STMT_REF stmt, bool){
	LFn;
	while (evaluate(stmt.while_loop().condition(),false).to_bool()){
		execute(stmt.while_loop().block(),false);
	}
	LVd;
}
void Interpreter::exec_var_decl(C_STMT_REF stmt, bool hndl){
	LFn;
	Expr expr = stmt.var().init();
	environment->define(stmt.var().name(),evaluate(expr,hndl));
	LVd;
}
void Interpreter::exec_fn_decl(C_STMT_REF stmt, bool){
	LFn;
	MereCallable mc(stmt);
	//		mc.set_onstack(false);
	environment->define(stmt.fn().name(),Object(Trait("function").make_function(),Var::fromValue(mc)));
	LVd;
}
void Interpreter::exec_ret(C_STMT_REF stmt, bool hndl){
	LFn;
	Object obj = evaluate(stmt.ret().value(),hndl);
	LThw Return(obj);
}
void Interpreter::exec_assert(C_STMT_REF stmt, bool hndl){
	LFn;
	if (!evaluate(stmt.assertion().value(),hndl).to_bool()){
		LThw Abort(stmt.assertion().code(), stmt.assertion().message());
	}
	LVd;
}

void Interpreter::exec_run(C_STMT_REF stmt, bool) {
	LFn;
	QFile file(stmt.run().filename());
	if (!file.open(QIODevice::ReadOnly)){
		LThw Abort(0x01," run stmt: failed to open file.");
	}
	Core::run(file);
	file.close();
#if T_GUI
#error Take care of the event-loop stuff
#endif
	LVd;
}

void Interpreter::exec_match(C_STMT_REF stmt, bool hndl){
	LFn;
	Object match = evaluate(stmt.match().match(),hndl);
	const std::vector<Branch>& br = stmt.match().branches();
	int size = br.size();
	int i = 0;
	for ( ; i != size; i++){
		const Branch& current = br[i];
		Object matcher = evaluate(current.expr(),hndl);
		if (match.match(matcher)){
			execute(current.stmt(),hndl);
			break;
		}
	}
	LVd;
}

void Interpreter::execute(C_STMT_REF stmt, bool hndl){
	LFn;
	Log ls("Interpreting stmt") ls(t_cast<int>(stmt.type()));
	if (!stmt.is(Stmt::Empty) && !stmt.is(Stmt::Invalid)){
		try{
			switch(stmt.type()){
				case Stmt::ExprStmt:
					exec_expr(stmt,hndl);
					break;
				case Stmt::Print:
					exec_print(stmt,hndl);
					break;
				case Stmt::Println:
					exec_println(stmt, hndl);
					break;
				case Stmt::If:
					exec_if(stmt,hndl);
					break;
				case Stmt::While:
					exec_while(stmt,hndl);
					break;
				case Stmt::Block:
					exec_block(stmt,hndl);
					break;
				case Stmt::VarDecl:
					exec_var_decl(stmt,hndl);
					break;
				case Stmt::Function:
					exec_fn_decl(stmt,hndl);
					hndl=false;
					break;
				case Stmt::Return:
					exec_ret(stmt,hndl);
					break;
				case Stmt::Assert:
					exec_assert(stmt,hndl);
					break;
				case Stmt::Match:
					exec_match(stmt,hndl);
					break;
				case Stmt::Run:
					exec_run(stmt,hndl);
					break;
				default:
					std::cout << "  > interpreter: Unknown statement.\n";
			}
		} catch(RuntimeError& re){
			stmt.handle();//Same thing applies here (check out interpret(Expr))
			LThw re;
		}
		clean_up{
			stmt.handle();
		}

	}
	LVd;
}

bool Interpreter::interpret(Stmts stmts){
	LFn;
	if (stmts.empty())
		LRet false;
	try{
		int sz = stmts.size();
		for (int i = 0; i != sz; i++){
			execute(stmts[i],true);
		}
		stmts.clear();
		LRet true;
	}

	catch(RuntimeError& re){
		Core::runtime_error(re);
	}
	catch(Return& ret){
		Q_UNUSED(ret);
		Core::error("Invalid Return. [inv_ret]");
	}
	catch(Abort& abt){
		QString msg = QString("[Code ") + QString::number(abt.code) + "]" + abt.message;
		Core::error(msg);
	}
	catch(ArgumentError& arg_err){
		QString errmsg = "[";
		errmsg.append(arg_err.callee).append("] ");
		errmsg.append("Expected a ").append(arg_err.expect).append(" instance but a ");
		errmsg.append(arg_err.received).append(" instance was provided. [arg_err]");
		Core::error(errmsg);
	}
	catch(ArityMismatchError& ame){
		QString errmsg = "[";
		errmsg.append(ame.callee).append("] ");
		errmsg.append("Expected ").append(QString::number(ame.expect)).append(" argument(s) ");
		errmsg.append("but ").append(QString::number(ame.received)).append(" was provided.");
		Core::error(errmsg);
	}

	catch(std::runtime_error& re){
		Core::error(QString(re.what()) + " [runtime_err]");
	}
	catch(std::bad_alloc& ba){
		Core::error(QString(ba.what()) + " [bad_alloc]");
	}
	catch(std::exception& ex){
		Core::error(QString(ex.what()) + " [exception]");
	}

	LRet false;
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

