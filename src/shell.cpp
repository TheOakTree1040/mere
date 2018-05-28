
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDate>
#include <QStringList>

#include "tlogger.h"
#include "shell.h"
#include "sourceeditor.h"
#include "interpretationunit.h"

#include <cstdlib>
#include <iostream>

using namespace mere;

#if !T_GUI
using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::getline;
using std::cout;
#endif

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
	hout << ".cls                    Clears screen.\n";
	hout << ".clear                  Clears the input.";
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
	cout << "\n"
			"  ""Welcome to the helper interface\n"
			"  ""For a list of commands, type 'cmds'\n"
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

	string initial_output = string(PROJECT) + " " + VERSION + " build " + BUILD + " by " + AUTHOR + "\n\n";
	initial_output += "Common commands:\n"
					  "	.exec	Execute the code you've typed.\n"
					  "	.help	Display help.\n"
					  "	.exit	Exit.\n"
					  "\n";
	cout << initial_output;
	Interpreter intp;
	IntpUnit unit = nullptr;
	forever{
		delete unit;
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
					if(print_lines)
						cout << "\n";
					if (!single && !calc)
						break;
				}
				else if (temp == ".help"){
					help();
				}
				else if (temp == ".cls"){
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
					cout << "  > print_lines = " << ((print_lines = !print_lines)?"on":"off") << "\n";
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
				else if (temp == ".clear"){
					input = "";
				}
				else if (temp == "._rst"){
					cout << "  > Resetting the interpreter...\n";
					intp.reset();
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
					if(print_lines)
						cout << "\n";
					if(calc)
						input = "println (" + input + ");";

					break;
				}
			}
		}
		unit = InterpretationUnit::from_source(QString::fromStdString(input));
		if (unit->success()){
			if (tok){
				unit->print_tokens();
			}
			if (intp.interpret(unit))
				unit->print_issues();

		}
		else {
			unit->print_issues();
		}
		if(print_lines)
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
		Log1("Setting up parser...");
		status = 1;

		App::setApplicationName(PROJECT);
		App::setApplicationVersion(VERSION " " BUILD);
		parser.setApplicationDescription(DESCRIPTION);

		parser.addHelpOption();
		parser.addVersionOption();

		parser.addOption({{"p","prompt"},"Enter the prompt mode (filename not needed)"});

		parser.addPositionalArgument("filename","File to interpret", "[filename]");

	}
	LVd;
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

short MereCmder::execute(){
	LFn;
	_init();
	parser.process(App::arguments());
	if (parser.isSet("prompt")){
		MerePrompt().interface();
		LRet 0;
	}
	auto arg = parser.positionalArguments();
	if (arg.empty()){
		std::cerr << "error: filename unspecified (enter '-p' for prompt mode)\n\n";
		parser.showHelp(EXIT_FAILURE);
	}
	IntpUnit unit = new InterpretationUnit(arg[0]);
	Interpreter().interpret(unit);
	if (!unit->success()) unit->print_issues();
	delete unit;
	LRet 0;
}


