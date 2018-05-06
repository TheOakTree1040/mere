#ifndef PARSEERROR_H
#define PARSEERROR_H
#include <stdexcept>
class ParseError : public std::runtime_error{
	public:
		ParseError():std::runtime_error("ParseError"){}
};
class ParseUnwind : public std::logic_error{
	public:
		ParseUnwind():std::logic_error("ParseUnwind"){}
};

#endif // PARSEERROR_H
