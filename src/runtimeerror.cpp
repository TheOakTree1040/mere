#include "runtimeerror.h"

mere::ArgumentError::ArgumentError(const QString& exp, const QString& rec, const QString c):
	std::runtime_error("ArgError"),
	expect(exp),
	received(rec),
	callee(c){}

mere::ArityMismatchError::ArityMismatchError(int expect, int received, const QString c):
	std::runtime_error("AMError"),
	expect(expect),
	received(received),
	callee(c){}

mere::Abort::Abort(int code, const QString& msg):
	std::runtime_error(msg.toStdString()),
	code(code),
	message(msg){}
