#ifndef STMT_H
#define STMT_H
#include "expr.h"
enum class StmtTy{
	Invalid,
	Empty,
	Expr,
	Decl,
	Block
//	IfStmt,
//	ValuedBlockStmt,
//	BlockStmt,
//	WhileStmt,
//	ForStmt
};

#define SIPtr StmtImpl*

struct StmtImpl final
{
	public:
		StmtTy ty;
		union{
			Expr expr;//for ExprStmt
			struct{
					Expr init;
					Token* var_name;
			};
			QVector<SIPtr>* block;
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

		static SIPtr decl_stmt(const Token& tok, Expr exp){
			EIPtr ptr = create();
			ptr->ty = StmtTy::Decl;
			ptr->var_name = new Token(tok);
			ptr->expr = exp;
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

		StmtTy type(){
			return ty;
		}
		bool is(StmtTy t){
			return ty==t;
		}

		StmtImpl();
		~StmtImpl(){
			ty = StmtTy::Invalid;
			_inv = true;
		}

};
typedef SIPtr Stmt;
typedef QVector<Stmt> Stmts;

#define ExprStmt	StmtImpl::expr_stmt
#define DeclStmt	StmtImpl::decl_stmt
#define NullStmt	StmtImpl::null_stmt
#define BlockStmt	StmtImpl::block_stmt

#endif // STMT_H
