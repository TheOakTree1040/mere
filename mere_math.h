
#ifndef MERE_MATH_FWD_H
#define MERE_MATH_FWD_H
#include <QString>
#include <QFile>
#include "token.h"
#include "runtimeerror.h"
#include <QVector>

class Interpreter;
class MereMath{
	public:
		struct Error{
				Error():ln(0),msg(""){}
				Error(int l, const QString& m):ln(l),msg(m){}
				int ln;
				QString msg;
		};
	public:
		static Interpreter* interpreter;
	private:
		static QVector<Error> errors;
	public:
		static void run(const QString&);
		static void run_file(QFile&);
		static void error(int, const QString&);
		static void report(int, const QString&, const QString&, bool=true);
		static void error(const Token&, const QString&);
		static void runtime_error(const RuntimeError&);
		static void show_errors();
};

#endif // MERE_MATH_FWD_H
