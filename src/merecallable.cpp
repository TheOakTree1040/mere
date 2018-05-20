#include "merecallable.h"
#include "interpreter.h"

using namespace mere;
int MereCallable::default_traits = 0b1000;
Object MereCallable::call(Interpreter& interpreter, std::vector<Object>& arguments){
	LFn;
	try{
		if (is(Call::Reg)){
			Environment env = new EnvImpl(interpreter.global());
			auto params = m_fn->fn().params();
			int sz = params.size();
			for (int i = 0; i != sz; i++){
				env->define(params[i],arguments[i]);
			}
			interpreter.exec_block(m_fn->fn().body(),false,env);
			LRet Object();
		}
		else if (is(Call::Nat)){
			LRet (*m_callable)(interpreter,arguments);
		}
		LThw RuntimeError(Token(),"Invalid function call.");
	} catch(Return& ret){
		return ret.value();
	}
}

