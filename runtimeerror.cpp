#include "runtimeerror.h"
const QHash<int,TString> Abort::abort_codes{
	{0x01	,	"Assertion Failed"	},
	{0x02	,	"Division by 0"	}
};
