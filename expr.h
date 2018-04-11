#ifndef EXPR_H
#define EXPR_H
#include "token.h"
#include "object.h"
#include <QSharedPointer>
#include <QVariant>
#include <QVector>

#include "logger.h"
#define EIPtr ExprImpl*

using std::pair;

class StmtImpl;

enum class ExprTy {
	Invalid,
	Group,
	Literal,
//	Variable,
	Binary,
	Prefix,
	Postfix,
	Map,
	Hash,
	Array,
	ArgsList,
	FuncCall,
//	LValue,
	CommaEx,
	VarAcsr,
	MemAccessor,
	Assign,
	Conditional,
	Lambda,
	Logical,
	Refer
	//Change destr if you want to add any.

};
struct ExprImpl final{
	public:
		union{
				struct{
						EIPtr expr;
						EIPtr right;
						Token* op;
				};//binop, unop, group, etc.

				QVector<EIPtr>* array_data;//array
				QVector<std::pair<EIPtr,EIPtr>>* map_data;//map
				QVector<std::pair<EIPtr,EIPtr>>* hash_data;//hash
				Token* var_acsr;
				Object* lit;//
				struct{
						EIPtr callee;
						EIPtr func_args_list;
				};
				struct{
						EIPtr refer_left;
						EIPtr refer_right;
						Token* refer_op;
				};
				struct{
						EIPtr asgn_left;
						EIPtr asgn_right;
						Token* asgn_op;
				};
				struct{
						EIPtr condition;
						EIPtr l_branch;
						EIPtr r_branch;
				};
				QVector<EIPtr>* args_list;
				EIPtr lval_expr;
				QVector<EIPtr>* comma_exprs;
				QVector<Token*>* acsr;
				struct{//temp. function
						QVector<Token*>* param_names;
						QVector<EIPtr>* param_ty;//accessors
						StmtImpl* fn_block;
				};
				bool _invalid;
		};
		ExprTy ty;
	private:
		static EIPtr create(){
			return new ExprImpl();
		}

	public:
		ExprImpl(){
			ty = ExprTy::Invalid;
			_invalid = true;
		}
		~ExprImpl();

		static EIPtr binary(EIPtr l, const Token& o, EIPtr r){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Binary;
			ptr->expr = l;
			ptr->right = r;
			ptr->op = new Token(o);
			return ptr;
		}

		static EIPtr postfix(EIPtr ex, const Token& o){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Postfix;
			ptr->expr = ex;
			ptr->right = nullptr;
			ptr->op = new Token(o);
			return ptr;
		}

		static EIPtr prefix(const Token& o, EIPtr ex){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Prefix;
			ptr->expr = ex;
			ptr->right = nullptr;
			ptr->op = new Token(o);
			return ptr;
		}

		static EIPtr group(EIPtr ex){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Group;
			ptr->expr = ex;
			ptr->right = nullptr;
			return ptr;
		}
/*
		static EIPtr variable(const Token& n, bool at_glob = false){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Variable;
			ptr->at_global = at_glob;
			ptr->var_name = new Token(n);
			return ptr;
		}
*/
		static EIPtr literal(const Object& raw){
			LFn;
			Log << ":::by ref";
			EIPtr ptr = create();
			ptr->ty = ExprTy::Literal;
			ptr->lit = new Object(raw);
			Log << ":::" << raw.trait.id();
			LRet ptr;
		}

		static EIPtr literal(Object* raw){
			LFn;
			Log << "by ptr";
			EIPtr ptr = create();
			ptr->ty = ExprTy::Literal;
			ptr->lit = raw;
			Log << raw->trait.id();
			LRet ptr;
		}

		static EIPtr array(const QVector<EIPtr>& initializers){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Array;
			ptr->array_data = new QVector<EIPtr>(initializers);
			return ptr;
		}

		static EIPtr assoc(const QVector<pair<EIPtr,EIPtr>> &m_data){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Map;
			ptr->map_data = new QVector<pair<EIPtr,EIPtr>>(m_data);
			return ptr;
		}

		static EIPtr hash(const QVector<pair<EIPtr,EIPtr>> &h_data){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Hash;
			ptr->hash_data = new QVector<pair<EIPtr,EIPtr>>(h_data);
			return ptr;
		}

		static EIPtr args(const QVector<EIPtr>& list){
			EIPtr ptr = create();
			ptr->ty = ExprTy::ArgsList;
			ptr->args_list = new QVector<EIPtr>(list);
			return ptr;
		}

		static EIPtr func_call(EIPtr callee, EIPtr args_li){
			EIPtr ptr = create();
			ptr->ty = ExprTy::FuncCall;
			ptr->callee = callee;
			if (args_li->is(ExprTy::ArgsList))
				ptr->args_list = args_li->args_list;
			else
				ptr->args_list = nullptr;
			return ptr;
		}

//		static EIPtr lvalue(EIPtr expr){
//			EIPtr ptr = create();
//			ptr->ty = ExprTy::LValue;
//			if (expr->is(ExprTy::VarAcsr) || expr->is(ExprTy::MemAccessor) || true)
//				ptr->lval_expr = expr;
//			return ptr;
//		}

		static EIPtr invalid(){
			return create();
		}

		static EIPtr var_accessor(const Token& v_name){
			EIPtr ptr = create();
			ptr->ty = ExprTy::VarAcsr;
			ptr->var_acsr = new Token(v_name);
			return ptr;
		}

		static EIPtr mem_accessor();

		static EIPtr comma_ex(const QVector<EIPtr>& cex){
			EIPtr ptr = create();
			ptr->ty = ExprTy::CommaEx;
			ptr->comma_exprs = new QVector<EIPtr>(cex);
			return ptr;
		}

		static EIPtr assignment(EIPtr l, const Token& op, EIPtr r){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Assign;
			ptr->asgn_op = new Token(op);
			ptr->asgn_left = l;
			ptr->asgn_right = r;
			return ptr;
		}

		static EIPtr lambda(const QVector<Token>& pn, const QVector<EIPtr>& t, StmtImpl* fnb){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Lambda;
			ptr->param_names = new QVector<Token*>();
			int s = pn.size();
			for (int i = 0; i != s; i++)
				ptr->param_names->push_back(new Token(pn.at(i)));
			ptr->param_ty = new QVector<EIPtr>(t);
			ptr->fn_block = fnb;
			return ptr;
		}

		static EIPtr conditional(EIPtr condit, EIPtr l, EIPtr r){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Conditional;
			ptr->condition = condit;
			ptr->l_branch = l;
			ptr->r_branch = r;
			return ptr;
		}

		static EIPtr logical(EIPtr l, const Token& op, EIPtr r){
			EIPtr ptr = binary(l,op,r);
			ptr->ty = ExprTy::Logical;
			return ptr;
		}

		static EIPtr refer(EIPtr rl, const Token& refop, EIPtr rr){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Refer;
			ptr->refer_left = rl;
			ptr->refer_right = rr;
			ptr->refer_op = new Token(refop);
			return ptr;
		}

		ExprTy type(){
			return ty;
		}

		bool is(ExprTy t){
			return ty == t;
		}
};

typedef EIPtr Expr;

#define InvalidExpr		ExprImpl::invalid

#define LitExpr			ExprImpl::literal
#define BinExpr			ExprImpl::binary
#define LogicalExpr		ExprImpl::logical
#define AssignExpr		ExprImpl::assignment
#define GroupExpr		ExprImpl::group
#define PstfxExpr		ExprImpl::postfix
#define PrefxExpr		ExprImpl::prefix
#define CSExpr			ExprImpl::comma_ex
#define CndtnlExpr		ExprImpl::conditional

#define VarAcsrExpr		ExprImpl::var_accessor
#define MemberAcsrExpr	ExprImpl::mem_accessor

#define ArgsLiExpr		ExprImpl::args
#define FnCallExpr		ExprImpl::func_call

//#define LValExpr		ExprImpl::lvalue

#define ArrayExpr		ExprImpl::array
#define AssocExpr		ExprImpl::assoc
#define HashExpr		ExprImpl::hash

#define RefExpr			ExprImpl::refer

#endif // EXPR_H
