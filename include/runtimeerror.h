#pragma once

#ifndef RUNTIMEERROR_H
#define RUNTIMEERROR_H

#include <stdexcept>
#include "token.h"

namespace mere {

	class RuntimeError : public std::runtime_error{
		public:
			QString msg;
			Token tok;
		public:
			RuntimeError():std::runtime_error("UnknownRuntimeError"),msg("UnknownRuntimeError"),tok(){}
			RuntimeError(const Token& t, const QString& m) : std::runtime_error(m.toStdString()), msg(m), tok(t){}
	};

	class ArgumentError : public std::runtime_error{
		public:
			QString expect;
			QString received;
			QString callee;
		public:
			ArgumentError():std::runtime_error("UnknownArgError"),expect("null"),received("void"),callee("internal"){}
			ArgumentError(const QString& exp, const QString& rec, const QString c = "internal");
	};

	class ArityMismatchError : public std::runtime_error{
		public:
			int expect;
			int received;
			QString callee;
		public:
			ArityMismatchError():std::runtime_error("UnknownAME"),expect(0),received(0),callee("internal"){}
			ArityMismatchError(int expect, int received, const QString c = "internal");
	};

	class Abort : public std::runtime_error{
		public:
			int code;
			QString message;
		public:
			Abort(int code, const QString& msg);
	};

}

#endif // RUNTIMEERROR_H
