#include "stmt.h"
StmtImpl::StmtImpl(){
	ty = StmtTy::Invalid;
	_inv = true;
}

StmtImpl::~StmtImpl(){
	switch(ty){
		case StmtTy::Print:
		case StmtTy::Expr:
			delete expr;
			break;
		case StmtTy::VarDecl:
			delete var_name;
			delete init;
			delete var_type;
			break;
		case StmtTy::If:
			delete if_block;
			delete else_block;
			delete condition;
			break;
		case StmtTy::Block:
			for (int i = block->size() - 1; i >= 0; i--)
				delete block->takeAt(i);
			delete block;
			break;
		case StmtTy::Function:
			for (int i = fn_params->size() - 1; i >= 0; i--)
				delete fn_params->takeAt(i);
			delete fn_params;
			for (int i = fn_body->size() - 1; i >= 0; i--)
				delete fn_body->takeAt(i);
			delete fn_body;
			delete fn_name;
			break;
		case StmtTy::While:
			delete cont_condit;
			delete while_block;
			break;
		case StmtTy::Return:
			delete retval;
			delete retop;
			break;
		case StmtTy::Empty:
			break;
		case StmtTy::Invalid:
			break;
	}
}
