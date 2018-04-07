#ifndef PARSEERROR_H
#define PARSEERROR_H
#include <QException>
class ParseError : public QException{
	public:
		void raise() const{
			throw *this;
		}
		ParseError* clone() const{
			return new ParseError(*this);
		}
};
class ParseUnwind : public QException{
	public:
		void raise() const{
			throw *this;
		}
		ParseUnwind* clone() const{
			return new ParseUnwind(*this);
		}
};

#endif // PARSEERROR_H
