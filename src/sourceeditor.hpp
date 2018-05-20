#ifndef SOURCEEDITOR_H
#define SOURCEEDITOR_H
#include "core.h"
#include "interpreter.h"

#if T_GUI
#include <QApplication>
#include <QTextEdit>
#include <QLayout>
#include <QPushButton>
#include <QCheckBox>

namespace mere {

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
				auto reset_btn = new QPushButton("Reset");
				auto layout = new QVBoxLayout();

				layout->addWidget(chk_tok);
				layout->addWidget(chk_syn);
				layout->addWidget(editor);
				layout->addWidget(exec_button);
				layout->addWidget(reset_btn);
				setLayout(layout);

				connect(exec_button,&QPushButton::clicked,this,&SourceEditor::exec_clicked);
				connect(reset_btn,&QPushButton::clicked,this,&SourceEditor::reset_clicked);
				LVd;
			}
		public slots:
			void exec_clicked(){
				Core::run(editor->toPlainText(),chk_tok->isChecked(),chk_syn->isChecked());
				Logp("clicked");
			}
			void reset_clicked(){
				Core::reset_intp();
			}
	};
}
#else
#include <QCoreApplication>
#endif // T_UI_Conf == T_UI_GUI
#endif // SOURCEEDITOR_H
