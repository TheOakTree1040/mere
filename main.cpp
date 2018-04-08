#include "sourceeditor.h"
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setApplicationName("MereMath Interpreter");
	if (!QMetaType::registerComparators<Object>()){
		qDebug() << "Failed registering comparators.";
		return a.exec();
	}
	SourceEditor* editor = new SourceEditor();
	editor->show();
	delete editor;
	return a.exec();
}
