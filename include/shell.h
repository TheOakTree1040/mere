#ifndef CMDPARSER_H
#define CMDPARSER_H


#include <bitset>

#include <QCommandLineParser>
#if T_GUI
#include <QApplication>
#include <QDialog>
#endif

#include "config.h"

typedef std::bitset<8> Options;

namespace mere {
	enum class Opt{
		Exc,//mode
		FFile,
		ShwSyn,//dbg
		ShwTok,//dbg
		Prompt,//mode
#if T_GUI
		Edtr,//edtr
#endif
		Src
	};
#if T_GUI
	class SrcEdit : public QDialog {
			Q_OBJECT
		private
			QTextEdit* editor;
		public:
			SrcEdit():QDialog(),editor(new QTextEdit()){
				QVBoxLayout* layout = new QVBoxLayout();

				QFont font;
				font.setFamily("Consolas");
				font.setStyleHint(QFont::Monospace);
				font.setFixedPitch(true);
				font.setPointSize(16);
				editor->setFont(font);
				editor->setTabStopWidth(4 * QFontMetrics(font).width(" "));

				auto exec_button = new QPushButton("Execute");

				layout->addWidget(editor);
				layout->addWidget(exec_button);
				setLayout(layout);

				connect(exec_button, &QPushButton::clicked, this, &SrcEdit::accept);
			}
			TString text() {
				return editor->toPlainText();
			}
	};
#endif
	class MerePrompt{
		private:
			std::string prompt	= " >> ";// The prompt
			std::string input	= ""	;// The input (accumulated temps)
			std::string last	= ""	;// The last piece of code executed
			std::string temp	= ""	;// User input

			bool print_prompt	= true	;// whether to print the prompt
			bool ast			= false	;// whether to show the syntax tree
			bool tok			= false	;// whether to show the token
			bool single			= false	;// whether single or multiline code is expected
			bool lines			= false	;// whether to print extra lines before and after code execution
			bool calc			= false	;// calculator mode
		private:
			void help();
			void display_cmds();
			void tutor();

		public:
			MerePrompt(){}
			void interface();
	};

	class MereCmder{
		private:
			Options options = 0;
			static QCommandLineParser parser;
		private:
			static short status;
			static void _init();
			void set(Opt index, bool _val = true);
			bool test(Opt index) const;

		public:
			~MereCmder() { clean_up(); }
			static void clean_up();
			bool execute();
	};
}

#endif // CMDPARSER_H
