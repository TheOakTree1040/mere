#pragma once
#ifndef NATIVES_H
#define NATIVES_H
#define CHECK(CALLEE,EXPECT) check_param(CALLEE,EXPECT,arguments)
#include "merecallable.h"
namespace mere{
	void check_param(const TString&, std::vector<TString>&&, std::vector<Object>&);
}

#endif // NATIVES_H
