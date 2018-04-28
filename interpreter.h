#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "stmt.h"
#include "runtimeerror.h"
#include "environment.h"
#define STR(ARG) TString(ARG)
#define ARE(TY) l.trait().is( TY ) && r.trait().is( TY )
#define IS(TY) r.trait().is( TY )
#define ARE_NUM l.trait().is_number() && r.trait().is_number()
#define IS_NUM r.trait().is_number()
#define OP(TY,OP) l.as<TY>() OP r.as<TY>()
#define PRE_OP(TY,OP) (OP r.as<TY>())

class Interpreter final{
	private:
		EnvImpl* globals = new EnvImpl();
		EnvImpl* environment = globals;
	public:
		Interpreter();
		~Interpreter();

		void reset(Environment=new EnvImpl());

		Environment global();

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
		Object evaluate		(Expr,bool);

		void exec_expr		(Stmt,bool);
		void exec_print		(Stmt, bool);
		void exec_if		(Stmt,bool);
		void exec_block		(Stmt,bool,Environment=nullptr);
		void exec_block		(QVector<Stmt>*,bool,Environment=nullptr);
		void exec_while		(Stmt,bool);
		void exec_var_decl	(Stmt,bool);
		void exec_fn_decl	(Stmt,bool);
		void exec_ret		(Stmt,bool);
		void execute		(Stmt,bool);
		void interpret		(Stmts);

};

#endif // INTERPRETER_H
