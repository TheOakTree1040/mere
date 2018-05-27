#pragma once

#ifndef PARSEERROR_H
#define PARSEERROR_H

#include <stdexcept>

namespace mere {
	class ParseError : public std::runtime_error{
		public:
			ParseError():std::runtime_error("ParseError"){}
	};
}

#endif // PARSEERROR_H
