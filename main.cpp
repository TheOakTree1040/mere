#include "sourceeditor.hpp"
#include "cmd.h"
int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MereCmder cmder;
    int r = cmder.execute()?a.exec():0;
	Log << "Exited main event loop";
	return r;
}
