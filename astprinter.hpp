#ifndef ASTPRINTER_H
#define ASTPRINTER_H
#include "stmt.h"

#define CHKTY(TY)\
	if (!(expr->type() == TY)){\
		print(lvls,expr);\
		return;\
	}
#define S_CHKTY(TY)\
	if (!(stmt->type() == TY)){\
		print(lvls,stmt);\
		return;\
	}
class ASTPrinter final{
	private:
		QString text;
	public:
		ASTPrinter(Stmts stmts):
		text(""){
			LFn;
			for (int i = 0; i != stmts.size(); i++)
				print(0,stmts[i]);
			Log << "end ASTPrinter()";
		}

		QString AST(){
			Log << "Getting AST";
			return text;
		}
	private:
		void write_ln(int lvls, const QString& ln){
			for (int i = lvls; i != 0; i--)
				text.append("  ");
			text.append(ln).append("\n");
		}

		void print_binary(int lvls, Expr expr){
			CHKTY(ExprTy::Binary)
					QString head = "BinExpr ";
			head.append(expr->op->lexeme)./*append(" [Ln ").append(expr->op.ln).*/append(":");
			write_ln(lvls,head);
			write_ln(lvls+1,"Left:");
			print(lvls+2,expr->expr);
			write_ln(lvls+1,"Right:");
			print(lvls+2,expr->right);
		}
		void print_literal(int lvls, Expr expr){
			CHKTY(ExprTy::Literal);
			QString head = "[";
			head.append(expr->lit->trait.id()).append("] Literal:");
			write_ln(lvls,head);
			write_ln(lvls+1,expr->lit->to_string());
		}
		void print_arr(int lvls, Expr expr){
			CHKTY(ExprTy::Array);
			QString head = "[Array]";
			write_ln(lvls,head);
		}
		void print_lval(int lvls, Expr expr){
			CHKTY(ExprTy::LValue);
			QString head = "Asserted_LValue:";
			write_ln(lvls,head);
			print(lvls+1,expr->lval_expr);
		}
		void print_group(int lvls, Expr expr){
			CHKTY(ExprTy::Group);
			QString head = "GroupExpr:";
			write_ln(lvls,head);
			print(lvls+1,expr->expr);
		}
		void print_prefx(int lvls, Expr expr){
			CHKTY(ExprTy::Prefix);
			QString head = "UnaryExpr [";
			head.append(expr->op->lexeme).append("var]:");
			write_ln(lvls,head);
			print(lvls+1,expr->expr);
		}
		void print_pstfx(int lvls, Expr expr){
			CHKTY(ExprTy::Postfix);
			QString head = "UnaryExpr [var";
			head.append(expr->op->lexeme).append("]:");
			write_ln(lvls,head);
			print(lvls+1,expr->expr);
		}
		void print_var(int lvls, Expr expr){
			CHKTY(ExprTy::VarAcsr);
			QString head = "[Variable ";
			head.append(expr->var_acsr->lexeme).append("]");
			write_ln(lvls,head);
		}

		void print_expr_stmt(int lvls, Stmt stmt){
			S_CHKTY(StmtTy::Expr);
			QString head = "ExprStmt";
			write_ln(lvls,head);
			print(lvls+1,stmt->expr);
		}
		void print_var_decl_stmt(int lvls, Stmt stmt){
			S_CHKTY(StmtTy::VarDecl);
			QString head = "var " + stmt->var_name->lexeme + " equals:";
			write_ln(lvls,head);
			print(lvls+1,stmt->init);
		}

		void print(int lvls, Expr expr){
			switch (expr->type()) {
				case ExprTy::Binary:
					print_binary(lvls,expr);
					break;
				case ExprTy::Literal:
					print_literal(lvls,expr);
					break;
				case ExprTy::VarAcsr:
					print_var(lvls,expr);
					break;
				case ExprTy::Array:
					print_arr(lvls,expr);
					break;
				case ExprTy::Prefix:
					print_prefx(lvls,expr);
					break;
				case ExprTy::Postfix:
					print_pstfx(lvls,expr);
					break;
				case ExprTy::Group:
					print_group(lvls,expr);
					break;
				case ExprTy::LValue:
					print_lval(lvls,expr);
					break;
				default:
					write_ln(lvls,QString("[UNPRINTABLE_EXPR_").append(QString::number((int)expr->type())).append("]"));
					break;
			}
		}

		void print(int lvls, Stmt stmt){
			switch(stmt->type()){
				case StmtTy::Expr:
					print_expr_stmt(lvls,stmt);
					break;
				case StmtTy::VarDecl:
					print_var_decl_stmt(lvls,stmt);
					break;
				default:
					write_ln(lvls,QString("[UNPRINTABLE_STMT_").append(QString::number((int)stmt->type())).append("]"));
					break;
			}
		}
};

#endif // ASTPRINTER_H
