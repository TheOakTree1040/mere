#include "sourceeditor.hpp"
#include "cmd.h"

int main(int argc, char *argv[]) {
	App a(argc, argv);

	MereCmder cmder;
#if T_GUI
	int r = cmder.execute()?(a.quit(),a.exec()):EXIT_SUCCESS;
#if T_DBG
	Log << "Exited main event loop";
#endif //T_DBG
#else
	cmder.execute();
	//a.quit();

	int r = EXIT_SUCCESS;
#endif // T_UI_Conf == T_UI_GUI
	return r;
}
