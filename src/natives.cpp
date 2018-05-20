#include "natives.h"
#include "interpreter.h"

void mere::check_param(const TString& callee, std::vector<TString>&& expect,std::vector<Object>& received_obj){
	LFn;
	int size = expect.size();
	if (size != received_obj.size()){
		LThw ArityMismatchError(size,received_obj.size(),callee);
	}
	std::vector<TString> received(received_obj.size());
	for (int i = 0; i != received.size(); i++){
		received[i] = received_obj[i].trait().id();
	}
	for (int i = 0; i != size; i++){
		if (expect[i] != received[i]){
			LThw ArgumentError(expect[i],received[i],callee);
		}
	}
}
