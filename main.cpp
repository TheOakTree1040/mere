#include "sourceeditor.h"
#include "cmd.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MereCmdExec::execute();

	SourceEditor* editor = new SourceEditor();
	editor->setAttribute(Qt::WA_DeleteOnClose);
	editor->show();
	int a_ret = a.exec();
	delete MereMath::interpreter;
	return a_ret;
}
