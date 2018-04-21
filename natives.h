#pragma once
#ifndef NATIVES_H
#define NATIVES_H
#include "merecallable.h"
namespace t{
	static void check_param(const TString&, QVector<TString>&&, QVector<Object>&);
	extern MereCallable clock;
	extern MereCallable time;
	//extern MereCallable XOR;
	extern MereCallable sin;
	extern MereCallable cos;
	extern MereCallable tan;
	void _init();
	//extern MereCallable cryptic_hash;
}

#endif // NATIVES_H
