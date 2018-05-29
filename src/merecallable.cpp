#include "merecallable.h"
#include "interpreter.h"
#include "runtimeerror.h"

using namespace mere;
int MereCallable::default_traits = 0b1000;

MereCallable::MereCallable(const MereCallable& mc):
	m_traits(mc.m_traits),
	m_arity(mc.m_arity){
	LFn;
	if (mc.is(Call::Reg)){
		m_impl = mc.m_impl;
	}
	else {
		m_callable = mc.m_callable;
	}
	LVd;
}

MereCallable::MereCallable():
	m_traits(default_traits),
	m_impl(new NullStmt()),
	m_closure(nullptr){}

MereCallable::MereCallable(const Callable& clb):
	m_traits(default_traits),
	m_callable(new Callable(clb)){
	as_nat();
}

MereCallable::MereCallable(const Stmt& fn_def, EnvImpl* closure):
	m_traits(default_traits),
	m_impl(new Stmt(fn_def)),
	m_closure(closure),
	m_arity(fn_def.fn().params().size()){
	as_reg();
}

MereCallable::~MereCallable(){
	LFn;
	if (is(Call::OnStack)){
		if (is(Call::Reg)){
			Logp("<---== deleting m_fn ==--->");
			delete m_impl;
		}
		else if (is(Call::Nat)){
			Logp("<---== deleting m_callable ==--->");
			delete m_callable;
		}
	}
	LVd;
}

MereCallable& MereCallable::set_arity(int i){
	if (!is(Call::Reg)){
		m_arity = i;
	}
	return *this;
}

Object MereCallable::call(Interpreter& interpreter, std::vector<Object>& arguments){
	LFn;
	try{
		if (is(Call::Reg)){
			Environment env = new EnvImpl(m_closure);
			auto params = m_impl->fn().params();
			int sz = params.size();
			for (int i = 0; i != sz; i++){
				env->define(params[i],arguments[i]);
			}
			interpreter.exec_block(m_impl->fn().body(),env);
			LRet Object();
		}
		else if (is(Call::Nat)){
			LRet (*m_callable)(interpreter,arguments);
		}
		LThw RuntimeError(Token(),"Invalid function call.");
	} catch(Return& ret){
		LRet ret.value();
	}
}

MereCallable& MereCallable::set(Call ind, bool val){
	m_traits.set(static_cast<size_t>(ind),val);
	return *this;
}

