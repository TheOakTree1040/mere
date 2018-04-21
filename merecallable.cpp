#include "merecallable.h"
#include "interpreter.h"
int MereCallable::default_traits = 0b1000;
Object MereCallable::call(Interpreter& interpreter, QVector<Object>& arguments){
	LFn;
	try{
		if (is(Call::Reg)){
			Log << "newing env";
			Environment env = new EnvImpl(interpreter.global());
			int sz = m_fn->fn_params->size();
			Log << "...";
			for (int i = 0; i != sz; i++){
				Log << "defining";
				env->define(*(m_fn->fn_params->at(i)),(Log << arguments[i].to_string(),arguments[i]));
			}
			Log << "exec.";
			interpreter.exec_block(m_fn->fn_body,false,env);
			LRet Object();
		}
		else if (is(Call::Nat)){
			LRet m_callable(interpreter,arguments);
		}
		LThw RuntimeError(Token(),"Invalid function call.");
	} catch(Return& ret){
		Log << "RET_VAL" << ret.value().to_string();
		return ret.value();
	}
}
