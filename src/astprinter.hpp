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
		TString text;
	public:
		ASTPrinter(Stmts stmts):
			text(""){
			LFn;
			for (int i = 0; i != stmts.size(); i++)
				print(0,stmts[i]);
#if _DEBUG
			Log << "end ASTPrinter()";
#endif
		}

		TString AST(){
#if _DEBUG
			Log << "Getting AST";
#endif
			return text;
		}
	private:
		void write_ln(int lvls, const TString& ln){
			for (int i = lvls; i != 0; i--)
				text.append("\t");
			text.append(ln).append("\n");
		}
		void print_conditional(int lvls, Expr expr){
			CHKTY(ExprTy::Conditional);
			write_ln(lvls,"Conditional:");
			print(lvls+1,expr->condition);
			write_ln(lvls,"If True  :");
			print(lvls+1,expr->l_branch);
			write_ln(lvls,"If False :");
			print(lvls+1,expr->r_branch);
		}
		void print_assign(int lvls, Expr expr){
			CHKTY(ExprTy::Assign);
			write_ln(lvls,"Assign value:");
			print(++lvls,expr->asgn_right);
			write_ln(lvls,"To:");
			print(lvls,expr->asgn_left);
		}

		void print_binary(int lvls, Expr expr){
			CHKTY(ExprTy::Binary);
			TString head = "BinExpr ";
			head.append(expr->op->lexeme)./*append(" [Ln ").append(expr->op.ln).*/append(":");
			write_ln(lvls,head);
			write_ln(lvls+1,"Left:");
			print(lvls+2,expr->expr);
			write_ln(lvls+1,"Right:");
			print(lvls+2,expr->right);
		}
		void print_literal(int lvls, Expr expr){
			CHKTY(ExprTy::Literal);
			TString head = "[";
			head.append(expr->lit->trait().id()).append("] Literal:");
			write_ln(lvls,head);
			write_ln(lvls+1,expr->lit->to_string());
		}
		void print_arr(int lvls, Expr expr){
			CHKTY(ExprTy::Array);
			TString head = "[Array]";
			write_ln(lvls,head);
		}
//		void print_lval(int lvls, Expr expr){
//			CHKTY(ExprTy::LValue);
//			TString head = "Asserted_LValue:";
//			write_ln(lvls,head);
//			print(lvls+1,expr->lval_expr);
//		}
		void print_group(int lvls, Expr expr){
			CHKTY(ExprTy::Group);
			TString head = "GroupExpr:";
			write_ln(lvls,head);
			print(lvls+1,expr->expr);
		}
		void print_prefx(int lvls, Expr expr){
			CHKTY(ExprTy::Prefix);
			TString head = "UnaryExpr [";
			head.append(expr->op->lexeme).append("var]:");
			write_ln(lvls,head);
			print(lvls+1,expr->expr);
		}
		void print_pstfx(int lvls, Expr expr){
			CHKTY(ExprTy::Postfix);
			TString head = "UnaryExpr [var";
			head.append(expr->op->lexeme).append("]:");
			write_ln(lvls,head);
			print(lvls+1,expr->expr);
		}
		void print_var(int lvls, Expr expr){
			CHKTY(ExprTy::VarAcsr);
			TString head = "[Variable ";
			head.append(expr->var_acsr->lexeme).append("]");
			write_ln(lvls,head);
		}

		void print_expr_stmt(int lvls, Stmt stmt){
			S_CHKTY(StmtTy::Expr);
			TString head = "ExprStmt";
			write_ln(lvls,head);
			print(lvls+1,stmt->expr);
		}
		void print_var_decl_stmt(int lvls, Stmt stmt){
			S_CHKTY(StmtTy::VarDecl);
			TString head = "var " + stmt->var_name->lexeme + " equals:";
			write_ln(lvls,head);
			print(lvls+1,stmt->init);
		}
		void print_block_stmt(int lvls, Stmt stmt){
			S_CHKTY(StmtTy::Block);
			TString head = "{";
			write_ln(lvls++,head);
			int sz = stmt->block->size();
			for (int i = 0; i != sz; i++){
				print(lvls,stmt->block->at(i));
			}
			write_ln(--lvls,"}");
		}
		void print_while_stmt(int lvls, Stmt stmt){
			S_CHKTY(StmtTy::While);
			TString head = "{\n   While:";
			write_ln(lvls,head);
			print(lvls+2,stmt->cont_condit);
			write_ln(lvls+1,"Do:");
			print(lvls+2,stmt->while_block);
			write_ln(lvls, "}");
		}
		void print_if_stmt(int lvls, Stmt stmt){
			S_CHKTY(StmtTy::If);
			write_ln(lvls,"if:");
			print(lvls+1,stmt->condition);
			write_ln(lvls,"[then]");
			print(lvls+1,stmt->if_block);
			if (stmt->else_block){
				write_ln(lvls,"[else]");
				print(lvls+1,stmt->else_block);
			}
		}

		void print(int lvls, Expr expr){
			if (!expr)
				return;
			switch (expr->type()) {
				case ExprTy::Logical:
					expr->ty = ExprTy::Binary;
					print_binary(lvls,expr);
					expr->ty = ExprTy::Logical;
					break;
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
//				case ExprTy::LValue:
//					print_lval(lvls,expr);
//					break;
				case ExprTy::Assign:
					print_assign(lvls,expr);
					break;
				case ExprTy::Conditional:
					print_conditional(lvls,expr);
					break;
				default:
					write_ln(lvls,TString("[EXPR-").append(TString::number((int)expr->type())).append("]"));
					break;
			}
		}

		void print(int lvls, Stmt stmt){
			if (!stmt)
				return;
			switch(stmt->type()){
				case StmtTy::Expr:
					print_expr_stmt(lvls,stmt);
					break;
				case StmtTy::VarDecl:
					print_var_decl_stmt(lvls,stmt);
					break;
				case StmtTy::Block:
					print_block_stmt(lvls,stmt);
					break;
				case StmtTy::While:
					print_while_stmt(lvls,stmt);
					break;
				case StmtTy::Empty:
					write_ln(lvls,"[No-op]");
					break;
				case StmtTy::Invalid:
					write_ln(lvls,"[!! Invalid !!]");
					break;
				case StmtTy::If:
					print_if_stmt(lvls,stmt);
					break;
				default:
					write_ln(lvls,TString("[STMT-").append(TString::number((int)stmt->type())).append("]"));
					break;
			}
		}
};

#endif // ASTPRINTER_H
