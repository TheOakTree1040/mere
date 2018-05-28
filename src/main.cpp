
#define NO_TEST 1

#if NO_TEST
#include "shell.h"

int main(int argc, char* argv[]){
	App a(argc, argv);
	return mere::MereCmder().execute();
}
#else
#include "shell.h"
#include <iostream>

int main(int argc, char *argv[]) {
	using namespace mere;

	IntpUnit unit = new InterpretationUnit(
							"/Users/TheOakTree/mere/mere-interpreter/tests/test.mr"
						);
	if (unit->success()){
		Interpreter intp;
		intp.interpret(unit);
	}
	unit->print_issues();
	delete unit;
	return 0;
}
#endif
