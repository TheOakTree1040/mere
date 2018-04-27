#include "cmd.h"
static Options MereCmdExec::options = std::bitset<8>(0b1000011);
static QCommandLineParser MereCmdExec::parser = QCommandLineParser();
static bool MereCmdExec::init = false;
