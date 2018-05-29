#pragma once

#ifndef INTERPRETATIONUNIT_H
#define INTERPRETATIONUNIT_H

#include "token.h"
#include "stmt.h"

class QFile;

namespace mere {
	class Tokenizer;
	class Parser;
	class Interpreter;
	class Resolver;

	typedef class InterpretationUnit {
			friend class Tokenizer;
			friend class Parser;
			friend class Interpreter;
			friend class Resolver;

		private:
			static bool _show_snippet;

		public:
			struct Issue{
					Issue():loc(),msg(""){}
					Issue(const srcloc_t loc, const QString& m):loc(loc),msg(m){}
					srcloc_t loc;
					QString msg;///< \brief The error message
			};

		private:
			QString m_filename;
			QString m_source;

			std::vector<Issue> issues{};///< \brief It stores all the errors occurred
			std::vector<uint> m_line_indices{0ul/*placeholder*/, 0ul/*line 1 starts @ 0*/};

			Tokens m_toks;
			AST m_ast;

		private:
			InterpretationUnit(){}
			InterpretationUnit (const InterpretationUnit&) = delete;

#if TEST
		public:
#else
		private:
#endif
			QString& source() noexcept { return m_source; }
			Tokens& tokens() noexcept { return m_toks; }
			AST& ast() noexcept { return m_ast; }

		public:
			InterpretationUnit(const QString& filename);
			InterpretationUnit(QFile& file);
			static InterpretationUnit* from_source(const QString&);

			QString filename() const noexcept { return m_filename; }

			void add_index(uint i) { m_line_indices.push_back(i); }
			uint last_index() const { return *m_line_indices.end(); }
			uint index_at(uint line) const { return m_line_indices[line]; }
			void print_tokens() const;
			void print_issues() const;
			QString snippet_at(int, const srcloc_t&) const;

			void report(const srcloc_t& loc, const QString& type, const QString& msg);
			void report(const QString& type, const QString & msg);

			bool success() const { return issues.empty(); }

			static void show_snippet(bool b){
				_show_snippet = b;
			}
	} * IntpUnit;
}

#endif // INTERPRETATIONUNIT_H
