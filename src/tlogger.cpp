
#include "tlogger.h"
#if T_DBG
#if T_GUI
#include <QDebug>
#else
#include <iostream>
#endif // T_GUI

using namespace mere;

int TLogHelper::indentation = 0;

template<>
TLogger& TLogger::operator<<(const TString& s){
	return put(s);
}

template<>
TLogger& TLogger::operator<<(const char& ch){
	return put(QChar(ch));
}

TString mere::TLogHelper::startln(const TString& starter){
	TString out = "";
	if (indentation < 0)
		indentation = 0;
	for (int i = indentation; i > 0; i--){
		out += "  ";
	}
	return out + " " + starter + " ";
}

TLogger::~TLogger(){
	if (out.size()){
#if T_GUI
		qDebug().noquote() << TLogHelper::startln(st) << out;
#elif T_CLI
		std::cout << (TLogHelper::startln(st) + " " + out + "\n").toStdString();
#endif // T_UI_Conf == T_UI_GUI
	}
}
#endif


