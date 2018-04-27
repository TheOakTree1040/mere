#ifndef CMDPARSER_H
#define CMDPARSER_H
#include "t.h"
#include <bitset>
#include <QCommandLineParser>
#include "mere_math.h"
#include <QApplication>
#include "sourceeditor.h"
typedef std::bitset<8> Options;
enum MICIndex{
	Exc,//mode
	FFile,
	ShwSyn,//dbg
	ShwTok,//dbg
	Prompt,//mode
	GUI,//ui
	Edtr//edtr
};
class MereCmdExec{
		static Options options;
		static QCommandLineParser parser;
		static bool init;
		static void _init(){
			if (!init){
				init = true;
				QApplication::setApplicationName("MereMath Interpreter");
				QApplication::setApplicationVersion("0.0.0.3-bkn");
				parser.setApplicationDescription("The MereMath Interpreter (cmm)");
				parser.addHelpOption();
				parser.addVersionOption();
				auto mode_opt = QCommandLineOption({"mode","m"},"The output mode","[mode]","exec");
				auto gui_opt = QCommandLineOption({"ui","u"},"The UI used by cmm.","[ui]","gui");
				auto dbg_opt = QCommandLineOption({"dbg","d"},"The debugging tools.","[dbg-tl]");
				auto edtr_opt = QCommandLineOption({"editor","edtr","e"},"Opens the inbuilt editor.");
				auto ff_opt = QCommandLineOption({"file","f"},"Input from file.","[file]");
				parser.addOptions({mode_opt,gui_opt,dbg_opt,edtr_opt,ff_opt});
			}
		}
		static constexpr long long
		h(const char* string)
		{
			long long hash = 0xeef8;
			while (*string)
			{
				hash ^= (uint)(*string++);
				hash *= 0xAE9D;
			}

			return hash;
		}
		void set(MICIndex index, bool _val = true){
			options.set(t_cast<size_t>(index),_val);
		}

	public:
		static void execute(){
			_init();
			parser.process(QApplication::arguments());
			if (parser.unknownOptionNames().size()){
				parser.showHelp(EXIT_FAILURE);
			}
			if (parser.isSet("mode")){
				switch(h(parser.value("mode").toStdString().c_str())){
					case h("exec"):
						set(MICIndex::Exc);
						if (parser.isSet("editor")){
							set(MICIndex::Edtr);
						}
						else if (parser.isSet("file")){
							set(MICIndex::FFile);
						}
						else {
							set(MICIndex::Edtr);
						}
						break;
					case h("prompt"):
						set(MICIndex::Prompt);
						break;
					default:
						parser.showHelp(EXIT_FAILURE);
				}
			}
			if (parser.isSet("ui")){
				switch(h(parser.value("ui").toStdString().c_str())){
					case h("qtgui"):
					case h("gui"):
						set(MICIndex::GUI);
						break;
					case h("con"):
						break;
					default:
						parser.showHelp(EXIT_FAILURE);
				}
			}
			if (parser.isSet("dbg")){
				QStringList values = parser.values("dbg");
				for (int i = values.size(); i != 0; i--){
					switch(h(parser.value("ui").toStdString().c_str())){
						case h("tok"):
							set(MICIndex::ShwTok);
							break;
						case h("tree"):
						case h("syn"):
							set(MICIndex::ShwSyn);
							break;
						default:
							parser.showHelp(EXIT_FAILURE);
					}
				}
			}

		}
};

#endif // CMDPARSER_H
