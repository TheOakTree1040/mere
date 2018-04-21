
#include "sourceeditor.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setApplicationName("MereMath Interpreter");
	MereMath::init_once();
	SourceEditor* editor = new SourceEditor();
	editor->setAttribute(Qt::WA_DeleteOnClose);
	editor->show();
	int a_ret = a.exec();
	delete MereMath::interpreter;
	return a_ret;
}
