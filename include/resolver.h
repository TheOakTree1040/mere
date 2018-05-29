#pragma once

#ifndef RESOLVER_H
#define RESOLVER_H

#include "interpreter.h"
#include <string>
#include <QStack>
#include <QMap>

namespace mere {
	class Resolver {
			typedef QHash<QString,bool> map_t;
			typedef QStack<map_t> stack_t;

			Interpreter* intp;
			IntpUnit unit = nullptr;
			stack_t scopes;

		private:
			void beg_scope(){ scopes.push(map_t()); }
			void end_scope(){ scopes.pop(); }

			void error(const Token& tok, const QString& errmsg) const;

			void declare(const Token& name);
			void define(const Token& name);

			void resolve_block		(C_STMT_REF);
			void resolve_var_decl	(C_STMT_REF);
			void resolve_fn_decl	(C_STMT_REF);
			void resolve_expr_stmt	(C_STMT_REF);
			void resolve_if			(C_STMT_REF);
			void resolve_print		(C_STMT_REF);
			void resolve_println	(C_STMT_REF);
			void resolve_while		(C_STMT_REF);
			void resolve_ret		(C_STMT_REF);
			void resolve_assert		(C_STMT_REF);
			void resolve_match		(C_STMT_REF);
			void resolve_run		(C_STMT_REF) const { }

			void resolve_function(C_STMT_REF stmt);

			void resolve(C_STMT_REF stmt);

			void resolve(const std::vector<Stmt>& stmts);

			void resolve_var(C_EXPR_REF expr);
			void resolve_assign(C_EXPR_REF expr);
			void resolve_bin(C_EXPR_REF expr);
			void resolve_unary(C_EXPR_REF expr);
			void resolve_lit(C_EXPR_REF) const {}
			void resolve_group(C_EXPR_REF expr);
			void resolve_logical(C_EXPR_REF expr);
			void resolve_refer(C_EXPR_REF expr);
			void resolve_call(C_EXPR_REF expr);
			void resolve_ternary(C_EXPR_REF expr);

			void resolve_local(C_EXPR_REF expr, const Token& name);

			void resolve(C_EXPR_REF expr);

		public:
			Resolver(Interpreter* i):intp(i){}

			void resolve(IntpUnit u);
	};
}

#endif // RESOLVER_H
