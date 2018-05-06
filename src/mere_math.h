
#ifndef MERE_MATH_FWD_H
#define MERE_MATH_FWD_H
#include "config.hpp"
#include <QFile>
#include "token.hpp"
#include "runtimeerror.h"
#include <QVector>

//forward declaration of Interpreter
class Interpreter;

class MereMath{
	public:
		struct Error{
				Error():ln(0),msg(""){}
				Error(int l, const TString& m):ln(l),msg(m){}
				int ln;
				TString msg;
		};
	public:
		static Interpreter* interpreter;
	private:
		static QVector<Error> errors;
	public:
		static void init_once();
		static bool run(const TString&, bool=false, bool=false);
		static bool run_file(QFile&);
		static void error(int, const TString&);
		static void error(const TString&);
		static void report(int, const TString&, const TString&, bool=true);
		static void error(const Token&, const TString&);
		static void runtime_error(const RuntimeError&);
		static void show_errors();
		static void reset_intp();
};

#endif // MERE_MATH_FWD_H
