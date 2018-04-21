#ifndef CMDPARSER_H
#define CMDPARSER_H
#include <TString>
#include <bitset>

enum MICIndex{
	Exc,
	FFile,
	ShwSyn,
	ShwTok
};
class MereIntpCmdParser{

		std::bitset<8> options;
	public:
		CmdParser(){}
		void parse(const TString&){

		}

};

#endif // CMDPARSER_H
