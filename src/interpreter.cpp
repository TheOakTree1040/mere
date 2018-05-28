
#include "interpreter.h"
#include "natives.h"
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

Object Interpreter::eval_pstfx(C_EXPR_REF expr){
	LFn;
	Object r = evaluate(expr.unary().expr());
	if (!r.trait().is_lvalue()){
		LThw RuntimeError(expr.unary().op(),"expected a lvalue");
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
	QString errmsg = "operand type mismatch: '";
	errmsg.append(r.trait().id()).append("' [");
	errmsg += expr.unary().op().lexeme() + "]";
	LThw RuntimeError(expr.unary().op(),errmsg);
}
Object Interpreter::eval_prefx(C_EXPR_REF expr){
	LFn;
	Object r = evaluate(expr.unary().expr());
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
				LThw RuntimeError(expr.unary().op(), "expected an lvalue");
			}
			if(IS_NUM && r.trait().is_lvalue()){
				LRet r.prefix(1);
			}
			break;
		case Tok::decr:
			if (!r.trait().is_lvalue()){
				LThw RuntimeError(expr.unary().op(), "expected an lvalue");
			}
			if(IS_NUM && r.trait().is_lvalue()){
				LRet r.prefix(-1);
			}
			break;
		default:;
	}
	QString errmsg = "operand type mismatch: [";
	errmsg.append(expr.unary().op().lexeme()).append("] '").append(r.trait().id()).append("'");
	LThw RuntimeError(expr.unary().op(),errmsg);
}
Object Interpreter::eval_lit(C_EXPR_REF expr){
	LFn;
	LRet expr.lit().lit();
}
Object Interpreter::eval_group(C_EXPR_REF expr){
	LFn;
	LRet evaluate(expr.group().expr());
}
Object Interpreter::eval_binary(C_EXPR_REF expr){
	LFn;
	Object l = evaluate(expr.bin().left());
	Object r = evaluate(expr.bin().right());
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
					LThw RuntimeError(expr.bin().op(),"division by 0");
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
			LThw RuntimeError(expr.bin().op(),"undefined binary operation");

	}
	QString errmsg = "operand type mismatch: '";
	errmsg.append(l.trait().id()).append("' ").append(expr.bin().op().lexeme())
			.append(" '").append(r.trait().id()).append("'");
	LThw RuntimeError(expr.bin().op(),errmsg);
}
Object Interpreter::eval_logical(C_EXPR_REF expr){
	LFn;
	Tokty ty = expr.logical().op().type();
	Object l = evaluate(expr.logical().left());
	if (ty == Tok::vert_vert){
		if (l.to_bool()){
			LRet l;
		}
	}
	else if (!l.to_bool()){
		LRet l;
	}
	LRet evaluate(expr.logical().right());
}
Object Interpreter::eval_var_acsr(C_EXPR_REF expr){
	LFn;
	Object o;
	Object& ref = environment->access(expr.var().accessor());//DO NOT TOUCH! Keep this line for catching exceptions
	o.as_acsr_of(ref);
	LRet o;
}
Object Interpreter::eval_asgn(C_EXPR_REF expr){
	LFn;
	Token& op = expr.assign().op();
	Expr& l = expr.assign().left();
	Expr& r = expr.assign().right();
	Tokty ty = op.type();
	Object ref(evaluate(l));
	Object right(evaluate(r));

	if (!ref.trait().is_lvalue()){
		LThw RuntimeError(op,"expected an lvalue");
	}
	if (!ref.trait().is_dynamic() && !ref.trait().has_type_of(right.trait())){
		LThw RuntimeError(op, "attempting to re-type a fixed-type variable");
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
			LThw RuntimeError(op,"no such assignment operator");
	}
RET_OP_ASGN:;
	Expr lex = LitExpr(ref);
	Expr rex = LitExpr(right);
	LRet ref.recv(evaluate(BinExpr(lex,Token(right_val_op,op.lexeme(),Object(),op.loc()),rex)));
}
Object Interpreter::eval_refer(C_EXPR_REF expr){
	LFn;
	Object rl = evaluate(expr.ref().left());
	Object rr = evaluate(expr.ref().right());
	if (!rl.trait().is_lvalue()){
		LThw RuntimeError(expr.ref().op(), "expected an lvalue (left)");
	}
	if (!rr.trait().is_lvalue()){
		LThw RuntimeError(expr.ref().op(), "expected an lvalue (right)");
	}
	LRet rl.as_ref_of(rr);
}
Object Interpreter::eval_call(C_EXPR_REF expr){
	LFn;
	std::vector<Ref<Expr>> args_exprs = expr.call().arguments();
	int sz = args_exprs.size();
	std::vector<Object> args(sz);
	Object o;
	for (int i = 0; i != sz; i++){
		o = evaluate(args_exprs[i].get());
		args[i] = o;
	}
	Object callee = evaluate(expr.call().callee());
	if (!callee.trait().is_function()){
		LThw RuntimeError(expr.call().op(),"invalid callee");
	}
	MereCallable* mc = t_cast<MereCallable*>(callee.data().data());
	if (sz != mc->arity()){
		LThw RuntimeError(expr.call().op(),QString("expected ") +
						  QString::number(mc->arity()) +
						  " argument(s), but " + QString::number(sz) + " was found instead");
	}

	LRet mc->call(*this,args);
}
//Object Interpreter::eval_cast(Expr){
//#error Implement CAST
//#error Put cast into the switch in Intp::evaluate
//}
Object Interpreter::eval_ternary(C_EXPR_REF expr){
	bool b = evaluate(expr.ternary().condition()).to_bool();
	return b?evaluate(expr.ternary().left()):
			 evaluate(expr.ternary().right());
}

Object Interpreter::evaluate(C_EXPR_REF expr){
	LFn;
	Log ls("Evaluating expr-") ls(QString::number((short)expr.type()));
	Object o;
	if (!expr.is(Expr::Empty)){
		switch(expr.type()){
			case Expr::Group:
				o = eval_group(expr);
				break;
			case Expr::Binary:
				o = eval_binary(expr);
				break;
			case Expr::Postfix:
				o = eval_pstfx(expr);
				break;
			case Expr::Prefix:
				o = eval_prefx(expr);
				break;
			case Expr::Literal:
				o = eval_lit(expr);
				break;
			case Expr::Logical:
				o = eval_logical(expr);
				break;
			case Expr::VarAcsr:
				o = eval_var_acsr(expr);
				break;
			case Expr::Assign:
				o = eval_asgn(expr);
				break;
			case Expr::Refer:
				o = eval_refer(expr);
				break;
			case Expr::FuncCall:
				o = eval_call(expr);
				break;
			case Expr::Conditional:
				o = eval_ternary(expr);
				break;
			default:
				LThw RuntimeError(Token(Tok::invalid,"",Object(),srcloc_t()),
								  "implementation needed");
		}
	}
	LRet o;
}

void Interpreter::exec_expr(C_STMT_REF stmt){
	LFn;
	evaluate(stmt.expr().expr());
	LVoid;
}
void Interpreter::exec_print(C_STMT_REF stmt){
	LFn;
	Object obj = evaluate(stmt.print().expr());
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
void Interpreter::exec_println(C_STMT_REF stmt) {
	LFn;
	Object obj = evaluate(stmt.println().expr());
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
void Interpreter::exec_if(C_STMT_REF stmt){
	LFn;
	if (evaluate(stmt.if_else().condition()).to_bool()){
		execute(stmt.if_else().if_block());
	}
	else {
		execute(stmt.if_else().else_block());
	}
	LVd;
}
void Interpreter::exec_block(C_STMT_REF stmt, EnvImpl* env){
	LFn;
	exec_block(stmt.block().block(),env);
	LVoid;
}
void Interpreter::exec_block(const std::vector<Ref<Stmt>>& stmts, EnvImpl* env){
	LFn;
	int sz = stmts.size();
	Environment outer = environment;
	environment = env?env:new EnvImpl(outer);
	for (int i = 0; i != sz; i++)
		execute(stmts[i]);
	delete environment;
	environment = outer;
	LVd;
}
void Interpreter::exec_while(C_STMT_REF stmt){
	LFn;
	while (evaluate(stmt.while_loop().condition()).to_bool()){
		execute(stmt.while_loop().block());
	}
	LVd;
}
void Interpreter::exec_var_decl(C_STMT_REF stmt){
	LFn;
	Expr& expr = stmt.var().init();
	environment->define(stmt.var().name(),evaluate(expr));
	LVd;
}
void Interpreter::exec_fn_decl(C_STMT_REF stmt){
	LFn;
	MereCallable mc(stmt);
	//		mc.set_onstack(false);
	environment->define(stmt.fn().name(),Object(Trait("function").make_function(),Var::fromValue(mc)));
	LVd;
}
void Interpreter::exec_ret(C_STMT_REF stmt){
	LFn;
	Object obj = evaluate(stmt.ret().value());
	LThw Return(obj);
}
void Interpreter::exec_assert(C_STMT_REF stmt){
	LFn;
	if (!evaluate(stmt.assertion().value()).to_bool()){
		LThw Abort(stmt.assertion().code(), stmt.assertion().message());
	}
	LVd;
}

void Interpreter::exec_run(C_STMT_REF stmt) {
	LFn;
	QFile file(stmt.run().filename());
	if (!file.open(QIODevice::ReadOnly)){
		LThw Abort(0x01," run stmt: couldn't open file");
	}
	IntpUnit unit = new InterpretationUnit(file);
	if (!unit->success()){
		unit->print_issues();
		file.close();
		delete unit;
		LThw Abort(0x02, " run stmt: pre-intp error");
	}
	if (!this->interpret(unit)) {
		unit->print_issues();
		file.close();
		delete unit;
		LThw Abort(0x03, " run_stmt: rt-error");
	}
	delete unit;
	file.close();
	LVd;
}

void Interpreter::exec_match(C_STMT_REF stmt){
	LFn;
	Object match = evaluate(stmt.match().match());
	const std::vector<Branch>& br = stmt.match().branches();
	int size = br.size();
	int i = 0;
	for ( ; i != size; i++){
		const Branch& current = br[i];
		Object matcher = evaluate(current.expr());
		if (match.match(matcher)){
			execute(current.stmt());
			break;
		}
	}
	LVd;
}

void Interpreter::execute(C_STMT_REF stmt){
	LFn;
	Log ls("Interpreting stmt") ls(t_cast<int>(stmt.type()));
	if (!stmt.is(Stmt::Empty) && !stmt.is(Stmt::Invalid)){
		switch(stmt.type()){
			case Stmt::ExprStmt:
				exec_expr(stmt);
				break;
			case Stmt::Print:
				exec_print(stmt);
				break;
			case Stmt::Println:
				exec_println(stmt);
				break;
			case Stmt::If:
				exec_if(stmt);
				break;
			case Stmt::While:
				exec_while(stmt);
				break;
			case Stmt::Block:
				exec_block(stmt);
				break;
			case Stmt::VarDecl:
				exec_var_decl(stmt);
				break;
			case Stmt::Function:
				exec_fn_decl(stmt);
				break;
			case Stmt::Return:
				exec_ret(stmt);
				break;
			case Stmt::Assert:
				exec_assert(stmt);
				break;
			case Stmt::Match:
				exec_match(stmt);
				break;
			case Stmt::Run:
				exec_run(stmt);
				break;
			default:
				std::cout << "  > interpreter: unknown statement found\n";
		}
	}
	LVd;
}

short Interpreter::interpret(IntpUnit unit){
	LFn;
	if (!unit->success())
		return 1;
	AST& ast = unit->ast();
	if (ast.empty())
		LRet false;
	try{
		int size = ast.size();
		for (int i = 0; i != size; i++){
			execute(ast[i]);
		}
		LRet 0;
	}

	catch(RuntimeError& re){
		unit->report(re.tok.loc(),"rt-error",re.msg);
	}
	catch(Return&){
		unit->report("inv-ret","invalid return");
	}
	catch(Abort& abt){
		QString msg = QString("code %1 | %2").arg(abt.code).arg(abt.message);
		unit->report("abort",msg);
	}
	catch(ArgumentError& ae){
		QString msg = QString("[callee: %1] expected a '%2' "
							  "but a '%3' was found instead")
					  .arg(ae.callee).arg(ae.expect).arg(ae.received);
		unit->report("arg-error",msg);
	}
	catch(ArityMismatchError& ame){
		QString msg = QString("[callee: %1] expected %2 arguments "
							  "but %3 was found instead")
					  .arg(ame.callee).arg(ame.expect).arg(ame.received);
		unit->report("for-art-mism-err",msg);
	}

	catch(std::runtime_error& re){
		unit->report("intl-rt-error",re.what());
	}
	catch(std::bad_alloc& ba){
		unit->report("bad-alloc",ba.what());
	}
	catch(std::exception& ex){
		unit->report("intn-except",ex.what());
	}

	LRet 1;
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

