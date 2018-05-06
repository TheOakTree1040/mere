#pragma once
#ifndef NATIVES_H
#define NATIVES_H
#define CHECK(CALLEE,EXPECT) t::check_param(CALLEE,EXPECT,arguments)
#include "merecallable.h"
namespace t{
	void check_param(const TString&, QVector<TString>&&, QVector<Object>&);
}

#endif // NATIVES_H
