
#ifndef T_LOGGER
#define T_LOGGER
#include "config.hpp"

#if T_DBG
namespace mere {
	class TLogHelper{
		private_fields:
			static int indentation;
		public_methods:
			static void indent(){ indentation++; }
			static void outdent(){ indentation--; }
			static TString startln(const TString& starter);
			static void reset(){ indentation = 0; }
	};

	class TLogger{
		private_fields:
			TString out = "";
			TString st = "|";
			bool noln = false;
		public_methods:
			TLogger(){ }

			TLogger& put(const TString& s){ out += s + " "; return *this; }

			template<typename Num>
			TLogger& operator<<(const Num& d) { return put(TString::number(d)); }

			TLogger& operator<<(const char str[]){ return put(TString(str)); }

			TLogger& indent(){ TLogHelper::indent(); return *this; }
			TLogger& outdent(){ TLogHelper::outdent(); return *this; }
			TLogger& noline(){ noln = true; return *this; }
			TLogger& begw(const TString& str){ st = str; return *this; }

			~TLogger();
	};

	template<> TLogger& TLogger::operator<<(const TString& s);
	template<> TLogger& TLogger::operator<<(const char& ch);
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
