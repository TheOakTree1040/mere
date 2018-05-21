#ifndef RUNTIMEERROR_H
#define RUNTIMEERROR_H

#include <stdexcept>
#include "token.h"

namespace mere {

	class RuntimeError : public std::runtime_error{
		public_fields:
			TString msg;
			Token tok;
		public_methods:
			RuntimeError():std::runtime_error("UnknownRuntimeError"),msg("UnknownRuntimeError"),tok(){}
			RuntimeError(const Token& t, const TString& m) : std::runtime_error(m.toStdString()), msg(m), tok(t){}
	};

	class ArgumentError : public std::runtime_error{
		public_fields:
			TString expect;
			TString received;
			TString callee;
		public_methods:
			ArgumentError():std::runtime_error("UnknownArgError"),expect("null"),received("void"),callee("internal"){}
			ArgumentError(const TString& exp, const TString& rec, const TString c = "internal");
	};

	class ArityMismatchError : public std::runtime_error{
		public_fields:
			int expect;
			int received;
			TString callee;
		public_methods:
			ArityMismatchError():std::runtime_error("UnknownAME"),expect(0),received(0),callee("internal"){}
			ArityMismatchError(int expect, int received, const TString c = "internal");
	};

	class Abort : public std::runtime_error{
		public_fields:
			int code;
			TString message;
		public_methods:
			Abort(int code, const TString& msg);
	};

}

#endif // RUNTIMEERROR_H
