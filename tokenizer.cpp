
#include "tokenizer.h"
#include "logger.h"
Tokenizer::Tokenizer(const QString& src):source(src)
{
	Logger() << "Source: " << source;
	Logger::indent++;
}
Tokenizer::~Tokenizer(){
	Logger::indent--;
	Logger() << "~Tokenizer()";
}

bool Tokenizer::is_at_end(){
	return current >= source.size();
}

char Tokenizer::advance(){
	Log << "  adv:" << QString::number(current++) << peek(-1);
	return source[current-1].toLatin1();
}

void Tokenizer::add_token(Tok ty){
	add_token(ty, Object());
}

void Tokenizer::add_token(Tok ty, const Object& lit){
	LFn;
	tokens.append(Token(ty,source.mid(start,current-start+1),lit,line));
	Log << "Added Token: Lexeme:" << tokens.last().lexeme;
	Log << "             Type  :" << (int)tokens.last().ty;
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
	return !is_at_end() && source[current] == expct?
				current++, Logger() << "  matched " << expct, true:
									   false;
}

bool Tokenizer::match(QString expct){
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
	QString str = "";
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
	add_token(Tok::STRING, Object(Trait("string"),QVariant(str)));
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
			deprecate();
			source[current] = ch;
		}
		else{
			MereMath::error(line,"Undefined escape sequence.");
		}
	}
	c = advance();
	deprecate();

	add_token(Tok::CHAR,Object(c));
}

bool Tokenizer::is_digit(char ch){
	return std::isdigit(static_cast<unsigned char>(ch));
}

void Tokenizer::number() {
	LFn;
	QString num = "";
	num.append(peek(-1)); // Retrieves the digit being advanced.
	current--;
	deprecate();
	int base = 10;
	if (num[0] == "0" && peek() != '.'){
		base = 8;
		if (match('x') || match('X')){
			current--;
			deprecate();
			num.append('x');
			base = 16;
		}
		else if (match('b') || match('b')){
			current--;
			deprecate();
			num.append('b');
			base = 2;
		}
	}

	while ((base == 10 && is_digit  (peek())) ||
		   (base == 16 && is_base_16(peek())) ||
		   (base ==  8 && is_base_8 (peek())) ||
		   (base ==  0 && peek() < 2)){
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

		add_token(Tok::REAL,
				 Object(Trait("real"),num.toDouble()));
		LVd;
	}
	bool stat = false;
	int n = num.toInt(&stat,base);
	Log << "Object constructing (real)";
	add_token(Tok::REAL,
			 Object(Trait("real"),num.toInt(&stat,base)));
	Log << "  Numeral: String:" << num;
	Log << "           Number:" << n;
	if (!stat){
		MereMath::error(line, QString("Invalid base-").append(QString::number(base)).append(" numeral."));
		Log << "Numeral Lexing Error.";
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
	QString val = "";
	val.append(peek());
	if ((val == "R" || val == "r") && peek(1) == '"' && peek(2) == '['){
		deprecate();
		deprecate();
		deprecate();
		raw_string();
		Logger() << "End of identifier()";
		Logger::indent--;
		return;
	}
	advance();
	while (is_alpha_numeric(peek()))
		val.append(advance());
	Tok ty = keywords.value(val, Tok::IDENTIFIER);
	tokens.append(Token(ty,val,Object(),line));
	Log << "Added Token: Lexeme:" << tokens.last().lexeme;
	Log << "             Type  :" << (int)tokens.last().ty;
	Log << val;
	Log << "Done.";
	LVoid
}

void Tokenizer::raw_string(){
	//Assume R"[ was eaten as in Tokenizer::identifier()
}

void Tokenizer::scan_token(){
	char c = advance();
	Log << c;
	switch (c) {
		case '@': add_token(Tok::AT_SYMB); break;
		case '(': add_token(Tok::LPAREN); break;
		case ')': add_token(Tok::RPAREN); break;
		case '{': add_token(Tok::LBRACE); break;
		case '}': add_token(Tok::RBRACE); break;
		case '[': add_token(Tok::LSQBRKT); break;
		case ']': add_token(Tok::RSQBRKT); break;
		case ',': add_token(Tok::COMMA); break;
		case '.': add_token(Tok::DOT); break;
		case ';': add_token(Tok::SCOLON); break;
		case '?': add_token(Tok::QUES_MK); break;
		case '$': add_token(Tok::DOLLAR); break;
		case '*': add_token(match('=')?Tok::MULT_ASGN:
									   match("*=")?Tok::EXP_ASGN:
												   Tok::STAR); break;
		case '^': add_token(match('=')?Tok::EXP_ASGN:
									   Tok::CARET); break;
		case '%': add_token(match('=')?Tok::MOD_ASGN:
									   Tok::MOD); break;
		case '-': add_token(match('>')?Tok::ARROW:
									   match('-')?Tok::DECR:
												  match('=')?Tok::MINUS_ASGN:
															 Tok::MINUS); break;
		case '+': add_token(match('+')?Tok::INCR:
									   match('=')?Tok::PLUS_ASGN:
												  Tok::PLUS); break;
		case '&': add_token(match('&')?Tok::AMPAMP:
									   match('=')?Tok::AMP_ASGN:
												  Tok::AMP); break;
		case ':': add_token(match(':')?Tok::SCOPE:
									   Tok::COLON); break;
		case '|': add_token(match('|')?Tok::VERTVERT:
									   match('=')?Tok::VERT_ASGN:
												  Tok::VERT); break;
		case '>': add_token(match('=')?Tok::GREATER_EQUAL:
									   Tok::GREATER); break;
		case '<': add_token(match('=')?Tok::LESS_EQUAL:
									   Tok::LESS); break;
		case '=': add_token(match('=')?Tok::EQUAL:
									   match('>')?Tok::FAT_ARROW:
												  Tok::ASSIGN); break;
		case '!': add_token(match('=')?Tok::N_EQUAL:
									   Tok::EXCL); break;
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
					add_token(Tok::DIV_ASGN);
				else
					add_token(Tok::SLASH);
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
}

Tokens Tokenizer::scan_tokens(){
	line = current = start = 0;
	tokens.clear();
	while (!is_at_end()){
		start = current;
		scan_token();
	}
	start = current = 0;
	add_token(Tok::END);
	return tokens;
}

QHash<QString, Tok> Tokenizer::keywords{
	{"struct"	,	Tok::STRUCT		},
	{"for"		,	Tok::FOR		},
	{"if"		,	Tok::IF			},
	{"else"		,	Tok::ELSE		},
	{"return"	,	Tok::RETURN		},
	{"this"		,	Tok::THIS		},
	{"true"		,	Tok::TRUE		},
	{"false"	,	Tok::FALSE		},
	{"do"		,	Tok::DO			},
	{"while"	,	Tok::WHILE		},
	{"case"		,	Tok::CASE		},
	{"switch"	,	Tok::SWITCH		},
	{"break"	,	Tok::BREAK		},
	{"default"	,	Tok::DEFAULT	},
	{"enum"		,	Tok::ENUM		},
	{"assert"	,	Tok::ASSERT		},
	{"define"	,	Tok::DEFINE		},
	{"array"	,	Tok::ARRAY		},
	{"set"		,	Tok::SET		},
	{"valued"	,	Tok::VALUED		},
	{"global"	,	Tok::GLOBAL		},
	{"var"		,	Tok::VAR		},
	{"null"		,	Tok::NULL_LIT	},
	{"print"	,	Tok::PRINT		}
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
