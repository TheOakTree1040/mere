#include "sourceeditor.hpp"
#include "cmd.h"
int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MereCmder cmder;
    int r = cmder.execute()?a.exec():0;
#if _DEBUG
	Log << "Exited main event loop";
#endif //_DEBUG
	return r;
}
