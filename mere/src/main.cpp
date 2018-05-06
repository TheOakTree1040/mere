#include "sourceeditor.hpp"
#include "cmd.h"

int main(int argc, char *argv[]) {
	App a(argc, argv);

	MereCmder cmder;
#if T_UI_Conf == T_UI_GUI
	int r = cmder.execute()?a.exec():EXIT_SUCCESS;
#if _DEBUG
	Log << "Exited main event loop";
#endif //_DEBUG
#else
	cmder.execute();
	//a.quit();

	int r = EXIT_SUCCESS;
#endif // T_UI_Conf == T_UI_GUI
	return r;
}
