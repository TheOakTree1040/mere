#ifndef CMDPARSER_H
#define CMDPARSER_H
#include "t.hpp"
#include <bitset>
#include <QCommandLineParser>
#include "mere_math.h"
#include <QApplication>
#include "sourceeditor.hpp"
#include <QFile>
#include <QDialog>
#include <iostream>
#include <QTextStream>
#include <QTimer>
using std::cout;
typedef std::bitset<8> Options;
enum Opt{
	Exc,//mode
	FFile,
	ShwSyn,//dbg
	ShwTok,//dbg
	Prompt,//mode
	GUI,//ui
	Edtr//edtr
};
class SrcEdit : public QDialog {
	Q_OBJECT

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
class MereCmder{
		Options options = 0b0000010;
		static QCommandLineParser parser;
		static short status;
		static void _init(){
			LFn;
			if (!status){
				status = 2;
				MereMath::init_once();
				QApplication::setApplicationName("MereMath Interpreter");
				QApplication::setApplicationVersion("0.0.0.3-pre-alpha");
				parser.setApplicationDescription("The MereMath Interpreter (cmm)");
				parser.addHelpOption();
				parser.addVersionOption();
				QCommandLineOption mode_opt({"mode","m"},"The output mode","[mode]","exec");//handled
				QCommandLineOption gui_opt({"ui","u"},"The UI used by cmm.","[ui]","gui");//
				QCommandLineOption dbg_opt({"dbg","d"},"The debugging tools.","[dbg-tl]");//handled
				QCommandLineOption edtr_opt({"editor","edtr","e"},"Opens the inbuilt editor.");//handled
				QCommandLineOption ff_opt({"file","f"},"Input from file.","[file]");//handled
				parser.addOptions({mode_opt,gui_opt,dbg_opt,edtr_opt,ff_opt});
			}
			else {
				status++;
				MereMath::interpreter = new Interpreter();
			}
			LVd;
		}
		static constexpr qulonglong
		h(const char* string)
		{
			qulonglong hash = 0xEEF8UL;
			while (*string)
			{
				hash ^= (qulonglong)(*string++);
				hash *= 0x8C17UL;
			}

			return hash;
		}
		void set(Opt index, bool _val = true){
			options.set(t_cast<size_t>(index),_val);
		}
		bool test(Opt index) {
			return options.test(t_cast<size_t>(index));
		}

	public:
		static void clean_up() {
			if (status) {
				status--;
				delete MereMath::interpreter;
				MereMath::interpreter = nullptr;
			}
		}
		~MereCmder() {
			clean_up();
		}
        bool execute(){
			LFn;
			_init();
			//parsing
			parser.process(QApplication::arguments());
			if (parser.unknownOptionNames().size()){
				LVd;
				parser.showHelp(EXIT_FAILURE);
			}
			if (parser.isSet("mode")){
				switch(h(parser.value("mode").toStdString().c_str())){
					case h("exec"):
						set(Opt::Exc);
						if (parser.isSet("editor")){
							set(Opt::Edtr);
						}
						else if (parser.isSet("file")){
							set(Opt::FFile);
						}
						else {
							set(Opt::Edtr);
						}
						break;
					case h("prompt"):
						set(Opt::Prompt);
						break;
					default:
						LVd;
						parser.showHelp(EXIT_FAILURE);
				}
			}
			if (parser.isSet("ui")){
				switch(h(parser.value("ui").toStdString().c_str())){
					case h("qtgui"):
					case h("gui"):
						set(Opt::GUI);
						break;
					case h("con"):
						break;
					default:
						LVd;
						parser.showHelp(EXIT_FAILURE);
				}
			}
			if (parser.isSet("dbg")){
				QStringList values = parser.values("dbg");
				for (int i = values.size(); i != 0; i--){
					switch(h(parser.value("ui").toStdString().c_str())){
						case h("tok"):
							set(Opt::ShwTok);
							break;
						case h("tree"):
						case h("syn"):
							set(Opt::ShwSyn);
							break;
						default:
							LVd;
							parser.showHelp(EXIT_FAILURE);
					}
				}
			}
			if (!test(Opt::Exc) && !test(Opt::Prompt)) {
				LVd;
				parser.showHelp(EXIT_FAILURE);
			}

			//execution
			//setup
			bool tok = test(Opt::ShwTok), syn = test(Opt::ShwSyn);
			if (test(Opt::Exc)) {
				TString source = "";
				if (test(Opt::Edtr)) {
					SrcEdit* edt = new SrcEdit;
					edt->exec();
					source = edt->text();
					edt->deleteLater();
				}
				else if (test(Opt::FFile)) {
					QFile file(parser.value("file"));
					if (file.isOpen()) {
						source = file.readAll();
						file.close();
					}
					else {
						source = "print \"Invalid Filename!\"";
					}
				}
                if (source.isEmpty())
                    return 0;
				MereMath::run(source, tok, syn);
                return 1;
			}
			else if (test(Opt::Prompt)) {
				/*TString src = "";
				QTextStream in(stdin);
				QTextStream out(stdout);*/
				std::string src = "";
				QTimer* timer = new QTimer();
				timer->setInterval(90000);
				timer->setSingleShot(true);
				timer->deleteLater();
				auto app = QApplication::instance();
				app->connect(timer, &QTimer::timeout, app, &QApplication::quit);
				cout << " < ";
				std::cin >> src;
				//out << " > ";
				//in >> src;
				if (src != "__cali__;") {
					timer->stop();
					return 0;
				}
				timer->stop();
				for (;;) {
					cout << " > ";
					std::cin >> src;
                    if (src == "quit();")
                        return 0;
                    MereMath::run(TString::fromStdString(src)/*src*/, tok, syn);
				}
			}
			LVd;
		}
};

#endif // CMDPARSER_H
