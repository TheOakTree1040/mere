#include "expr.h"
#include "stmt.h"
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
		case ExprTy::FuncCall:
			delete func_args_list;
			delete callee;
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
		case ExprTy::Conditional:
			delete condition;
			delete l_branch;
			delete r_branch;
			break;
		case ExprTy::Assign:
			delete asgn_left;
			delete asgn_right;
			break;
		case ExprTy::Lambda:
			for (int i = param_ty->size() - 1; i >= 0; i--){
				delete param_ty->takeAt(i);
			}
			for (int i = param_names->size() - 1; i >= 0; i--){
				delete param_ty->takeAt(i);
			}
			delete param_ty;
			delete param_names;
			delete fn_block;
			break;
		case ExprTy::VarAcsr:
			delete var_acsr;
			break;
		//case ExprTy::MemAccessor:

	}
}
