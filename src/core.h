#ifndef MERE_MATH_FWD_H
#define MERE_MATH_FWD_H

#include "config.hpp"
#include <vector>

class QFile;

namespace mere {
	class Token;
	class RuntimeError;
	class Interpreter;

	class Core{
		public_decls:
			struct Error{
					Error():ln(0),msg(""){}
					Error(int l, const TString& m):ln(l),msg(m){}
					int ln;
					TString msg;
			};
		public_fields:
			static Interpreter* interpreter;
		private_methods:
			static void init_once();
		private_fields:
			static std::vector<Error> errors;
			static bool intd;
		public_methods:
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
