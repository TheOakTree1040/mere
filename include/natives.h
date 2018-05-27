
#pragma once

#pragma once
#ifndef NATIVES_H
#define NATIVES_H

#include "merecallable.h"

#define CHECK(CALLEE,EXPECT) check_param(CALLEE,EXPECT,arguments)

namespace mere{
	void check_param(const QString&, const std::vector<QString>&, const std::vector<Object>&);
}

#endif // NATIVES_H
