#include "runtimeerror.h"

mere::ArgumentError::ArgumentError(const TString& exp, const TString& rec, const TString c):
	std::runtime_error("ArgError"),
	expect(exp),
	received(rec),
	callee(c){}

mere::ArityMismatchError::ArityMismatchError(int expect, int received, const TString c):
	std::runtime_error("AMError"),
	expect(expect),
	received(received),
	callee(c){}

mere::Abort::Abort(int code, const TString& msg):
	std::runtime_error(msg.toStdString()),
	code(code),
	message(msg){}
