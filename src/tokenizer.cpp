
#include "tokenizer.h"
#include "tlogger.h"

#define NEWLINE()\
{loc.line++; loc.col = 1; unit->add_index(current);}
#define MOVE_BACK() {loc.col--;current--;}

namespace mere {
	Tokenizer::Tokenizer(IntpUnit unit):
		unit(unit),
		source(unit->source()),
		tokens(unit->tokens()){
		Log ls("Source: ") ls(source);
		LIndt;
	}
	Tokenizer::~Tokenizer(){
		LOdt;
		Logp("~Tokenizer()");
	}

	void Tokenizer::error(const QString& errmsg) {
		unit->report(loc, "lex-error", errmsg);
	}

	bool Tokenizer::is_at_end() const {
		return current >= source.size() || source.isEmpty();
	}

	char Tokenizer::advance(){
		Log ls("  adv:") ls(QString::number(current));
		Log ls("      ") ls(peek());
		loc.col++;
		return source[current++].toLatin1();
	}

	void Tokenizer::add_token(Tokty ty){
		add_token(ty, Object());
	}

	void Tokenizer::add_token(Tokty ty, const Object& lit){
		LFn;
		tokens.push_back(Token(ty,source.mid(start,current-start),lit,start_loc));
		Log ls("Added Token: Lexeme:") ls(tokens[tokens.size()-1].lexeme());
		Log ls("             Type  :") ls((int)tokens[tokens.size()-1].type());
		LVd;
	}

	char Tokenizer::peek(bool ahead) const {
		if (is_at_end() || (ahead && (current + 1 >= source.size())))
			return '\0';
		return (ahead?source[current+1]:source[current]).toLatin1();
	}

	bool Tokenizer::match(char expect) {
		return (!is_at_end() && source[current] == expect)?(advance(), true):false;
	}

	void Tokenizer::string(){
		QString str = "";
		bool terminated = false;
		while (!is_at_end()) {
			if (peek() == '\n') {
				error("expected a closing quotation mark to terminate string.");
				return;
			}
			else if (match('\\')){
				QChar ch = escaped.value(peek(),'\0');
				if (ch == '\0'){
					error("undefined escape sequence.");
				}
				str += ch;
				advance();
				continue;
			}
			else if (match('"')){
				// To support concatenation of string literals w/o "+"
				while (true){
					switch(peek()){
						case '\n':
							advance();
							NEWLINE(); continue;
						case ' ':
						case '\r':
						case '\t':
							advance();
							continue;
						default:;
					}
					break;
				}
				//If a quote is found, lex another string lit.
				if (match('"')){
					continue;
				}
				//If no quote is found, break
				terminated = true;
				break;
			}
			str += advance();
		}

		// Unterminated string.
		if (is_at_end() && !terminated) {
			error("expected string termination");
			return;
		}
		add_token(Tok::l_string, Object(Trait("string"),QVariant(str)));
	}

	void Tokenizer::character(){
		char c = '\0';
		if (is_at_end()){
			error("expected a character");
			return;
		}
		if (peek() == '\\'){
			advance();
			auto ch = escaped.value(peek(),'\0');
			if (ch != '\0')
				c = ch.toLatin1();
			else
				error("undefined escape sequence");
		}
		else
			c = advance();
		if (peek() != '\''){
			error("expected a `'`");
		}
		else
			advance();

		add_token(Tok::l_char,Object(c));
	}

	bool Tokenizer::is_digit(char ch){
		return ((uint32_t)ch - '0') < 10u; //optimized is_digit
	}

	void Tokenizer::number() {
		LFn;
		QString num = "";
		MOVE_BACK();
		num.push_back(advance()); // Retrieves the digit advanced.

		//base prediction
		int base = 10;
		if (num[0] == QLatin1Char('0') && peek() != '.'){
			if (match('x') || match('X')){
				num.push_back('x');
				base = 16;
			}
			else if (match('b') || match('B')){
				num.push_back('b');
				base = 2;
			}
			else {
				base = 8;
			}
		}


		while ((base == 10 && is_digit  (peek())) ||
			   (base == 16 && is_base_16(peek())) ||
			   (base ==  8 && is_base_8 (peek())) ||
			   (base ==  2 && (peek() == '0' || peek() == '1'))){
			num.push_back(advance());
		}

		// Look for a fractional part.
		if (peek() == '.' && is_digit(peek(true)) && base == 10) {
			// Consume the "."
			num.push_back(advance());
			while (is_digit(peek())) {
				num.push_back(advance());
			}

			add_token(Tok::l_real,
					  Object(Trait("real"),num.toDouble()));//assumes base-10 (decimal)
			LVd;
		}
		bool stat = false;
		int n = num.toInt(&stat,base);
		add_token(Tok::l_real,
				  Object(Trait("real"),n));
		Log ls("  Numeral: String:") ls(num);
		Log ls("           Number:") ls(n);
		if (!stat){
			error(QString("invalid base-%1 numeral").arg(QString::number(base)));
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
		MOVE_BACK();
		QString val = "";
		val.push_back(peek());
		if ((val == QLatin1String("R")
			 || val == QLatin1String("r")
			 )
			&& peek(true) == '"'
			&& peek(2) == '['){
			advance();
			advance();
			advance();
			raw_string();
			LVd;
		}
		advance();
		while (is_alpha_numeric(peek()))
			val.push_back(advance());
		Tokty ty = keywords.value(val, Tok::identifier);
		tokens.push_back(Token(ty,val,Object(),start_loc));
		LVd;
	}

	void Tokenizer::raw_string(){
		//Assume R"[ was eaten as in Tokenizer::identifier()
		error("raw string literal not supported");
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
						if (c == '\n'){
							NEWLINE(); //leave this block alone -- if () {stmts}; else if ...
						}
						else if (c == '*'){
							if (peek(true) == '/'){
								advance();
								advance();
								break;
							}
						} else if (c == '\0'){
							error("unterminated multi-line comment");
							return;
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
				NEWLINE();
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
					error("unexpected character");
		}
		LVd;
	}

	void Tokenizer::scan_tokens(){
		LFn;
		if(source.isEmpty() || !unit->success()){
			LVd;
		}
		while (!is_at_end()){
			start = current;
			start_loc = loc;
			scan_token();
		}
		tokens.push_back(Token(Tok::done," ",Object(),loc));
		tokens.push_back(Token(Tok::eof,"",Object(),loc));
		unit->add_index(source.size()); // eof line index
		LVd;
	}

	const QHash<QString, Tok::tok_type> Tokenizer::keywords{
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

	const QHash<QChar,QChar> Tokenizer::escaped{
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
