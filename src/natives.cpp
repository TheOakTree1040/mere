#include "natives.h"
#include "interpreter.h"
#include "runtimeerror.h"

void mere::check_param(const QString& callee, const std::vector<QString>& expect,
					   const std::vector<Object>& received_obj){
	LFn;
	uint size = expect.size();
	if (size != received_obj.size()){
		LThw ArityMismatchError(size,received_obj.size(),callee);
	}
	for (uint i = 0u; i != size; i++){
		if (expect[i] != received_obj[i].trait().id()){
			LThw ArgumentError(expect[i],received_obj[i].trait().id(),callee);
		}
	}
}
