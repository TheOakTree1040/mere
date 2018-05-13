#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "stmt.h"
#include "runtimeerror.h"
#include "environment.h"

//string ctor
#define STR(ARG) TString(ARG)
//checks l & r trait
#define ARE(TY) l.trait().is( TY ) && r.trait().is( TY )
//checks r trait
#define IS(TY) r.trait().is( TY )
//checks whether l & r are numbers
#define ARE_NUM l.trait().is_number() && r.trait().is_number()
//checks whether r is a number
#define IS_NUM r.trait().is_number()
//apply the operator OP to l & r after converting to TY
#define OP(TY,OP) l.as<TY>() OP r.as<TY>()
//apply the operator before r after converting to TY
#define PRE_OP(TY,OP) (OP r.as<TY>())

/// @class Interpreter
/// @brief It interprets statements given by the parser.
class Interpreter final{
	private:
		Environment globals = new EnvImpl();
		Environment environment = globals;

	public:
		Interpreter	();
		~Interpreter();

		void reset(Environment=new EnvImpl());

		Environment global();
	public:
		Object eval_group	(Expr,bool);
		Object eval_lit		(Expr,bool);
		Object eval_binary	(Expr,bool);
		Object eval_prefx	(Expr,bool);
		Object eval_pstfx	(Expr,bool);
		Object eval_logical	(Expr,bool);
		Object eval_var_acsr(Expr,bool);
		Object eval_asgn	(Expr,bool);
		Object eval_refer	(Expr,bool);
		Object eval_call	(Expr,bool);
		Object eval_ternary	(Expr,bool);
//		Object eval_cast	(Expr,bool);

		void exec_expr		(Stmt,bool);
		void exec_print		(Stmt,bool);
		void exec_println	(Stmt,bool);
		void exec_if		(Stmt,bool);
		void exec_block		(Stmt,bool,Environment=nullptr);
		void exec_block		(QVector<Stmt>*,bool,Environment=nullptr);
		void exec_while		(Stmt,bool);
		void exec_var_decl	(Stmt,bool);
		void exec_fn_decl	(Stmt,bool);
		void exec_ret		(Stmt,bool);
		void exec_assert	(Stmt,bool);
		void exec_match		(Stmt,bool);
		void execute		(Stmt,bool);
	public:
		Object evaluate		(Expr,bool);
		bool interpret		(Stmts);

};

#endif // INTERPRETER_H
