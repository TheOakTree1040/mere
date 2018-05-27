#include "tokenizer.h"
#include "parser.h"
#include <iostream>
#define NO_TEST 0
#if NO_TEST
#include "shell.h"
int entry(int argc, char* argv[]){
	App a(argc, argv);

	mere::MereCmder cmder;
#if T_GUI
	int r = cmder.execute()?(a.quit(),a.exec()):EXIT_SUCCESS;
	Log1("Exited main event loop");
#else
	cmder.execute();
	//a.quit();

	int r = EXIT_SUCCESS;
#endif // T_UI_Conf == T_UI_GUI
	return r;
}
#endif
int main(int argc, char *argv[]) {
#if NO_TEST
	return entry(argc,argv);
#else
	using namespace mere;
	QString test =
R"(var a = 1+1;
var b;
c=>a;)";
	Tokens toks = Tokenizer(test).scan_tokens();
	std::cout << toks.to_string("\n").toStdString();
	std::cout << "\n\n";
	Stmts stmt = Parser(toks).parse();
	Core::show_errors();
	return !stmt.size();
#endif
}
