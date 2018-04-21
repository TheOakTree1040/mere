
#include "environment.h"
EnvImpl::EnvImpl():enclosing(nullptr)
{}
EnvImpl::EnvImpl(EnvPtr encl):enclosing(encl){}
void EnvImpl::define(const Token& t, const Object& o)throw(RuntimeError){
	LFn;
	Log << "Checking table for existing entry...";
	if (values.contains(t.lexeme)){
		LThw RuntimeError(t,TString("Redefinion of variable '").
						  append(t.lexeme).append("'."));
	}
	Log << "Start:";
	//if (t.lexeme.isEmpty()){
	//	LThw RuntimeError(t,"Defining variable with no identifier.");
	//}
	//Log << "setting:" << t.lexeme;
	values.insert(t.lexeme,o);
	Object& obj = values[t.lexeme];
	obj.trait().set_on_stack();
	obj.fn_init();
	Log << "OBJ_VAL" << obj.to_string();
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
	LFn << t.lexeme << "at ln" << t.ln ;
	if (values.contains(t.lexeme)){
		LRet values[t.lexeme].recv(o);
	}
	if (enclosing != nullptr){
		LRet enclosing->assign(t,o);
	}
	LThw RuntimeError(t,TString("Variable '").append(t.lexeme).append("' undefined."));
}
