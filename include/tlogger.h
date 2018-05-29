#pragma once

#ifndef T_LOGGER
#define T_LOGGER
#include "config.h"

#if T_DBG
#include <QString>
#ifndef __GNUC__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

namespace mere {
	class TLogHelper{
		private:
			static int indentation;
		public:
			static void indent(){ indentation++; }
			static void outdent(){ indentation--; }
			static QString startln(const QString& starter);
			static void reset(){ indentation = 0; }
	};

	class TLogger{
		private:
			QString out = "";
			QString st = "|";
			bool noln = false;
		public:
			TLogger(){ }

			TLogger& put(const QString& s){ out += s + " "; return *this; }

			template<typename Num>
			TLogger& operator<<(const Num& d) { return put(QString::number(d)); }

			TLogger& operator<<(const char str[]){ return put(QString(str)); }

			TLogger& indent(){ TLogHelper::indent(); return *this; }
			TLogger& outdent(){ TLogHelper::outdent(); return *this; }
			TLogger& noline(){ noln = true; return *this; }
			TLogger& begw(const QString& str){ st = str; return *this; }

			~TLogger();
	};

	template<> TLogger& TLogger::operator<<(const QString& s);
	template<> TLogger& TLogger::operator<<(const char& ch);
}
#define LRstImpl	TLogHelper::reset()
#define LIndtImpl	TLogger().indent()
#define LOdtImpl	TLogger().outdent()
#define LogImpl		TLogger()
#define lsImpl(L)	<< (L)
#define LFn_Impl	LogImpl.begw(">") << __PRETTY_FUNCTION__ << "started on line" << QString::number((long)__LINE__)
#define LFnImpl		(LFn_Impl, LIndtImpl)
#define LRetImpl	return \
	((LogImpl.begw("<") << __PRETTY_FUNCTION__ << "returned on line" << QString::number((long)__LINE__)),LOdtImpl),
#define LVdImpl		((LogImpl.begw("<") << __PRETTY_FUNCTION__ << "returned on line" << QString::number((long)__LINE__)),LOdtImpl);return
#define LVoidImpl	LVdImpl
#define LThwImpl	(LogImpl.begw("<") << __PRETTY_FUNCTION__ << "threw on line" << QString::number((long)__LINE__)), LOdtImpl; throw
#define LCThwImpl	(STMTS,EX) try { STMTS } catch( EX & ex ){LThw ex;}
#define Log1Impl(MSG) TLogger() << MSG
#define LogpImpl(MSG) TLogger() << MSG
#define LCtorImpl(MSG) TLogger() << MSG



#define LRst		LRstImpl
#define LIndt		LIndtImpl
#define LOdt		LOdtImpl
#define Log			LogImpl
#define ls(L)		lsImpl(L)
#define LFn_		LFn_Impl
#define LFn			LFnImpl
#define LRet		LRetImpl
#define LVd			LVdImpl
#define LVoid		LVoidImpl
#define LThw		LThwImpl
#define LCThw		LCThwImpl
#define Log1(MSG)	Log1Impl(MSG)
#define Logp(MSG)	Logp(MSG)
#define LCtor(MSG)	LCtorImpl(MSG)
#else
#define LRst
#define LIndt
#define LOdt
#define Log
#define ls(L)
#define LFn_
#define LFn
#define LRet		return
#define LVd			return
#define LVoid		return
#define LThw		throw
#define LCThw		(STMTS,EX) try { STMTS } catch( EX & ex ){throw ex;}
#define Log1(MSG)
#define Logp(MSG)
#define LCtor(MSG)
#endif // T_DBG (else)
#endif // T_LOGGER
