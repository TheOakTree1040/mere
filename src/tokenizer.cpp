
#include "tokenizer.h"
#include "tlogger.h"

namespace mere {
	Tokenizer::Tokenizer(const TString& src):source(src)
	{
		Log ls("Source: ") ls(source);
		LIndt;
	}
	Tokenizer::~Tokenizer(){
		LOdt;
		Logp("~Tokenizer()");
	}

	bool Tokenizer::is_at_end(){
		return current >= source.size();
	}

	char Tokenizer::advance(){
		Log ls("  adv:") ls(TString::number(current));
		Log ls("      ") ls(peek());
		return source[current++].toLatin1();
	}

	void Tokenizer::add_token(Tok::tok_type ty){
		add_token(ty, Object());
	}

	void Tokenizer::add_token(Tokty ty, const Object& lit){
		LFn;
		tokens.push_back(Token(ty,source.mid(start,current-start),lit,line));
		Log ls("Added Token: Lexeme:") ls(tokens[tokens.size()-1].lexeme);
		Log ls("             Type  :") ls((int)tokens[tokens.size()-1].ty);
		LVd;
	}

	char Tokenizer::peek(short i) {
		if (!source.size())
			return '\0';
		if (is_at_end() && i < 0 && (current + i) >= 0)
			return source[current + i].toLatin1();
		if (is_at_end())
			return '\0';
		if (current + i > source.size())
			return '\0';
		return source[current + i].toLatin1();
	}
	//var i = 0;
	//0123456789
	void Tokenizer::deprecate(){
		if (!is_at_end())
			source.remove(current,1);
	}

	bool Tokenizer::match(char expct) {
		return !is_at_end() && source[current] == expct?current++, true:false;
		}

		bool Tokenizer::match(TString expct){
		if (is_at_end())
		return false;
		if (current + 1 + expct.size() > source.size())
			return false;
		int tmp = current;
		for (int i = 0; i != expct.size(); i++, tmp++){
			if (source[tmp] != expct[i])
				return false;
		}
		current = tmp;
		return true;
	}

	void Tokenizer::string(){
		TString str = "";
		current--;
		deprecate();
		while (!is_at_end()) {
			if (peek() == '\n') {
				Core::error(line, "Expected a closing quotation mark to terminate string.");
				return;
			}
			if (peek() == '\\'){
				QChar ch = escaped.value(QChar(peek(1)),QChar('*'));
				if (ch == QChar('*')){
					Core::error(line,"Undefined escape sequence.");
				}

				deprecate();
				source[current] = ch;
			}
			if (match('"')){
				while (true){
					switch(peek()){
						case '\n':
							line++;
						case ' ':
						case '\r':
						case '\t':
							deprecate();
							continue;
					}
					break;
				}
				current--;
				deprecate();
				if (peek() == '"'){
					deprecate();
					continue;
				}
				break;
			}
			str += advance();
		}

		// Unterminated string.
		if (is_at_end()) {
			Core::error(line, "Expected string termination.");
			return;
		}
		add_token(Tok::l_string, Object(Trait("string"),QVariant(str)));
	}

	void Tokenizer::character(){
		current--;
		deprecate();
		char c = '\0';
		if (is_at_end()){
			Core::error(line,"Expected a character.");
			return;
		}
		if (peek() == '\\'){
			deprecate();
			QChar ch = escaped.value(QChar(peek()),QChar('*'));
			if (ch != QChar('*')){
				source[current] = ch;
			}
			else{
				Core::error(line,"Undefined escape sequence.");
			}
		}
		c = advance();
		if (peek() != '\''){
			Core::error(line,"Expected a closing single quote.");
		}
		else
			deprecate();

		add_token(Tok::l_char,Object(c));
	}

	bool Tokenizer::is_digit(char ch){
#ifdef RND_NAME//__GNUC__
		return std::isdigit(t_cast<unsigned char>(ch));
#else
		return ch >= '0' && ch <= '9';
#endif

	}

	void Tokenizer::number() {
		LFn;
		TString num = "";
		num.push_back(peek(-1)); // Retrieves the digit being advanced.
		current--;
		deprecate();

		//base prediction
		int base = 10;
		if (num[0] == "0" && peek() != '.'){
			base = 8;
			if (match('x') || match('X')){
				current--;
				deprecate();
				num.push_back('x');
				base = 16;
			}
			else if (match('b') || match('B')){
				current--;
				deprecate();
				num.push_back('b');
				base = 2;
			}
		}


		while ((base == 10 && is_digit  (peek())) ||
			   (base == 16 && is_base_16(peek())) ||
			   (base ==  8 && is_base_8 (peek())) ||
			   (base ==  2 && peek() < 2)){
			num.push_back(QChar(peek()));
			deprecate();
		}

		// Look for a fractional part.
		if (peek() == '.' && is_digit(peek(1)) && base == 10) {
			// Consume the "."
			num.push_back(QChar(peek()));
			deprecate();
			while (is_digit(peek())) {
				num.push_back(QChar(peek()));
				deprecate();
			}

			add_token(Tok::l_real,
					  Object(Trait("real"),num.toDouble()));
			LVd;
		}
		bool stat = false;
		int n = num.toInt(&stat,base);
		add_token(Tok::l_real,
				  Object(Trait("real"),n));
		Log ls("  Numeral: String:") ls(num);
		Log ls("           Number:") ls(n);
		if (!stat){
			Core::error(line, TString("Invalid base-").append(TString::number(base)).append(" numeral."));
		}
		LVd;
	}

	bool Tokenizer::is_alpha(char c) {
		return	(c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') ||
				c == '_';
	}

	bool Tokenizer::is_alpha_numeric(char c){
		return is_alpha(c) || is_digit(c);
	}

	bool Tokenizer::is_base_16(char c){
		return (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || is_digit(c);
	}

	bool Tokenizer::is_base_8(char c){
		return c >= '0' && c <= '7';
	}

	void Tokenizer::identifier(){
		LFn;
		current--;
		TString val = "";
		val.push_back(peek());
		if ((val == "R" || val == "r") && peek(1) == '"' && peek(2) == '['){
			deprecate();
			deprecate();
			deprecate();
			raw_string();
			LVd;
		}
		advance();
		while (is_alpha_numeric(peek()))
			val.push_back(advance());
		Tokty ty = keywords.value(val, Tok::identifier);
		tokens.push_back(Token(ty,val,Object(),line));
		LVd;
	}

	void Tokenizer::raw_string(){
		//Assume R"[ was eaten as in Tokenizer::identifier()
		Core::error(0,"Raw string literal not supported.");
	}

	void Tokenizer::scan_token(){
		LFn;
		char c = advance();
		Logp(c);
		switch (c) {
			case '@': add_token(Tok::at_symbol); break;
			case '(': add_token(Tok::l_paren); break;
			case ')': add_token(Tok::r_paren); break;
			case '{': add_token(Tok::l_brace); break;
			case '}': add_token(Tok::r_brace); break;
			case '[': add_token(Tok::l_sq_brkt); break;
			case ']': add_token(Tok::r_sq_brkt); break;
			case ',': add_token(Tok::comma); break;
			case '.': add_token(Tok::dot); break;
			case ';': add_token(Tok::semi_colon); break;
			case '?': add_token(Tok::ques_mk); break;
			case '$': add_token(Tok::dollar); break;
			case '*': add_token(match('=')?Tok::mult_asgn:
										   Tok::star); break;
			case '^': add_token(match('=')?Tok::exp_asgn:
										   Tok::caret); break;
			case '%': add_token(match('=')?Tok::mod_asgn:
										   Tok::mod); break;
			case '-': add_token(match('>')?Tok::arrow:
										   match('-')?Tok::decr:
													  match('=')?Tok::minus_asgn:
																 Tok::minus); break;
			case '+': add_token(match('+')?Tok::incr:
										   match('=')?Tok::plus_asgn:
													  Tok::plus); break;
			case '&': add_token(match('&')?Tok::amp_amp:
										   match('=')?Tok::amp_asgn:
													  Tok::amp); break;
			case ':': add_token(match(':')?Tok::scope:
										   Tok::colon); break;
			case '|': add_token(match('|')?Tok::vert_vert:
										   match('=')?Tok::vert_asgn:
													  Tok::vert); break;
			case '>': add_token(match('=')?Tok::greater_equal:
										   Tok::greater); break;
			case '<': add_token(match('=')?Tok::less_equal:
										   Tok::less); break;
			case '=': add_token(match('=')?Tok::equal:
										   match('>')?Tok::fat_arrow:
													  Tok::assign); break;
			case '!': add_token(match('=')?Tok::bang_equal:
										   Tok::bang); break;
			case '/':
				if (match('/')) {
					// A comment goes until the end of the line.
					while (peek() != '\n' && !is_at_end()) advance();
				}
				else if (match('*')){
					char c = '\0';
					while ((c = peek())){
						if (c == '\0'){
							Core::error(line,"Unterminated multi-line comment.");
							return;
						}
						else if (c == '\n')
							line++;
						else if (c == '*'){
							if (peek(1) == '/'){
								advance();
								advance();
								break;
							}
						}
						advance();
					}

				}
				else {
					if (match('='))
						add_token(Tok::div_asgn);
					else
						add_token(Tok::slash);
				}
				break;
			case '\n':
				line++;
			case ' ':
			case '\r':
			case '\t':
				// Ignore whitespace.
				break;
			case '"':
				string();
				break;
			case '\'':
				character();
				break;
			default:
				if (is_digit(c))
					number();
				else if (is_alpha(c))
					identifier();
				else
					Core::error(line, "Unexpected character.");
		}
		LVd;
	}

	Tokens Tokenizer::scan_tokens(){
		LFn;
		line = current = start = 0;
		tokens.clear();
		if(source.isEmpty())
			LRet tokens;
		while (!is_at_end()){
			start = current;
			scan_token();
		}
		start = current = 0;
		add_token(Tok::eof);
		LRet tokens;
	}

	QHash<TString, Tok::tok_type> Tokenizer::keywords{
		{"struct"	,	Tok::k_struct	},
		{"for"		,	Tok::k_for		},
		{"if"		,	Tok::k_if		},
		{"else"		,	Tok::k_else		},
		{"return"	,	Tok::k_return	},
		{"this"		,	Tok::k_this		},
		{"true"		,	Tok::l_true		},
		{"false"	,	Tok::l_false	},
		{"do"		,	Tok::k_do		},
		{"while"	,	Tok::k_while	},
		{"case"		,	Tok::k_case		},
		{"switch"	,	Tok::k_switch	},
		{"break"	,	Tok::k_break	},
		{"default"	,	Tok::k_default	},
		{"enum"		,	Tok::k_enum		},
		{"assert"	,	Tok::k_assert	},
		{"define"	,	Tok::k_define	},
		{"array"	,	Tok::k_array	},
		{"set"		,	Tok::k_set		},
		{"var"		,	Tok::k_var		},
		{"null"		,	Tok::l_null		},
		{"print"	,	Tok::k_print	},
		{"println"	,	Tok::k_println	},
		{"fn"		,	Tok::k_fn		},
		{"match"	,	Tok::k_match	},
		{"matches"	,	Tok::k_matches	},
		{"char"		,	Tok::t_char		},
		{"string"	,	Tok::t_string	},
		{"real"		,	Tok::t_real		},
		{"bool"		,	Tok::t_bool		},
		{"run"		,	Tok::k_run		}
	};

	QHash<QChar,QChar> Tokenizer::escaped{
		{'0'	,	'\0'},
		{'"'	,	'"'	},
		{'n'	,	'\n'},
		{'t'	,	'\t'},
		{'b'	,	'\b'},
		{'v'	,	'\v'},
		{'\\'	,	'\\'},
		{'\''	,	'\''}
	};
}
