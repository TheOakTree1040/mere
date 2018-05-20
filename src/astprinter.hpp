#if !defined(ASTPRINTER_H) && AST_PRINTER_FIXED
#define ASTPRINTER_H

#include "stmt.h"
namespace mere{
	class ASTPrinter final{
		private:
			TString text;
		public:
			ASTPrinter(Stmts stmts):
				text(""){
				LFn;
				for (int i = 0; i != stmts.size(); i++)
					print(0,stmts[i]);
				Log ls("end ASTPrinter()");
			}

			TString AST(){
				Log ls("Getting AST");
				return text;
			}
		private:
			void write_ln(int lvls, const TString& ln){
				for (int i = lvls; i != 0; i--)
					text.append("\t");
				text.append(ln).append("\n");
			}
			void print_conditional(int lvls, Expr expr){
				write_ln(lvls,"Conditional:");
				print(lvls+1,expr.ternary().condition());
				write_ln(lvls,"If True  :");
				print(lvls+1,expr.ternary().left());
				write_ln(lvls,"If False :");
				print(lvls+1,expr.ternary().right());
			}
			void print_assign(int lvls, Expr expr){
				write_ln(lvls,"Assign:");
				print(++lvls,expr.assign().right());
				write_ln(lvls,"To:");
				print(lvls,expr.assign().left());
			}
			void print_binary(int lvls, Expr expr){
				TString s = "BinExpr ";
				s.append(expr.bin().op().lexeme).append(":");
				write_ln(lvls,s);
				write_ln(lvls+1,"Left:");
				print(lvls+2,expr.bin().left());
				write_ln(lvls+1,"Right:");
				print(lvls+2,expr.bin().right());
			}
			void print_literal(int lvls, Expr expr){
				TString s = "[";
				s.append(expr.lit().lit().trait().id()).append("] Literal:");
				write_ln(lvls,s);
				write_ln(lvls+1,expr.lit().lit().to_string());
			}
			void print_arr(int lvls, Expr expr){
				write_ln(lvls,"array {\n");
				auto arr = expr.array().array();
				for (int i = 0; i != arr.size(); i++){
					print(lvls+1,array[i]);
				}
				write_ln(lvls,"}\n");
			}
			void print_group(int lvls, Expr expr){
				write_ln(lvls,"GroupExpr:");
				print(lvls+1,expr->expr);
			}
			void print_prefx(int lvls, Expr expr){
				TString s = "UnaryExpr [";
				s.append(expr->op->lexeme).append("var]:");
				write_ln(lvls,s);
				print(lvls+1,expr->expr);
			}
			void print_pstfx(int lvls, Expr expr){
				TString s = "UnaryExpr [var";
				s.append(expr->op->lexeme).append("]:");
				write_ln(lvls,s);
				print(lvls+1,expr->expr);
			}
			void print_var(int lvls, Expr expr){
				TString s = "[Variable ";
				s.append(expr->var_acsr->lexeme).append("]");
				write_ln(lvls,s);
			}

			void print_expr_stmt(int lvls, Stmt stmt){
				write_ln(lvls,"ExprStmt");
				print(lvls+1,stmt->expr);
			}
			void print_var_decl_stmt(int lvls, Stmt stmt){
				write_ln(lvls,"create var " + stmt->var_name->lexeme + " with value:");
				print(lvls+1,stmt->init);
			}
			void print_block_stmt(int lvls, Stmt stmt){
				write_ln(lvls++,"{");
				int sz = stmt->block->size();
				for (int i = 0; i != sz; i++){
					print(lvls,stmt->block->at(i));
				}
				write_ln(--lvls,"}");
			}
			void print_while_stmt(int lvls, Stmt stmt){
				write_ln(lvls,"{\n   While:");
				print(lvls+2,stmt->cont_condit);
				write_ln(lvls+1,"Do:");
				print(lvls+2,stmt->while_block);
				write_ln(lvls, "}");
			}
			void print_if_stmt(int lvls, Stmt stmt){
				write_ln(lvls,"if:");
				print(lvls+1,stmt->condition);
				write_ln(lvls,"[then]");
				print(lvls+1,stmt->if_block);
				if (stmt->else_block){
					write_ln(lvls,"[else]");
					print(lvls+1,stmt->else_block);
				}
			}
			void print_print_stmt(int lvls, Stmt stmt){
				write_ln(lvls, "print:");
				print(lvls+1,stmt->expr);
			}
			void print_println_stmt(int lvls, Stmt stmt){
				write_ln(lvls, "println:");
				print(lvls+1,stmt->expr);
			}
			void print_ret_stmt(int lvls, Stmt stmt){
				write_ln(lvls, "return:");
				print(lvls+1,stmt->retval);
			}
			void print_assert_stmt(int lvls, Stmt stmt){
				write_ln(lvls,"Assert:");
				print(lvls+1,stmt->assertion);
			}

			void print(int lvls, Expr expr){
				if (!expr)
					return;
				switch (expr->type()) {
					case Expr::Logical:
						expr->ty = Expr::Binary;
						print_binary(lvls,expr);
						expr->ty = Expr::Logical;
						break;
					case Expr::Binary:
						print_binary(lvls,expr);
						break;
					case Expr::Literal:
						print_literal(lvls,expr);
						break;
					case Expr::VarAcsr:
						print_var(lvls,expr);
						break;
					case Expr::Array:
						print_arr(lvls,expr);
						break;
					case Expr::Prefix:
						print_prefx(lvls,expr);
						break;
					case Expr::Postfix:
						print_pstfx(lvls,expr);
						break;
					case Expr::Group:
						print_group(lvls,expr);
						break;
					case Expr::Assign:
						print_assign(lvls,expr);
						break;
					case Expr::Conditional:
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
					case Stmt::ExprStmt:
						print_expr_stmt(lvls,stmt);
						break;
					case Stmt::VarDecl:
						print_var_decl_stmt(lvls,stmt);
						break;
					case Stmt::Block:
						print_block_stmt(lvls,stmt);
						break;
					case Stmt::While:
						print_while_stmt(lvls,stmt);
						break;
					case Stmt::Empty:
						write_ln(lvls,"[No-op]");
						break;
					case Stmt::Invalid:
						write_ln(lvls,"[!! Invalid !!]");
						break;
					case Stmt::If:
						print_if_stmt(lvls,stmt);
						break;
					case Stmt::Print:
						print_print_stmt(lvls,stmt);
						break;
					case Stmt::Println:
						print_println_stmt(lvls,stmt);
						break;
					case Stmt::Return:
						print_ret_stmt(lvls,stmt);
						break;
					default:
						write_ln(lvls,TString("[STMT-").append(TString::number((int)stmt->type())).append("]"));
						break;
				}
			}
	};
}

#endif // ASTPRINTER_H
