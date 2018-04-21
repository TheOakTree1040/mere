#ifndef SOURCEEDITOR_H
#define SOURCEEDITOR_H
#include "mere_math.h"
#include <QApplication>
#include <QTextEdit>
#include <QLayout>
#include <QPushButton>
#include "interpreter.h"
#include <QCheckBox>

class SourceEditor : public QWidget{
		Q_OBJECT
		QTextEdit* editor = nullptr;
		QCheckBox* chk_tok = nullptr;
		QCheckBox* chk_syn = nullptr;
	public:
		SourceEditor(QWidget* parent = nullptr):QWidget(parent){
			LFn;
			editor = new QTextEdit();
			chk_tok = new QCheckBox("Show Tokens");
			chk_syn = new QCheckBox("Show Syntax Tree");
			QFont font;

			font.setFamily("Courier");
			font.setStyleHint(QFont::Monospace);
			font.setFixedPitch(true);
			font.setPointSize(16);
			editor->setFont(font);
			editor->setTabStopWidth(4*QFontMetrics(font).width(" "));
			auto exec_button = new QPushButton("Execute");
			auto layout = new QVBoxLayout();
			layout->addWidget(chk_tok);
			layout->addWidget(chk_syn);
			layout->addWidget(editor);
			layout->addWidget(exec_button);
			setLayout(layout);

			connect(exec_button,&QPushButton::clicked,this,&SourceEditor::exec_clicked);
			LVd;

		}
	public slots:
		void exec_clicked(){
			MereMath::run(editor->toPlainText(),chk_tok->isChecked(),chk_syn->isChecked());
			Log << "clicked";
		}

};
#endif // SOURCEEDITOR_H
