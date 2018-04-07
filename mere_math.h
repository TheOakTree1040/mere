#ifndef MERE_MATH_FWD_H
#define MERE_MATH_FWD_H
#include <QString>
#include <QFile>
#include "token.h"
#include "runtimeerror.h"

class Interpreter;
class MereMath{
	private:
		static Interpreter* interpreter;
	public:
		static bool has_error;
		static bool has_runtime_error;
		static void run(const QString&);
		static void run_file(QFile&);
		static void error(int, const QString&);
		static void report(int, const QString&, const QString&, bool=true);
		static void error(const Token&, const QString&);
		static void runtime_error(const RuntimeError&);
};

#endif // MERE_MATH_FWD_H
