#include "stmt.h"
StmtImpl::StmtImpl(){
	ty = StmtTy::Invalid;
	_inv = true;
}

StmtImpl::~StmtImpl(){
	switch(ty){
		case StmtTy::ExprStmt:
			delete expr;
			break;
		case StmtTy::DeclStmt:
			delete var_name;
			delete init;
			break;
	}
}
