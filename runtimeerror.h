#ifndef RUNTIMEERROR_H
#define RUNTIMEERROR_H
#include <stdexcept>
#include "token.h"
class RuntimeError : public std::runtime_error{
	public:
		TString msg;
		Token tok;
		RuntimeError():std::runtime_error("UnknownRuntimeError"),msg("UnknownRuntimeError"),tok(){}
		RuntimeError(const Token& t, const TString& m) : std::runtime_error(m.toStdString()), msg(m), tok(t){}
};
#endif // RUNTIMEERROR_H
