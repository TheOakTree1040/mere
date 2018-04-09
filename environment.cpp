
#include "environment.h"
EnvImpl::EnvImpl():enclosing(nullptr)
{}
EnvImpl::EnvImpl(EnvPtr encl):enclosing(encl){}
void EnvImpl::define(const Token& t, const Object& o)throw(RuntimeError){
	if (t.lexeme.isEmpty())
	if (values.contains(t.lexeme))
		throw RuntimeError(t,QString("Redefinion of variable '").
						   append(t.lexeme).append("'."));
	if (values.isEmpty())
		throw RuntimeError(t,"Defining variable with no identifier.");
	if (!o.trait.is_typed())
		throw RuntimeError(t,QString("Initializer of variable '").append(t.lexeme)
						   .append("' is untyped."));
	values.insert(t.lexeme,o);
}

Object& EnvImpl::access(const Token& t)throw(RuntimeError){
	if (values.contains(t.lexeme))
		return values[t.lexeme];
	if (enclosing != nullptr)
		return enclosing->access(t);
	throw RuntimeError(t,QString("Variable '").append(t.lexeme).append("' undefined."));
}

Object& EnvImpl::assign(const Token& t, const Object& o)throw(RuntimeError){
	if (values.contains(t.lexeme))
		return values[t.lexeme] = o;
	if (enclosing != nullptr)
		return enclosing->assign(t,o);
	throw RuntimeError(t,QString("Variable '").append(t.lexeme).append("' undefined."));
}
