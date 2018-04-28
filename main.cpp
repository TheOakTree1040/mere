#include "sourceeditor.hpp"
#include "cmd.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MereCmder().execute();
	int ret = a.exec();
	MereCmder::clean_up();
	return ret;
}
