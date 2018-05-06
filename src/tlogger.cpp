
#include "tlogger.h"
#if _DEBUG
int TLogHelper::indentation = 0;
//template<>
//TLogger& TLogger::operator<<(const TBuiltinString& s){
//	return put(s);
//}
template<>
TLogger& TLogger::operator<<(const TString& s){
	return put(s);
}
template<>
TLogger& TLogger::operator<<(const char& ch){
	return put(QChar(ch));
}
#endif
