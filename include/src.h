#pragma once

#ifndef SRCLOCATION_H
#define SRCLOCATION_H

#include <QString>
#include <vector>

namespace mere{
	struct srcloc_t {
		unsigned int line = 1u;
		unsigned int col = 1u;

		srcloc_t(unsigned int l, unsigned int c):
			line(l),
			col(c){}
		srcloc_t(){}
	};
	struct errloc_t : public srcloc_t {
		unsigned int line_start = 0u;

		errloc_t(const srcloc_t& s, unsigned int ls):
			srcloc_t(s),
			line_start(ls){}
		errloc_t(unsigned int l, unsigned int c, unsigned int ls):
			srcloc_t(l,c),
			line_start(ls){}
		errloc_t(){}
	};
}



#endif // SRCLOCATION_H
