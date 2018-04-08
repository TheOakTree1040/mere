#ifndef SOURCEEDITOR_H
#define SOURCEEDITOR_H
#include "mere_math.h"
#include <QApplication>
#include <QTextEdit>
#include <QLayout>
#include <QPushButton>
#include "interpreter.h"

class SourceEditor : public QWidget{
		Q_OBJECT

		QTextEdit* editor = nullptr;
	public:
		SourceEditor(QWidget* parent = nullptr):QWidget(parent){
			editor = new QTextEdit();
			auto exec_button = new QPushButton("Execute");
			auto layout = new QVBoxLayout();
			layout->addWidget(editor);
			layout->addWidget(exec_button);
			this->setLayout(layout);
			connect(exec_button,&QPushButton::clicked,this,&SourceEditor::exec_clicked);
		}
		~SourceEditor(){
			delete MereMath::interpreter;
			MereMath::interpreter = 0;
		}
	public slots:
		void exec_clicked(){
			MereMath::run(editor->toPlainText());
		}

};
#endif // SOURCEEDITOR_H
