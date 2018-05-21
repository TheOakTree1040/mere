#include "object.h"
#include "environment.h"
#include "runtimeerror.h"

using namespace mere;
EnvImpl::EnvImpl():enclosing(nullptr)
{}
EnvImpl::EnvImpl(EnvPtr encl):enclosing(encl){}
void EnvImpl::define(const Token& t, const Object& o){
	LFn;
	if (values.contains(t.lexeme)){
		LThw RuntimeError(t,TString("Redefinion of variable '").
						  append(t.lexeme).append("'."));
	}
	Log ls("LEXEME SIZE") ls(t.lexeme.size());
	Log ls("LEXEME CONT") ls(t.lexeme);
	if (t.lexeme.isEmpty()){
		LThw RuntimeError(t,"Defining variable with no identifier.");
	}
	Object& obj = values.insert(t.lexeme,o).value();
	obj.trait().set_on_stack();
	obj.fn_init();
	LVd;
}
void EnvImpl::define(const TString& t, const Object& o){
	LFn;
	if (values.contains(t)){
		LThw RuntimeError(Token(Tok::identifier,t,Object(),-1),TString("Redefinion of symbol '").
						  append(t).append("'."));
	}
	if (t.isEmpty()){
		LThw RuntimeError(Token(Tok::identifier,t,Object(),-1),"Definition without identifier.");
	}
	Object& obj = values.insert(t,o).value();
	obj.trait().set_on_stack();
	obj.fn_init();
	LVd;
}
Object& EnvImpl::access(const Token& t){
	LFn;
	Log1( "VARS = " + QStringList::fromStdList(values.keys().toStdList()).join(QString(", ")));
	Log1("ACCESSING " + t.lexeme);
	if (values.contains(t.lexeme)){
		LRet values[t.lexeme];
	}
	if (enclosing != nullptr){
		LRet enclosing->access(t);
	}
	LThw RuntimeError(t,TString("Variable '").append(t.lexeme).append("' undefined."));
}

Object& EnvImpl::assign(const Token& t, const Object& o){
	LFn;
	Log ls(t.lexeme) ls("at ln") ls(t.ln);
	if (values.contains(t.lexeme)){
		LRet values[t.lexeme].recv(o);
	}
	if (enclosing != nullptr){
		LRet enclosing->assign(t,o);
	}
	LThw RuntimeError(t,TString("Variable '").append(t.lexeme).append("' undefined."));
}

