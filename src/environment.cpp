#include "object.h"
#include "environment.h"
#include "runtimeerror.h"

using namespace mere;
EnvImpl::EnvImpl():enclosing(nullptr)
{}
EnvImpl::EnvImpl(EnvPtr encl):enclosing(encl){}
void EnvImpl::define(const Token& t, const Object& o){
	LFn;
	if (values.contains(t.lexeme())){
		LThw RuntimeError(t,QString("redefinion of variable '").
						  append(t.lexeme()).append("'"));
	}
	Log ls("LEXEME") ls(t.lexeme());
	if (t.lexeme().isEmpty()){
		LThw RuntimeError(t,"defining variable with no identifier");
	}
	Object& obj = values.insert(t.lexeme(),o).value();
	obj.trait().set_on_stack();
	obj.fn_init();
	LVd;
}
void EnvImpl::define(const QString& t, const Object& o){
	LFn;
	if (values.contains(t)){
		LThw RuntimeError(Token(Tok::identifier,t,Object(),srcloc_t()),QString("redefinion of var '").
						  append(t).append("'."));
	}
	if (t.isEmpty()){
		LThw RuntimeError(Token(Tok::identifier,t,Object(),srcloc_t()),"definition without identifier");
	}
	Object& obj = values.insert(t,o).value();
	obj.trait().set_on_stack();
	obj.fn_init();
	LVd;
}
Object& EnvImpl::access(const Token& t){
	LFn;
	Log1( "VARS = " + QStringList::fromStdList(values.keys().toStdList()).join(QString(", ")));
	Log1("ACCESSING " + t.lexeme());
	if (values.contains(t.lexeme())){
		Object& o = values[t.lexeme()];
		Log1("RETURNING...");
		LRet o;
	}
	if (enclosing != nullptr){
		LRet enclosing->access(t);
	}
	LThw RuntimeError(t,QString("variable '").append(t.lexeme()).append("' undefined"));
}

EnvImpl*EnvImpl::ancestor(int dist){
	EnvImpl* env = this;
	for (int i = 0; i < dist; i++){
		env = env->enclosing;
	}
	return env;
}

Object& EnvImpl::assign(const Token& t, const Object& o){
	LFn;
	Log ls(t.lexeme()) ls("at ln") ls(t.line());
	if (values.contains(t.lexeme())){
		LRet values[t.lexeme()].receive_from(o);
	}
	if (enclosing != nullptr){
		LRet enclosing->assign(t,o);
	}
	LThw RuntimeError(t,QString("variable '").append(t.lexeme()).append("' undefined"));
}

