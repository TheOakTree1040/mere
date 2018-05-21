#ifndef MERE_MATH_FWD_H
#define MERE_MATH_FWD_H

#include "config.h"
#include <vector>

class QFile;

namespace mere {
	class Token;
	class RuntimeError;
	class Interpreter;

	class Core{
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
			static void init_once();
		private:
			static std::vector<Error> errors;
			static bool intd;
		public:
			static bool run(const TString&, bool=false, bool=false);
			static bool run(QFile&);
			static void error(int, const TString&);
			static void error(const TString&);
			static void report(int, const TString&, const TString&, bool=true);
			static void error(const Token&, const TString&);
			static void runtime_error(const RuntimeError&);
			static void show_errors();
			static void reset_intp();
	};
}

#endif // MERE_MATH_FWD_H
