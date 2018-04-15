#ifndef STMT_H
#define STMT_H
#include "expr.h"
enum class StmtTy{
	Invalid,
	Empty,
	Expr,
	VarDecl,
	Block,
	If,
	While,
	Print
};

#define SIPtr StmtImpl*

class StmtImpl final
{
	public:
		StmtTy ty;
		union{
			Expr expr;// ExprStmt, PrintStmt
			struct{// var_decl
					Expr init;
					Expr var_type; // nullptr or ScopeAcsr
					Token* var_name;
			};
			QVector<SIPtr>* block;
			struct{//if
				Expr condition;
				SIPtr if_block;
				SIPtr else_block;
			};
			struct {//while
				Expr cont_condit;
				SIPtr while_block;
			};
			bool _inv;
			bool empty;
		};
	private:
		static SIPtr create(){
			return new StmtImpl();
		}

	public:
		static SIPtr expr_stmt(Expr exp){
			SIPtr ptr = create();
			ptr->ty = StmtTy::Expr;
			ptr->expr = exp;
			return ptr;
		}

		static SIPtr var_decl_stmt(const Token& tok, Expr exp, Expr ty){
			SIPtr ptr = create();
			ptr->ty = StmtTy::VarDecl;
			ptr->var_name = new Token(tok);
			ptr->init = exp;
			ptr->var_type = ty;
			return ptr;
		}

		static SIPtr null_stmt(){
			SIPtr ptr = create();
			ptr->ty = StmtTy::Empty;
			ptr->empty = true;
			return ptr;
		}

		static SIPtr block_stmt(const QVector<SIPtr>& blk){
			SIPtr ptr = create();
			ptr->ty = StmtTy::Block;
			ptr->block = new QVector<SIPtr>(blk);
			return ptr;
		}

		static SIPtr if_stmt(Expr condit, SIPtr i_blk, SIPtr e_blk){
			SIPtr ptr = create();
			ptr->ty = StmtTy::If;
			ptr->condition = condit;
			ptr->if_block = i_blk;
			ptr->else_block = e_blk;
			return ptr;
		}

		static SIPtr while_stmt(Expr condit, SIPtr w_blk){
			SIPtr ptr = create();
			ptr->ty = StmtTy::While;
			ptr->cont_condit = condit;
			ptr->while_block = w_blk;
			return ptr;
		}

		static SIPtr print_stmt(Expr expr){
			SIPtr ptr = create();
			ptr->ty = StmtTy::Print;
			ptr->expr = expr;
			return ptr;
		}

		StmtTy type(){
			return ty;
		}
		bool is(StmtTy t){
			return ty==t;
		}

		StmtImpl();
		~StmtImpl();

};
typedef SIPtr Stmt;
typedef QVector<Stmt> Stmts;

#define ExprStmt	StmtImpl::expr_stmt
#define VarDeclStmt	StmtImpl::var_decl_stmt
#define NullStmt	StmtImpl::null_stmt
#define BlockStmt	StmtImpl::block_stmt
#define IfStmt		StmtImpl::if_stmt
#define WhileStmt	StmtImpl::while_stmt
#define PrintStmt	StmtImpl::print_stmt

#endif // STMT_H
