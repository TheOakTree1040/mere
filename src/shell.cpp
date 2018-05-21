
#include <cstdlib>

#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDate>
#include <QStringList>

#include "shell.h"

#include "core.h"
#include "sourceeditor.hpp"

using namespace mere;

#if !T_GUI
using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::getline;
using std::cout;
#endif

//For MereCmder init
#define AS
#define SET_APP_NAME	App::setApplicationName
#define SET_APP_VER		App::setApplicationVersion
#define SET_APP_DESCR	parser.setApplicationDescription

//help_ostream (for MerePrompt)
#define hout (cout << "    ")

// ================== MerePrompt ==================

void MerePrompt::display_cmds(){
	hout << "\n\n";
	hout << "==============================================>\n";
	hout << "List of Commands:\n";
	hout << ".                       Execute the code you have typed.\n";
	hout << ".exec                   The same as above.\n";
	hout << ".quit                   Quit\n";
	hout << ".exit                   The same as above.\n";
	hout << ".help                   Opens the helper interface.\n";
	hout << ".clear                  Clear screen.\n";
	hout << ".last                   Add the last piece of code executed to the current input.\n";
	hout << ".view                   View the code you have typed (The input)\n";
	hout << ".prompt        [toggle] Print the prompt.\n";
	hout << ".ln            [toggle] Print extra lines before & after the execution.\n";
	hout << ".multi         [toggle] Multi-line / Single-line.\n";
	hout << ".calc          [toggle] Calculator mode; switches into single-line mode; same thing as 'print (INPUT);'\n";
	hout << ".set_prompt             Sets the prompt. You will be prompted to enter on the second line.\n";
	hout << ".ast           [toggle] Display the AST before the execution. It will be turned off after execution.\n";
	hout << ".tok           [toggle] Display the tokens before the execution. It will be turned off after execution.\n";
	hout << "==============================================>\n";
	hout << "\n\n";
}

void MerePrompt::tutor(){
	hout << "\n\n";
	hout << "===============================>\n";
	hout << "Quick tutorial (Too lazy)\n";
	hout << "Data types: real, string, bool, char, null, void\n";

	hout << "To print something:       print \"foo\";\n";
	hout << "To create a variable:     var bar = 1;\n";
	hout << "Loops:\n";
	hout << "    for loop:             for (var i = 0; i != 3; i++){}\n";
	hout << "    while loop:           while (false){}\n";
	hout << "Conditional:\n";
	hout << "    if statement:         if true print true else print false;\n";
	hout << "    ternary operator:     bar = 3<5?3:5;\n";
	hout << "Assert:                   assert 1 : \"Assertion failed.\";\n";
	hout << "    with code:            assert 1 : 0xFF;\n";
	hout << "    with both:            assert 1 : 0xFF, \"Assertion failed.\";\n";
	hout << "Function:                 fn a(){ return \"Howdy!\"; }\n";
	hout << "Reference (=>):           var ref; ref => bar;\n";

	hout << "===============================>\n";
	hout << "\n\n";
}

void MerePrompt::help(){
	cout << "\n";
	cout << "  " << "Welcome to the helper interface\n";
	cout << "  " << "For a list of commands, type 'cmds'\n"
					"  ""For a brief tutorial, type 'tutor'\n"
					"  ""To return to the editor, type 'ret'\n"
					"\n";
	string helpstr = "";
	forever{
		helpstr = "";
		hout << "help > ";
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

void MerePrompt::interface(){

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
					Core::reset_intp();
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
		Core::run(TString::fromStdString(input),tok,ast);
		if(lines)
			cout << "\n";
		if(input.size() && temp != ".last"){
			last = (calc||single)?temp:input;
		}
	}
}

// ================== MereCmder ==================

QCommandLineParser MereCmder::parser;
short MereCmder::status = 0;

void MereCmder::_init(){
	LFn;
	if (!status){
		status = 2;
		//					Core::init_once();

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
		Core::interpreter = new Interpreter();
	}
	LVd;
}

void MereCmder::set(Opt index, bool _val){
	options.set(t_cast<size_t>(index),_val);
}

bool MereCmder::test(Opt index) const {
	return options.test(t_cast<size_t>(index));
}

void MereCmder::clean_up() {
	if (status) {
		status--;
		delete Core::interpreter;
		Core::interpreter = nullptr;
	}
}

constexpr qulonglong
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

bool MereCmder::execute(){
	LFn;
	_init();
	parser.process(App::arguments());
	if (parser.unknownOptionNames().size()){
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
		return source.size()?Core::run(source, tok, syn):false;
	}
	else if (test(Opt::Prompt)) {
		MerePrompt().interface();
	}
	LRet false;
}


