#include "natives.h"
#include "interpreter.h"
#include "runtimeerror.h"

void mere::check_param(const TString& callee, std::vector<TString>&& expect,std::vector<Object>& received_obj){
	LFn;
	uint size = expect.size();
	if (size != received_obj.size()){
		LThw ArityMismatchError(size,received_obj.size(),callee);
	}
	std::vector<TString> received(received_obj.size());
	for (uint i = 0u; i != received.size(); i++){
		received[i] = received_obj[i].trait().id();
	}
	for (uint i = 0u; i != size; i++){
		if (expect[i] != received[i]){
			LThw ArgumentError(expect[i],received[i],callee);
		}
	}
}
