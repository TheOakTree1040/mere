
#ifndef MERE_MATH_FWD_H
#define MERE_MATH_FWD_H
#include "t.hpp"
#include <QFile>
#include "token.hpp"
#include "runtimeerror.h"
#include <QVector>

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
		static void run(const TString&, bool=false, bool=false);
		static void run_file(QFile&);
		static void error(int, const TString&);
		static void report(int, const TString&, const TString&, bool=true);
		static void error(const Token&, const TString&);
		static void runtime_error(const RuntimeError&);
		static void show_errors();
		static void reset_intp();
};

#endif // MERE_MATH_FWD_H
