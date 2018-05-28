#pragma once

#ifndef SRCLOCATION_H
#define SRCLOCATION_H

namespace mere{
	struct srcloc_t final {
		unsigned int line = 1u;
		unsigned int col = 1u;

		srcloc_t(unsigned int l, unsigned int c):
			line(l),
			col(c){}
		srcloc_t(){}
	};
}



#endif // SRCLOCATION_H
