
#include "tokenizer.h"
#include "tlogger.h"
Tokenizer::Tokenizer(const TString& src):source(src)
{
#if T_DBG
	Log << "Source: " << source;
	TLogHelper::indent();
#endif
}
Tokenizer::~Tokenizer(){
#if T_DBG
	TLogHelper::outdent();
	Log << "~Tokenizer()";
#endif
}

bool Tokenizer::is_at_end(){
	return current >= source.size();
}

char Tokenizer::advance(){
#if T_DBG
	Log << "  adv:" << TString::number(current);
	Log << "      " << peek();
#endif
	return source[current++].toLatin1();
}

void Tokenizer::add_token(Tok ty){
	add_token(ty, Object());
}

void Tokenizer::add_token(Tok ty, const Object& lit){
	LFn;
	tokens.append(Token(ty,source.mid(start,current-start),lit,line));
#if T_DBG
	Log << "Added Token: Lexeme:" << tokens.last().lexeme;
	Log << "             Type  :" << (int)tokens.last().ty;
#endif
	LVd;
}

char Tokenizer::peek(int i) {
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
			MereMath::error(line, "Expected a closing quotation mark to terminate string.");
			return;
		}
		if (peek() == '\\'){
			QChar ch = escaped.value(QChar(peek(1)),QChar('*'));
			if (ch == QChar('*')){
				MereMath::error(line,"Undefined escape sequence.");
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
		MereMath::error(line, "Expected string termination.");
		return;
	}
	add_token(Tok::_string, Object(Trait("string"),QVariant(str)));
}

void Tokenizer::character(){
	current--;
	deprecate();
	char c = '\0';
	if (is_at_end()){
		MereMath::error(line,"Expected a character.");
		return;
	}
	if (peek() == '\\'){
		deprecate();
		QChar ch = escaped.value(QChar(peek()),QChar('*'));
		if (ch != QChar('*')){
			source[current] = ch;
		}
		else{
			MereMath::error(line,"Undefined escape sequence.");
		}
	}
	c = advance();
	if (peek() != '\''){
		MereMath::error(line,"Expected a closing single quote.");
	}
	else
		deprecate();

	add_token(Tok::_char,Object(c));
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
	num.append(peek(-1)); // Retrieves the digit being advanced.
	current--;
	deprecate();

	//base prediction
	int base = 10;
	if (num[0] == "0" && peek() != '.'){
		base = 8;
		if (match('x') || match('X')){
			current--;
			deprecate();
			num.append('x');
			base = 16;
		}
		else if (match('b') || match('B')){
			current--;
			deprecate();
			num.append('b');
			base = 2;
		}
	}


	while ((base == 10 && is_digit  (peek())) ||
		   (base == 16 && is_base_16(peek())) ||
		   (base ==  8 && is_base_8 (peek())) ||
		   (base ==  2 && peek() < 2)){
		num.append(QChar(peek()));
		deprecate();
	}

	// Look for a fractional part.
	if (peek() == '.' && is_digit(peek(1)) && base == 10) {
		// Consume the "."
		num.append(QChar(peek()));
		deprecate();
		while (is_digit(peek())) {
			num.append(QChar(peek()));
			deprecate();
		}

		add_token(Tok::_real,
				 Object(Trait("real"),num.toDouble()));
		LVd;
		return;
	}
	bool stat = false;
	int n = num.toInt(&stat,base);
	add_token(Tok::_real,
			 Object(Trait("real"),n));
#if T_DBG
	Log << "  Numeral: String:" << num;
	Log << "           Number:" << n;
#endif
	if (!stat){
		MereMath::error(line, TString("Invalid base-").append(TString::number(base)).append(" numeral."));
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
	val.append(peek());
	if ((val == "R" || val == "r") && peek(1) == '"' && peek(2) == '['){
		deprecate();
		deprecate();
		deprecate();
		raw_string();
		LVd;
		return;
	}
	advance();
	while (is_alpha_numeric(peek()))
		val.append(advance());
	Tok ty = keywords.value(val, Tok::identifier);
	tokens.append(Token(ty,val,Object(),line));
	LVd;
}

void Tokenizer::raw_string(){
	//Assume R"[ was eaten as in Tokenizer::identifier()
	MereMath::error(0,"Raw string literal not supported.");
}

void Tokenizer::scan_token(){
	LFn;
	char c = advance();
#if T_DBG
	Log << c;
#endif
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
						MereMath::error(line,"Unterminated multi-line comment.");
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
				MereMath::error(line, "Unexpected character.");
	}
	LVd;
}

Tokens Tokenizer::scan_tokens(){
	line = current = start = 0;
	tokens.clear();
	while (!is_at_end()){
		start = current;
		scan_token();
	}
	start = current = 0;
	add_token(Tok::_eof_);
	return tokens;
}

QHash<TString, Tok> Tokenizer::keywords{
	{"struct"	,	Tok::_struct	},
	{"for"		,	Tok::_for		},
	{"if"		,	Tok::_if		},
	{"else"		,	Tok::_else		},
	{"return"	,	Tok::_return	},
	{"this"		,	Tok::_this		},
	{"true"		,	Tok::_true		},
	{"false"	,	Tok::_false		},
	{"do"		,	Tok::_do		},
	{"while"	,	Tok::_while		},
	{"case"		,	Tok::_case		},
	{"switch"	,	Tok::_switch	},
	{"break"	,	Tok::_break		},
	{"default"	,	Tok::_default	},
	{"enum"		,	Tok::_enum		},
	{"assert"	,	Tok::_assert	},
	{"define"	,	Tok::_define	},
	{"array"	,	Tok::_array		},
	{"set"		,	Tok::_set		},
	{"var"		,	Tok::_var		},
	{"null"		,	Tok::_null		},
	{"print"	,	Tok::_print		},
	{"println"	,	Tok::_println	},
	{"fn"		,	Tok::_fn		},
	{"match"	,	Tok::_match		},
	{"matches"	,	Tok::_matches	}
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
