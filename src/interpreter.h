#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "stmt.h"
#include "runtimeerror.h"
#include "environment.h"

#define C_EXPR_REF const Expr&
#define C_STMT_REF const Stmt&
namespace mere {
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
			Object eval_group	(C_EXPR_REF,bool);
			Object eval_lit		(C_EXPR_REF,bool);
			Object eval_binary	(C_EXPR_REF,bool);
			Object eval_prefx	(C_EXPR_REF,bool);
			Object eval_pstfx	(C_EXPR_REF,bool);
			Object eval_logical	(C_EXPR_REF,bool);
			Object eval_var_acsr(C_EXPR_REF,bool);
			Object eval_asgn	(C_EXPR_REF,bool);
			Object eval_refer	(C_EXPR_REF,bool);
			Object eval_call	(C_EXPR_REF,bool);
			Object eval_ternary	(C_EXPR_REF,bool);

			void exec_expr		(C_STMT_REF,bool);
			void exec_print		(C_STMT_REF,bool);
			void exec_println	(C_STMT_REF,bool);
			void exec_if		(C_STMT_REF,bool);
			void exec_block		(C_STMT_REF,bool,Environment=nullptr);
			void exec_block		(const std::vector<Ref<Stmt>>&,bool,Environment=nullptr);
			void exec_while		(C_STMT_REF,bool);
			void exec_var_decl	(C_STMT_REF,bool);
			void exec_fn_decl	(C_STMT_REF,bool);
			void exec_ret		(C_STMT_REF,bool);
			void exec_assert	(C_STMT_REF,bool);
			void exec_run		(C_STMT_REF,bool);
			void exec_match		(C_STMT_REF,bool);
		public:
			Object evaluate		(C_EXPR_REF,bool);
			void execute		(C_STMT_REF,bool);
			bool interpret		(Stmts);

	};
}

#endif // INTERPRETER_H
