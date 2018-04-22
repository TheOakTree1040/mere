#include "natives.h"
#include "interpreter.h"
#define FN_PARAM Interpreter& interpreter, QVector<Object>& arguments
#define CHECK(CALLEE,EXPECT) t::check_param(CALLEE,EXPECT,arguments)
void t::check_param(const TString& callee, QVector<TString>&& expect,QVector<Object>& received_obj){
	LFn;
	int size = expect.size();
	if (size != received_obj.size()){
		LThw ArityMismatchError(size,received_obj.size(),callee);
	}
	QVector<TString> received(received_obj.size());
	for (int i = 0; i != received.size(); i++){
		received[i] = received_obj[i].trait().id();
	}
	for (int i = 0; i != size; i++){
		if (expect[i] != received[i]){
			LThw ArgumentError(expect[i],received[i],callee);
		}
	}
}

//void t::_init(){
//	t::clock.set_arity(0);
//	t::time.set_arity(0);
//	t::sin.set_arity(1);
//	t::cos.set_arity(1);
//	t::tan.set_arity(1);
//}
//
//MereCallable t::clock = MereCallable(CALLABLE{
//							Q_UNUSED(interpreter);
//							CHECK("clock",QVector<TString>{});
//							return Object(t_cast<double>(::clock()));
//						});
//MereCallable t::time = MereCallable(CALLABLE{
//							Q_UNUSED(interpreter);
//							CHECK("time",QVector<TString>{"string"});
//							return Object(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
//						});
//MereCallable t::sin = MereCallable(CALLABLE{
//					  Q_UNUSED(interpreter);
//					  CHECK("sin",QVector<TString>{"real"});
//					  return Object(::sin(arguments[0].as<double>()));
//					  });
//MereCallable t::cos = MereCallable(CALLABLE{
//					  Q_UNUSED(interpreter);
//					  CHECK("cos",QVector<TString>{"real"});
//					  return Object(::cos(arguments[0].as<double>()));
//					  });
//MereCallable t::tan = MereCallable(CALLABLE{
//					  Q_UNUSED(interpreter);
//					  CHECK("tan",QVector<TString>{"real"});
//					  return Object(::tan(arguments[0].as<double>()));
//					  });
