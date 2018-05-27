#pragma once

#ifndef MERE_CORE_H
#define MERE_CORE_H

#include <vector>
#include "src.h"

//Qt Forward decl.
class QString;
class QFile;

/**
 * \brief The mere namespace holds all declarations in this program.
 */
namespace mere {

	//Forward decls.
	class Token;
	class RuntimeError;
	class Interpreter;

	/**
	 * \brief The Core of this interpreter.
	 */
	class Core final {
		public:
			/// \brief The Error class holds the line # and the error message.
			struct Error;

		private:
			//Core shouldn't be instantiated.
			Core() = delete;

		private:
			static std::vector<Error> errors;
			static bool intd;

		private:
			static void init_once();
			static void report(const errloc_t&, const QString&);

		public:
			static Interpreter* interpreter; ///< \brief The interpreter.

		public:
			static bool run(const QString&, bool = false, bool = false);
			static bool run(QFile&);
			static void error(const errloc_t&, const QString&);
			static void error(const QString&);
			static void runtime_error(const RuntimeError&);
			static void show_errors();
			static void reset_intp();
	};
}

#endif // MERE_CORE_H
