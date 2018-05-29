
#define TEST 0

#if !TEST
#include "shell.h"

int main(int argc, char* argv[]){
	App a(argc, argv);
	return mere::MereCmder().execute();
}
#else
#include "interpreter.h"
#include <iostream>

int main(int argc, char *argv[]) {
	using namespace mere;

	InterpretationUnit* unit = new InterpretationUnit(
							"/Users/TheOakTree/mere/mere-interpreter/tests/test.mr"
						);
	unit->print_tokens();
	delete unit;
	return 0;
}
#endif
