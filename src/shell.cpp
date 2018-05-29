
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
	std::cout << "\n" << list_of_commands << "\n";
}

void MerePrompt::tutor(){
	std::cout << "\n" << tutorial << "\n";
}

MerePrompt::MerePrompt(){
	QFile resrc_list_cmds(":/help/list-of-commands.txt");

	if (!resrc_list_cmds.open(QIODevice::ReadOnly | QIODevice::Text)){
		std::cout << "  > failure: failed to load resource: list of commands\n";
		list_of_commands = "[failure: failed to load resource: list of commands]\n";
	}
	else {
		list_of_commands = resrc_list_cmds.readAll().toStdString();
	}

	QFile resrc_tutor(":/help/tutorial.txt");

	if (!resrc_tutor.open(QIODevice::ReadOnly | QIODevice::Text)){
		std::cout << "  > failure: failed to load resource: tutorial\n";
		tutorial = "[failure: failed to load resource: tutorial]\n";
	}
	else {
		tutorial = resrc_tutor.readAll().toStdString();
	}
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
	forever{ // prompt

		delete unit;
		input = temp = "";
		ast = tok = false;

		forever{ // get input
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

		parser.addOption({{"p","prompt"},"Enters the prompt mode."});
		parser.addOption({{"s","show-snippet"},"Opt to add a code snippet when printing issues."});

		parser.addPositionalArgument("filename","File to interpret.", "[filename]");

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
	if (parser.isSet("show-snippet"))
		InterpretationUnit::show_snippet(true);
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


