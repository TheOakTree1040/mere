
#include "environment.h"
EnvImpl::EnvImpl():enclosing(nullptr)
{}
EnvImpl::EnvImpl(EnvPtr encl):enclosing(encl){}
void EnvImpl::define(const Token& t, const Object& o)throw(RuntimeError){
	LFn;
	if (values.contains(t.lexeme)){
		LThw RuntimeError(t,TString("Redefinion of variable '").
						  append(t.lexeme).append("'."));
	}
#if _DEBUG
    Log << "LEXEME SIZE" << t.lexeme.size();
    Log << "LEXEME CONT" << t.lexeme;
#endif
	if (t.lexeme.isEmpty()){
		LThw RuntimeError(t,"Defining variable with no identifier.");
	}
	Object& obj = values.insert(t.lexeme,o).value();
	obj.trait().set_on_stack();
	obj.fn_init();
	LVd;
}
void EnvImpl::define(const TString& t, const Object& o)throw(RuntimeError){
	LFn;
	if (values.contains(t)){
		LThw RuntimeError(Token(Tok::IDENTIFIER,t,Object(),-1),TString("Redefinion of symbol '").
						  append(t).append("'."));
	}
	if (t.isEmpty()){
		LThw RuntimeError(Token(Tok::IDENTIFIER,t,Object(),-1),"Definition without identifier.");
	}
	Object& obj = values.insert(t,o).value();
	obj.trait().set_on_stack();
	obj.fn_init();
	LVd;
}
Object& EnvImpl::access(const Token& t)throw(RuntimeError){
	LFn;
	if (values.contains(t.lexeme)){
		LRet values[t.lexeme];
	}
	if (enclosing != nullptr){
		LRet enclosing->access(t);
	}
	LThw RuntimeError(t,TString("Variable '").append(t.lexeme).append("' undefined."));
}

Object& EnvImpl::assign(const Token& t, const Object& o)throw(RuntimeError){
#if _DEBUG
	LFn << t.lexeme << "at ln" << t.ln ;
#endif
	if (values.contains(t.lexeme)){
		LRet values[t.lexeme].recv(o);
	}
	if (enclosing != nullptr){
		LRet enclosing->assign(t,o);
	}
	LThw RuntimeError(t,TString("Variable '").append(t.lexeme).append("' undefined."));
}
