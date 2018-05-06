#ifndef CMDPARSER_H
#define CMDPARSER_H

#include "t.hpp"

#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>
#include <QTimer>

#if T_UI_Conf == T_UI_GUI
#include <QApplication>
#include <QDialog>
#endif

#include <iostream>
#include <bitset>
#include <cstdlib>

#include "mere_math.h"
#include "sourceeditor.hpp"

using std::cout;
typedef std::bitset<8> Options;
enum class Opt{
	Exc,//mode
	FFile,
	ShwSyn,//dbg
	ShwTok,//dbg
	Prompt,//mode
#if IS_GUI_APP
	Edtr,//edtr
#endif
	Src
};
#if T_UI_Conf == T_UI_GUI
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
#elif T_UI_Conf == T_UI_CLI
class MerePrompt{
	private:
		std::string prompt = " >> ";
	public:
		MerePrompt(){}
		static void back(int n){
			return;
			if (!n)
				return;
			std::string out = "";
			std::string bk = "";
			for (int i = n; i != 0; i--){
				bk += "\b";
			}
			out += bk;
			for (int i = n; i != 0; i--){
				out += " ";
			}
			out += bk;
			printf("%s",out.c_str());
		}

		void interface(){
			using std::cout;
			using std::endl;
			using std::cin;
			using std::string;
			using std::getline;

			string initial_output = string PROJECT + " " + VERSION COUT_COMPAT + " by " + AUTHOR COUT_COMPAT + "\n\n";
			initial_output += "Common commands:\n"
								"	.exec	Execute the code you've typed.\n"
								"	.help	Display help.\n"
								"	.quit	Quit.\n"
								"\n";
			cout << initial_output;
			string input = "";
			string last = "";

			string temp = "";

			bool print_prompt = true;
			bool syn = false, tok = false;
			bool single = true, lines = true;
			bool calc = false;
			forever{
				input = temp = "";
				syn = tok = false;
				forever{
					if(print_prompt)
						cout << prompt;
					getline(cin,temp);
					if(!temp.size()){
						input += "\n";
						continue;
					}
					if (temp[0] == '.'){
						temp = QString::fromStdString(temp).trimmed().toStdString();
                        if		(temp == "." || temp == ".exec") {
							back(prompt.size()+5+1);
							if(lines)
								cout << "\n";
							if (!single && !calc)
								break;
						}
						else if (temp == ".help"){
							string output = "Help page not available.";
							cout << output;
							cin.get();
							back(prompt.size()+5+1+output.size()+1);
						}
						else if (temp == ".clear"){
#if T_UNDER_PF(T_PF_OSX)
							system("clear");
#elif T_UNDER_PF(T_PF_WIN)
							system("cls");
#endif
						}
						else if (temp == ".last"){
							input += "\n" + last;
						}
						else if (temp == ".view"){
							cout << "----\n" << input << "----\n";
						}
						else if (temp == ".toggle_multi"){
							single = !single;
						}
						else if (temp == ".calc"){
							input = "";
							calc = !calc;
							single = true;
							continue;
						}
						else if (temp == ".toggle_lines"){
							lines = !lines;
						}
						else if (temp == ".toggle_prompt"){
							print_prompt = !print_prompt;
						}
						else if (temp == ".set_prompt"){
							int bk = prompt.size() + 11 + 1;
							cout << prompt;
							bk += prompt.size();
							getline(cin,temp);
							bk += temp.size() + 1;
							back(bk);
							prompt = temp;
						}
						else if (temp == ".toggle_syn"){
							back(prompt.size() + 11 + 1);
							syn = !syn;
						}
						else if (temp == ".toggle_tok"){
							back(prompt.size() + 11 + 1);
							tok = !tok;
						}
						else if	(temp == ".quit") return;
						else {
							goto PRE_EXEC;
						}
					}
					else {
						PRE_EXEC:
						input += temp + (calc||single?"":"\n");
						if (single || calc){
							if(lines)
								cout << "\n";
							break;
						}
					}
				}
				Log1(TString::fromStdString("Input: " + input));
				if(calc)
					input = "print (" + input + ");";
				MereMath::run(TString::fromStdString(input),tok,syn);
				if(lines)
					cout << "\n";
				last = input;
			}
		}
};

#endif
class MereCmder{
		Options options = 0;
		static QCommandLineParser parser;
		static short status;
		static void _init(){
			LFn;
			if (!status){
				status = 2;
				MereMath::init_once();
				App::setApplicationName PROJECT ;
				App::setApplicationVersion VERSION ;
				parser.setApplicationDescription DESCRIPTION ;
				parser.addHelpOption();
				parser.addVersionOption();
				QCommandLineOption mode_opt({"mode","m"},"The output mode","mode","exec");//handled
				QCommandLineOption dbg_opt({"dbg","d"},"The debugging tools.","tool-name");//handled
#if IS_GUI_APP
				QCommandLineOption edtr_opt({"editor","edtr","e"},"Opens the inbuilt editor.");//handled
#endif
				QCommandLineOption ff_opt({"file","f"},"File input.","filename");//handled
				QCommandLineOption src_opt({"src","s"},"The source that you want to execute directly.","code");
				parser.addOptions({
									  mode_opt,
									  dbg_opt,
					  #if IS_GUI_APP
									  edtr_opt,
					  #endif
									  ff_opt,
									  src_opt
								  });
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
			parser.process(App::arguments());
			if (parser.unknownOptionNames().size()){
				LVd;
				parser.showHelp(EXIT_FAILURE);
			}
			if (parser.isSet("mode")){
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
				switch(h(parser.value("mode").toStdString().c_str())){
					case h("exec"):
						set(Opt::Exc);
#if IS_GUI_APP
						if (parser.isSet("editor")){
							set(Opt::Edtr);
						} else
#endif
						if (parser.isSet("file")){
							set(Opt::FFile);
						}
						else if (parser.isSet("src")){
							set(Opt::Src);
						}
#if IS_GUI_APP
						else {
							set(Opt::Edtr);
						}
#else
						else {
							set(Opt::Prompt);
							set(Opt::Exc,false);
						}
#endif
						break;
					case h("prompt"):
						set(Opt::Prompt);
						break;
					default:
						LVd;
						parser.showHelp(EXIT_FAILURE);
				}
			}
			else {
				set(Opt::Prompt);
			}
			//execution

			//setup
			bool tok = test(Opt::ShwTok), syn = test(Opt::ShwSyn);
			if (test(Opt::Exc)) {
				TString source = "";
#if IS_GUI_APP
				if (test(Opt::Edtr)) {
					SrcEdit* edt = new SrcEdit;
					edt->exec();
					source = edt->text();
					delete edt;
				}
				else
#endif
				if (test(Opt::FFile)) {
					QFile file(parser.value("file"));
					if (file.isOpen()) {
						source = file.readAll();
						file.close();
					}
					else {
						source = "print \"Invalid Filename!\";";
					}
				}
				else if (test(Opt::Src)){
					source = parser.value("src");
				}
				else parser.showHelp(EXIT_FAILURE);
				return source.size()?MereMath::run(source, tok, syn):false;
			}
			else if (test(Opt::Prompt)) {
				MerePrompt().interface();
			}
			LRet false;
		}
};

#endif // CMDPARSER_H
