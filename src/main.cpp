#include "shell.h"
#define NO_TEST 1
#if NO_TEST
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
	QFile file ("/Users/TheOakTree/mere/mere-interpreter/tests/test.mr");
	file.open(QIODevice::ReadOnly);
	Core::run(file);
	return 0;
#endif
}
