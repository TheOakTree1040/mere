#include "expr.h"

ExprImpl::~ExprImpl(){
	switch(this->ty){
		case ExprTy::Binary:
			delete this->right;
		case ExprTy::Prefix:
		case ExprTy::Postfix:
			delete this->op;
		case ExprTy::Group:
			delete this->expr;
			break;
		case ExprTy::Literal:
			delete this->lit;
			break;
		case ExprTy::Variable:
			delete this->var_name;
			break;
		case ExprTy::Hash:
			for (int i = hash_data->size() - 1; i >= 0; i--){
				delete hash_data->at(i).first;
				delete hash_data->takeAt(i).second;
			}
			delete this->hash_data;
			break;
		case ExprTy::Map:
			for (int i = map_data->size() - 1; i >= 0; i--){
				delete map_data->at(i).first;
				delete map_data->takeAt(i).second;
			}
			delete this->map_data;
			break;
		case ExprTy::Array:
			for (int i = array_data->size() - 1; i >= 0; i--){
				delete array_data->takeAt(i);
			}
			delete this->array_data;
			break;
		case ExprTy::MemberAccessor:
			for (int i = member_accessor->size() - 1; i >= 0; i--){
				delete member_accessor->takeAt(i);
			}
			delete member_accessor;
		case ExprTy::ScopeAccessor:
			delete scope_accessor;
			break;
		case ExprTy::FuncCall:
			delete func_args_list;
			delete func_name;
			break;
		case ExprTy::ArgsList:
			for (int i = args_list->size() - 1; i >= 0; i--){
				delete args_list->takeAt(i);
			}
			delete args_list;
			break;
		case ExprTy::LValue:
			delete lval_expr;
			break;
		case ExprTy::CommaEx:
			for (int i = comma_exprs->size() - 1; i >= 0; i--){
				delete comma_exprs->takeAt(i);
			}
			delete comma_exprs;
			break;
		case ExprTy::Invalid:
			break;
		case ExprTy::AccessorItem:
			delete accessor_item;
			break;
		case ExprTy::Conditional:
			delete condition;
			delete l_branch;
			delete r_branch;
			break;
		case ExprTy::AssignExpr:
			delete asgn_mem_acsr;
			delete asgn_mem_acsr;
			break;
	}
}
