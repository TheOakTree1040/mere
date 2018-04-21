#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "token.h"
#include "mere_math.h"
#include "object.h"
#include <QVector>
#include <QChar>
class Tokenizer
{
	private:
		TString source;
		Tokens tokens;

		static QHash<TString, Tok> keywords;// defined in source
		static QHash<QChar,QChar> escaped;

		int start = 0;
		int current = 0;
		int line = 0;
	private:
		bool is_at_end();
		bool is_digit(char c);
		char peek(int=0);
		char peek_next();
		bool match(char expct);
		bool match(TString expct);
		char advance();
		void deprecate();

		void add_token(Tok);
		void add_token(Tok, const Object&);

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
		Tokenizer(const TString&);
		~Tokenizer();
		Tokens scan_tokens();
};

#endif // TOKENIZER_H
