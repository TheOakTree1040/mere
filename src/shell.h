#ifndef CMDPARSER_H
#define CMDPARSER_H

#include "config.hpp"

#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDate>
#include <QStringList>

#if T_GUI
#include <QApplication>
#include <QDialog>
#endif

#include <bitset>
#include <cstdlib>

#include "mere_math.h"
#include "sourceeditor.hpp"


using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::getline;
using std::cout;

typedef std::bitset<8> Options;
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
		void display_cmds(){
			cout << "\n\n";
			cout << "==============================================>\n";
			cout << "List of Commands:\n"
					".                       Execute the code you have typed.\n"
					".exec                   The same as above.\n"
					".quit                   Quit\n"
					".exit                   The same as above.\n"
					".help                   Opens the helper interface.\n"
					".clear                  Clear screen.\n"
					".last                   Add the last piece of code executed to the current input.\n"
					".view                   View the code you have typed (The input)\n"
					".prompt        [toggle] Print the prompt.\n"
					".ln            [toggle] Print extra lines before & after the execution.\n"
					".multi         [toggle] Multi-line / Single-line.\n"
					".calc          [toggle] Calculator mode; switches into single-line mode; same thing as 'print (INPUT);'\n"
					".set_prompt             Sets the prompt. You will be prompted to enter on the second line.\n"
					".ast           [toggle] Display the AST before the execution. It will be turned off after execution.\n"
					".tok           [toggle] Display the tokens before the execution. It will be turned off after execution.\n";
			cout << "==============================================>\n";
			cout << "\n\n";
		}
		void tutor(){
			cout << "\n\n";
			cout << "===============================>\n";
			cout << "Quick tutorial (Too lazy)\n";
			cout <<
					"Data types: real, string, bool, char, null, void\n"

					"To print something:       print \"foo\";\n"
					"To create a variable:     var bar = 1;\n"
					"Loops:\n"
					"    for loop:             for (var i = 0; i != 3; i++){}\n"
					"    while loop:           while (false){}\n"
					"Conditional:\n"
					"    if statement:         if true print true else print false;\n"
					"    ternary operator:     bar = 3<5?3:5;\n"
					"Assert:                   assert 1 : \"Assertion failed.\";\n"
					"    with code:            assert 1 : 0xFF;\n"
					"    with both:            assert 1 : 0xFF, \"Assertion failed.\";\n"
					"Function:                 fn a(){ return \"Howdy!\"; }\n"
					"Reference (=>):           var ref; ref => bar;\n";

			cout << "===============================>\n";
			cout << "\n\n";
		}

		void help(){
			cout << "\n";
			cout << "Welcome to the helper interface\n";
			cout << "For a list of commands, type 'cmds'\n"
					"For a brief tutorial, type 'tutor'\n"
					"To return to the editor, type 'ret'\n"
					"\n";
			string helpstr = "";
			forever{
				helpstr = "";
				cout << "help > ";
				std::getline(cin,helpstr);
				if (helpstr == "cmds"){
					display_cmds();
				}
				else if (helpstr == "tutor"){
					tutor();
				}
				else if (helpstr == "ret"){
					cout << "\n";
					return;
				}
			}
		}

	public:
		MerePrompt(){}

		void interface(){

			string initial_output = string PROJECT + " " + VERSION + " build " + BUILD + " by " + AUTHOR + "\n\n";
			initial_output += "Common commands:\n"
								"	.exec	Execute the code you've typed.\n"
								"	.help	Display help.\n"
								"	.exit	Exit.\n"
								"\n";
			cout << initial_output;

			forever{
				input = temp = "";
				ast = tok = false;
				forever{
					if(print_prompt)
						cout << prompt;
					std::getline(cin,temp);
					if(!temp.size()){
						if((single || calc) && input.size())
							goto PRE_EXEC;
						input += "\n";
						continue;
					}
					if (temp[0] == '.'){
						temp = QString::fromStdString(temp).trimmed().toStdString();
						if		(temp == "." || temp == ".exec") {
							if(lines)
								cout << "\n";
							if (!single && !calc)
								break;
						}
						else if (temp == ".help"){
							help();
						}
						else if (temp == ".clear"){
#if T_DARWIN
							system("clear");
#elif T_WIN32
							system("cls");
#endif
						}
						else if (temp == ".last"){
							input += last;
							if(single || calc)
								goto EXEC;
						}
						else if (temp == ".view"){
							cout << "--beg--\n" << input << "--end--\n";
						}
						else if (temp == ".multi"){
							cout << "  > multi = " << (!(single = !single)?"on":"off") << "\n";
						}
						else if (temp == ".calc"){
							input = "";
							cout << "  > calc = " << ((calc = !calc)?"on":"off") << "\n";
							single = true;
							continue;
						}
						else if (temp == ".ln"){
							cout << "  > print_lines = " << ((lines = !lines)?"on":"off") << "\n";
						}
						else if (temp == ".prompt"){
							cout << "  > print_prompt = " << ((print_prompt = !print_prompt)?"on":"off") << "\n";
						}
						else if (temp == ".set_prompt"){
							cout << prompt << "prompt = ";
							std::getline(cin,temp);
							prompt = temp;
						}
						else if (temp == ".ast"){
							cout << "  > ast = " << ((ast = !ast)?"on":"off") << "\n";
						}
						else if (temp == ".tok"){
							cout << "  > tok = " << ((tok = !tok)?"on":"off") << "\n";
						}
						else if	(temp == ".quit" || temp == ".exit") return;
						else if (temp == "._rst"){
							cout << "  > Resetting the interpreter...\n";
							MereMath::reset_intp();
							cout << "  > Done resetting.\n";
						}
						else {
							goto PRE_EXEC;
						}
					}
					else {
						PRE_EXEC:
						input += temp + (calc||single?"":"\n");
						EXEC:
						if (single || calc){
							if(lines)
								cout << "\n";
							if(calc)
								input = "println (" + input + ");";

							break;
						}
					}
				}
				MereMath::run(TString::fromStdString(input),tok,ast);
				if(lines)
					cout << "\n";
				if(input.size() && temp != ".last"){
					last = (calc||single)?temp:input;
				}
			}
		}
};

#define AS
#define SET_APP_NAME	App::setApplicationName
#define SET_APP_VER		App::setApplicationVersion
#define SET_APP_DESCR	parser.setApplicationDescription

class MereCmder{
		Options options = 0;
		static QCommandLineParser parser;
		static short status;
		static void _init(){
			LFn;
			if (!status){
				status = 2;
				MereMath::init_once();

				SET_APP_NAME AS PROJECT;
				SET_APP_VER AS VERSION;
				SET_APP_DESCR AS DESCRIPTION;

				parser.addHelpOption();
				parser.addVersionOption();

				QCommandLineOption mode_opt({"mode","m"},"The output mode","mode","exec");//handled
				QCommandLineOption dbg_opt({"dbg","d"},"The debugging tools.","tool-name");//handled
#if T_GUI
				QCommandLineOption edtr_opt({"editor","edtr","e"},"Opens the inbuilt editor.");//handled
#endif
				QCommandLineOption ff_opt({"file","f"},"File input.","filename");//handled
				QCommandLineOption src_opt({"src","s"},"The source that you want to execute directly.","code");

				parser.addOptions({
									  mode_opt,
									  dbg_opt,
					  #if T_GUI
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
			qulonglong hash = 0xA8ULL;
			while (*string)
			{
				hash ^= qulonglong(*string++);
				hash *= qulonglong(0x3C17ULL);
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
					QStringList tools({"tok","tree","syn"});
					QStringList opts = parser.values("dbg");
					for (int i = 0; i != opts.size(); i++){
						switch(tools.indexOf(opts[i])){
							case 0:
								set(Opt::ShwTok);
								break;
							case 1:
							case 2:
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
#if T_GUI
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
#if T_GUI
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
#if T_CLI
				set(Opt::Prompt);
#else
				set(Opt::Edtr);
#endif
			}
			//execution

			//setup
			bool tok = test(Opt::ShwTok), syn = test(Opt::ShwSyn);
			if (test(Opt::Exc)) {
				TString source = "";
#if T_GUI
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
