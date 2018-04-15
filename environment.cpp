
#include "environment.h"
EnvImpl::EnvImpl():enclosing(nullptr)
{}
EnvImpl::EnvImpl(EnvPtr encl):enclosing(encl){}
void EnvImpl::define(const Token& t, const Object& o)throw(RuntimeError){
	LFn;
	if (values.contains(t.lexeme)){
		LThw RuntimeError(t,QString("Redefinion of variable '").
						  append(t.lexeme).append("'."));
	}
	//if (t.lexeme.isEmpty()){
	//	LThw RuntimeError(t,"Defining variable with no identifier.");
	//}
	Object s(o);
	s.trait().set_on_stack();
	values.insert(t.lexeme,s);
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
	Log << "Attempting to throw.";
	LThw RuntimeError(t,QString("Variable '").append(t.lexeme).append("' undefined."));
}

Object& EnvImpl::assign(const Token& t, const Object& o)throw(RuntimeError){
	LFn << t.lexeme << "at ln" << t.ln ;
	if (values.contains(t.lexeme)){
		LRet values[t.lexeme].recv(o);
	}
	if (enclosing != nullptr){
		LRet enclosing->assign(t,o);
	}
	LThw RuntimeError(t,QString("Variable '").append(t.lexeme).append("' undefined."));
}
