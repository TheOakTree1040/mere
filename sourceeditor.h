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
			LFn;
			editor = new QTextEdit();
			QFont font;

			font.setFamily("Courier");
			font.setStyleHint(QFont::Monospace);
			font.setFixedPitch(true);
			font.setPointSize(16);
			editor->setFont(font);
			editor->setTabStopWidth(4*QFontMetrics(font).width(" "));
			auto exec_button = new QPushButton("Execute");
			auto layout = new QVBoxLayout();
			layout->addWidget(editor);
			layout->addWidget(exec_button);
			setLayout(layout);

			connect(exec_button,&QPushButton::clicked,this,&SourceEditor::exec_clicked);
			Log << "SourceEditor()";
			Logger::indent--;

		}
	public slots:
		void exec_clicked(){
			MereMath::run(editor->toPlainText());
			Log << "clicked";
		}

};
#endif // SOURCEEDITOR_H
