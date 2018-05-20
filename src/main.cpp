#include "shell.h"

int main(int argc, char *argv[]) {
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
