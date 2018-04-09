#include "stmt.h"
StmtImpl::StmtImpl(){
	ty = StmtTy::Invalid;
	_inv = true;
}

StmtImpl::~StmtImpl(){
	switch(ty){
		case StmtTy::Expr:
			delete expr;
			break;
		case StmtTy::VarDecl:
			{
				Log << "del_var";
				delete var_name;
				Log << "del_init";
				delete init;
				Log << "del_type";
				delete var_type;
				Log << "del_done";
			}
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
		case StmtTy::While:
			delete cont_condit;
			delete while_block;
			break;
		case StmtTy::Empty:
			break;
		case StmtTy::Invalid:
			break;
	}
}
