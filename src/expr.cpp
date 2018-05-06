#include "expr.h"
#include "stmt.h"
#include "merecallable.h"
ExprImpl::~ExprImpl(){
	switch(this->ty){
		case ExprTy::Logical:
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
			for (int i = arguments->size() - 1; i >= 0; i--){
				delete arguments->takeAt(i);
			}
			delete arguments;
			delete callee;
			delete call_paren;
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
			delete asgn_op;
			break;
		case ExprTy::Lambda:
			delete this->function;
			break;
		case ExprTy::VarAcsr:
			delete var_acsr;
			break;
		case ExprTy::Refer:
			delete refer_left;
			delete refer_right;
			delete refer_op;
			break;
		//case ExprTy::MemAccessor:

	}
}
