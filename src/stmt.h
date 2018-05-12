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
	Print,
	Println,
	Function,
	Return,
	Assert,
	Match
};

//For MatchStmt
struct Branch;

#define SIPtr StmtImpl*

class StmtImpl final
{
	public:
		StmtTy ty;
		union{
			Expr expr;// ExprStmt, PrintStmt, PrintlnStmt
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
			struct{//func
				Token* fn_name;
				QVector<Token*>* fn_params;
				QVector<SIPtr>* fn_body;
			};
			struct{//return
					Expr retval;
					Token* retop;
			};
			struct{//assert
					Expr assertion;
					int code;
					TString* msg;
			};
			struct{//match
					Expr match;
					QVector<Branch*>* branches;
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

		static SIPtr fn_decl_stmt(const Token& name, const QVector<Token>& params, const QVector<SIPtr>& body){
			SIPtr ptr = create();
			ptr->ty = StmtTy::Function;
			ptr->fn_name = new Token(name);
			int sz = params.size();
			ptr->fn_params = new QVector<Token*>(sz);
			for (int i = 0; i != sz; i++){
				const_cast<Token*&>(ptr->fn_params->at(i)) = new Token(params[i]);
			}
			ptr->fn_body = new QVector<SIPtr>(body);
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

		static SIPtr println_stmt(Expr expr) {
			SIPtr ptr = create();
			ptr->ty = StmtTy::Println;
			ptr->expr = expr;
			return ptr;
		}

		static SIPtr ret_stmt(const Token& op, Expr ex){
			SIPtr ptr = create();
			ptr->ty = StmtTy::Return;
			ptr->retop = new Token(op);
			ptr->retval = ex;
			return ptr;
		}

		static SIPtr assert_stmt(Expr expr, int code, const TString& message){
			SIPtr ptr = create();
			ptr->ty = StmtTy::Assert;
			ptr->assertion = expr;
			ptr->code = code;
			ptr->msg = new TString(message);
			return ptr;
		}

		static SIPtr match_stmt(Expr m, const QVector<Branch*>& b){
			SIPtr ptr = create();
			ptr->ty = StmtTy::Match;
			ptr->match = m;
			ptr->branches = new QVector<Branch*>(b);
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

struct Branch {
	   Expr expr;
	   Stmt stmt;
	   Branch(Expr ex, Stmt st):expr(ex),stmt(st){}
	   ~Branch(){
		   LFn;
		   delete expr;
		   delete stmt;
		   LVd;
	   }
};

#define ExprStmt	StmtImpl::expr_stmt
#define VarDeclStmt	StmtImpl::var_decl_stmt
#define NullStmt	StmtImpl::null_stmt
#define BlockStmt	StmtImpl::block_stmt
#define IfStmt		StmtImpl::if_stmt
#define WhileStmt	StmtImpl::while_stmt
#define PrintStmt	StmtImpl::print_stmt
#define FnStmt		StmtImpl::fn_decl_stmt
#define RetStmt		StmtImpl::ret_stmt
#define AssertStmt	StmtImpl::assert_stmt
#define MatchStmt	StmtImpl::match_stmt
#define PrintlnStmt	StmtImpl::println_stmt

#endif // STMT_H
