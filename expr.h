#ifndef EXPR_H
#define EXPR_H
#include "token.h"
#include "object.h"
#include <QSharedPointer>
#include <QVariant>
#include <QVector>

#include <QDebug>
#define EIPtr ExprImpl*

using std::pair;

class StmtImpl;

enum class ExprTy {
	Invalid,
	Group,
	Literal,
	Variable,
	Binary,
	Prefix,
	Postfix,
	Map,
	Hash,
	Array,
	MemberAccessor,
	ScopeAccessor,
	ArgsList,
	FuncCall,
	LValue,
	CommaEx,
	AccessorItem,
	AssignExpr,
	Conditional,
	Lambda
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

				Object* lit;//
				struct{
						bool at_global;
						union{
								struct{
										QVector<Token>* scope_accessor;
										QVector<EIPtr>* member_accessor;
								};
								struct{
										Token* var_name;
								};
						};
				};
				struct{
						EIPtr func_name;
						EIPtr func_args_list;
				};
				struct{
						EIPtr asgn_mem_acsr;
						Object* asgn_val;
				};
				struct{
						EIPtr condition;
						EIPtr l_branch;
						EIPtr r_branch;
				};
				QVector<EIPtr>* args_list;
				EIPtr lval_expr;
				QVector<EIPtr>* comma_exprs;
				Token* accessor_item;
				struct{//temp. function
						QVector<Token>* param_names;
						QVector<StmtImpl*>* stmts;
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

		static EIPtr variable(const Token& n, bool at_glob = false){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Variable;
			ptr->at_global = at_glob;
			ptr->var_name = new Token(n);
			return ptr;
		}

		static EIPtr literal(const Object& raw){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Literal;
			ptr->lit = new Object(raw);
			return ptr;
		}

		static EIPtr literal(Object* raw){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Literal;
			ptr->lit = raw;
			return ptr;
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

		static EIPtr member(EIPtr scope, const QVector<EIPtr>& accessor){
			EIPtr ptr = create();
			ptr->ty = ExprTy::MemberAccessor;
			ptr->member_accessor = new QVector<EIPtr>(accessor);
			ptr->scope_accessor = scope->scope_accessor;
			return ptr;
		}

		static EIPtr scope(const QVector<Token>& accessor, bool at_glob = false){
			EIPtr ptr = create();
			ptr->ty = ExprTy::ScopeAccessor;
			ptr->scope_accessor = new QVector<Token>(accessor);
			ptr->member_accessor = nullptr;
			ptr->at_global = at_glob;
			return ptr;
		}

		static EIPtr args(const QVector<EIPtr>& list){
			EIPtr ptr = create();
			ptr->ty = ExprTy::ArgsList;
			ptr->args_list = new QVector<EIPtr>(list);
			return ptr;
		}

		static EIPtr func_call(EIPtr fn_name, EIPtr args_li){
			EIPtr ptr = create();
			ptr->ty = ExprTy::FuncCall;
			if (fn_name->is(ExprTy::MemberAccessor))
				ptr->func_name = fn_name;
			else
				ptr->func_name = nullptr;
			if (args_li->is(ExprTy::ArgsList))
				ptr->args_list = args_li->args_list;
			else
				ptr->args_list = nullptr;
			return ptr;
		}

		static EIPtr lvalue(EIPtr expr){
			EIPtr ptr = create();
			ptr->ty = ExprTy::LValue;
			if (expr->is(ExprTy::Variable) || expr->is(ExprTy::MemberAccessor) || true)
				ptr->lval_expr = expr;
			return ptr;
		}

		static EIPtr invalid(){
			return create();
		}

		static EIPtr comma_ex(const QVector<EIPtr>& cex){
			EIPtr ptr = create();
			ptr->ty = ExprTy::CommaEx;
			ptr->comma_exprs = new QVector<EIPtr>(cex);
			return ptr;
		}

		static EIPtr acsr_item(const Token& tok){
			EIPtr ptr = create();
			ptr->ty = ExprTy::AccessorItem;
			ptr->accessor_item = new Token(tok);
			return ptr;
		}

		static EIPtr assignment(EIPtr mem_ascr, const Object& ro){
			EIPtr ptr = create();
			ptr->ty = ExprTy::AssignExpr;
			ptr->asgn_mem_acsr = mem_ascr;
			ptr->asgn_val = ro;
			return ptr;
		}

		static EIPtr lambda(const QVector<Token>& pn,const QVector<StmtImpl*>& s){
			EIPtr ptr = create();
			ptr->ty = ExprTy::Lambda;
			ptr->param_names = new QVector<Token>(pn);
			ptr->stmts = new QVector<StmtImpl*>(s);
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

		ExprTy type(){
			return ty;
		}

		bool is(ExprTy t){
			return ty == t;
		}
};

typedef EIPtr Expr;

#define BinExpr			ExprImpl::binary
#define GroupExpr		ExprImpl::group
#define VarExpr			ExprImpl::variable
#define LitExpr			ExprImpl::literal
#define PstfxExpr		ExprImpl::postfix
#define PrefxExpr		ExprImpl::prefix
#define ArrayExpr		ExprImpl::array
#define SetExpr			ExprImpl::set
#define ScopeAcsExpr	ExprImpl::scope
#define MemberAcsExpr	ExprImpl::member
#define ArgsLiExpr		ExprImpl::args
#define FnCallExpr		ExprImpl::func_call
#define LValExpr		ExprImpl::lvalue
#define InvalidExpr		ExprImpl::invalid
#define CSExpr			ExprImpl::comma_ex
#define AItemExpr		ExprImpl::acsr_item
#define AssignExpr		ExprImpl::assignment
#define CndtnlExpr		ExprImpl::conditional
#define AssocExpr		ExprImpl::assoc
#define HashExpr		ExprImpl::hash

#endif // EXPR_H
