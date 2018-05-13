#include "merecallable.h"
#include "interpreter.h"
int MereCallable::default_traits = 0b1000;
Object MereCallable::call(Interpreter& interpreter, QVector<Object>& arguments){
	LFn;
	try{
        if (is(Call::Reg)){
			Environment env = new EnvImpl(interpreter.global());
            int sz = m_fn->fn_params->size();
            for (int i = 0; i != sz; i++){
                env->define(*(m_fn->fn_params->at(i)),arguments[i]);
            }
			interpreter.exec_block(m_fn->fn_body,false,env);
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
