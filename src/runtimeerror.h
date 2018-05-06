#ifndef RUNTIMEERROR_H
#define RUNTIMEERROR_H
#include <stdexcept>
#include "token.hpp"
class RuntimeError : public std::runtime_error{
	public:
		TString msg;
		Token tok;
		RuntimeError():std::runtime_error("UnknownRuntimeError"),msg("UnknownRuntimeError"),tok(){}
		RuntimeError(const Token& t, const TString& m) : std::runtime_error(m.toStdString()), msg(m), tok(t){}
};
class ArgumentError : public std::runtime_error{
	public:
		TString expect;
		TString received;
		TString callee;
		ArgumentError():std::runtime_error("UnknownArgError"),expect("null"),received("void"),callee("internal"){}
		ArgumentError(TString exp,TString rec, const TString c = "internal"):
			std::runtime_error("ArgError"),
			expect(exp),
			received(rec),
			callee(c){}
};
class ArityMismatchError : public std::runtime_error{
	public:
		int expect;
		int received;
		TString callee;
		ArityMismatchError():std::runtime_error("UnknownAME"),expect(0),received(0),callee("internal"){}
		ArityMismatchError(int expect, int received, const TString c = "internal"):
			std::runtime_error("AMError"),
			expect(expect),
			received(received),
			callee(c){}
};
class Abort : public std::runtime_error{
	//public:
		//static const QHash<int, TString> abort_codes;
	public:
		int code;
		TString message;
		Abort(int code,const TString& msg):std::runtime_error(msg.toStdString()),code(code),message(msg){}
};

#endif // RUNTIMEERROR_H
