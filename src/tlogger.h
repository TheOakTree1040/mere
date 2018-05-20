
#ifndef T_LOGGER
#define T_LOGGER
#include "config.hpp"

#if T_DBG
#if T_GUI
#include <QDebug>
#else
#include <iostream>
#endif // T_GUI

namespace mere {
	class TLogHelper{
			static int indentation;
		public:
			static void indent(){
				indentation++;
			}
			static void outdent(){
				indentation--;
			}
			static TString startln(const TString& starter){
				TString out = "";
				if (indentation < 0)
					indentation = 0;
				for (int i = indentation; i > 0; i--){
					out += "  ";
				}
				return out + " " + starter + " ";
			}
			static void reset(){
				indentation = 0;
			}
	};

	class TLogger{
		private:
			TString out = "";
			TString st = "|";
			bool noln = false;
		public:
			TLogger(){
			}

			TLogger& put(const TString& s){
				out += s + " ";
				return *this;
			}
			template<typename Num>
			TLogger& operator<<(const Num& d){
				return put(TString::number(d));
			}

			TLogger& operator<<(const char str[]){
				return put(TString(str));
			}

			TLogger& indent(){
				TLogHelper::indent();
				return *this;
			}
			TLogger& outdent(){
				TLogHelper::outdent();
				return *this;
			}
			TLogger& noline(){
				noln = true;
				return *this;
			}
			TLogger& begw(const TString& str){
				st = str;
				return *this;
			}

			~TLogger(){
				if (out.size()){
#if T_GUI
					qDebug().noquote() << TLogHelper::startln(st) << out;
#elif T_CLI
					std::cout << (TLogHelper::startln(st) + " " + out + "\n").toStdString();
#endif // T_UI_Conf == T_UI_GUI
				}
			}
	};
	//template<>
	//TLogger& TLogger::operator<<(const TBuiltinString& s);
	template<>
	TLogger& TLogger::operator<<(const TString& s);
	template<>
	TLogger& TLogger::operator<<(const char& ch);
}

#define LRst	TLogHelper::reset()
#define LIndt	TLogger().indent()
#define LOdt	TLogger().outdent()
#define Log		TLogger()
#define ls(L)	<< (L)
#define LFn_	Log.begw(">") << __PRETTY_FUNCTION__ << "started on line" << TString::number((long)__LINE__)
#define LFn		(LFn_, LIndt)
#define LRet	return \
	((Log.begw("<") << __PRETTY_FUNCTION__ << "returned on line" << TString::number((long)__LINE__)),LOdt),
#define LVd		((Log.begw("<") << __PRETTY_FUNCTION__ << "returned on line" << TString::number((long)__LINE__)),LOdt);return
#define LVoid	LVd
#define LThw	(Log.begw("<") << __PRETTY_FUNCTION__ << "threw on line" << TString::number((long)__LINE__)), LOdt; throw
#define LCThw	(STMTS,EX) try { STMTS } catch( EX & ex ){LThw ex;}
#define Log1(MSG) TLogger() << MSG
#define Logp(MSG) TLogger() << MSG
#define LCtor(MSG) TLogger() << MSG
#else
#define LRst
#define LIndt
#define LOdt
#define Log
#define ls(L)
#define LFn_
#define LFn
#define LRet	return
#define LVd		return;
#define LVoid	LVd
#define LThw	throw
#define LCThw	(STMTS,EX) try { STMTS } catch( EX & ex ){throw ex;}
#define Log1(MSG)
#define Logp(MSG)
#define LCtor(MSG)
#endif // T_DBG (else)
#endif // T_LOGGER
