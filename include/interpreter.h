#pragma once

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "environment.h"
#include "interpretationunit.h"

#define C_EXPR_REF const Expr&
#define C_STMT_REF const Stmt&

#define STR(ARG) QString(ARG)
#define ARE(TY) l.trait().is( TY ) && r.trait().is( TY )
#define IS(TY) r.trait().is( TY )
#define ARE_NUM l.trait().is_number() && r.trait().is_number()
#define IS_NUM r.trait().is_number()
#define OP(TY,OP) l.as<TY>() OP r.as<TY>()
#define PRE_OP(TY,OP) (OP r.as<TY>())

namespace mere {
	class EnvImpl;
	/**
	 * \class Interpreter
	 * \brief It interprets statements given by the parser.
	 */
	class Interpreter final{
		private:
			EnvImpl* globals = new EnvImpl();
			EnvImpl* environment = globals;

		public:
			Interpreter	();
			~Interpreter();

			void reset(EnvImpl* = new EnvImpl());

			EnvImpl* global() const { return globals; }

			Object eval_group	(C_EXPR_REF);
			Object eval_lit		(C_EXPR_REF);
			Object eval_binary	(C_EXPR_REF);
			Object eval_prefx	(C_EXPR_REF);
			Object eval_pstfx	(C_EXPR_REF);
			Object eval_logical	(C_EXPR_REF);
			Object eval_var_acsr(C_EXPR_REF);
			Object eval_asgn	(C_EXPR_REF);
			Object eval_refer	(C_EXPR_REF);
			Object eval_call	(C_EXPR_REF);
			Object eval_ternary	(C_EXPR_REF);

			void exec_expr		(C_STMT_REF);
			void exec_print		(C_STMT_REF);
			void exec_println	(C_STMT_REF);
			void exec_if		(C_STMT_REF);
			void exec_block		(C_STMT_REF,EnvImpl* = nullptr);
			void exec_block		(const std::vector<Ref<Stmt>>&, EnvImpl* = nullptr);
			void exec_while		(C_STMT_REF);
			void exec_var_decl	(C_STMT_REF);
			void exec_fn_decl	(C_STMT_REF);
			void exec_ret		(C_STMT_REF);
			void exec_assert	(C_STMT_REF);
			void exec_run		(C_STMT_REF);
			void exec_match		(C_STMT_REF);

			Object evaluate		(C_EXPR_REF);
			void execute		(C_STMT_REF);
			short interpret(IntpUnit);

	};
}

#endif // INTERPRETER_H
