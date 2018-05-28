#pragma once

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "interpretationunit.h"

class QChar;

namespace mere {

	class Tokenizer {
		public:
			static const QHash<QString, Tokty> keywords;// defined in source
			static const QHash<QChar,QChar> escaped;

		private:
			const IntpUnit unit;
			const QString& source;
			Tokens& tokens;

			uint start = 0ul;
			uint current = 0ul;

			srcloc_t loc;
			srcloc_t start_loc;
		private:
			void error(const QString& errmsg);

			bool is_at_end() const;
			bool is_digit(char c);
			char peek(bool ahead=false) const;//ahead: peeks the next char instead of the current one
			bool match(char expect);
			char advance();
		  //void deprecate();

			void add_token(Tokty);
			void add_token(Tokty, const Object&);

			void raw_string();
			void string();
			void character();
			void number();
			//literal helpers
			bool is_alpha(char);
			bool is_alpha_numeric(char);
			bool is_base_16(char);
			bool is_base_8(char);

			void identifier();
			void scan_token();
		public:
			Tokenizer(IntpUnit unit);
			~Tokenizer();
			void scan_tokens();
	};
}

#endif // TOKENIZER_H
