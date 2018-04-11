
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
	if (t.lexeme.isEmpty()){
		LThw RuntimeError(t,"Defining variable with no identifier.");
	}
	if (!o.trait.is_typed()){
		LThw RuntimeError(t,QString("Initializer of variable '").append(t.lexeme)
						  .append("' is untyped."));
	}
	values.insert(t.lexeme,o);
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
	LThw RuntimeError(t,QString("Variable '").append(t.lexeme).append("' undefined."));
}

Object& EnvImpl::assign(const Token& t, const Object& o)throw(RuntimeError){
	LFn << t.lexeme << "at ln" << t.ln ;
	if (values.contains(t.lexeme)){
		LRet values[t.lexeme] = o;
	}
	if (enclosing != nullptr){
		LRet enclosing->assign(t,o);
	}
	LThw RuntimeError(t,QString("Variable '").append(t.lexeme).append("' undefined."));
}
