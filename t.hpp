#ifndef T_SPECS
#define T_SPECS

/**
  * @author TheOakCode
  * @brief This file is a convenience file
  * for development under different frameworks and platforms
  */

//DEBUG - whether to display debug output.

#ifndef _DEBUG
//#define _DEBUG 0
#endif

#define T_FW_QT				0x01
#define T_FW_MSVS			0x02
#define T_FW_NONE			0x03

#define T_PF_WIN			0x11
#define T_PF_OSX			0x12

#define T_UNDER_FW(FW_NAME) T_FW	== FW_NAME
#define T_UNDER_PF(PF_NAME)	T_PF	== PF_NAME
//>>environment
//framework
#define T_FW				T_FW_QT
//platform
#ifdef _WIN32
# define T_PF				T_PF_WIN
#else
# define T_PF				T_PF_OSX
#endif
#ifndef __GNUC__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
#if T_FW != T_FW_QT
template <typename char_t>
class t_raw_char {
	private:
	char_t* chref = nullptr;
	bool del = false;
	void destruct() {
		if (del) {
			delete chref;
		}
		del = false;
		chref = nullptr;
	}
	void construct(char_t val = '\0') {
		destruct();
		chref = new char_t(val);
		del = true;
	}

	public:
	t_raw_char(const char_t& tch) {
		from(tch);
	}
	t_raw_char(char_t c) {
		from(c);
	}
	~t_raw_char() {
		destruct();
	}

	operator char_t() {
		return *chref;
	}
	void from(const t_raw_char& tch) {
		destruct();
		tch.del = false;
		chref = tch.chref;
	}
	void from(char_t ch) {
		if (del) {
			*chref = ch;
		}
		else {
			construct(ch);
		}
	}
	t_raw_char& operator=(char_t ch) {
		from(ch);
		return *this;
	}
	bool operator==(char_t ch) const {
		return *chref == ch;
	}
	bool operator==(const t_raw_char& tch) const {
		return *chref == *(tch.chref);
	}
	bool operator!=(char_t ch) const {
		return *chref != ch;
	}
	bool operator!=(const t_raw_char& tch) const {
		return *chref != *(tch.chref);
	}

};

typedef t_raw_char<char> tchar;
typedef t_raw_char<wchar_t> twchar;
#endif // T_FW != T_FW_QT

#if T_UNDER_FW(T_FW_QT)
# include <QString>
typedef QString TBuiltinString;
#else
# include <string>
typedef std::basic_string<TChar> TBuiltinString;
typedef unsigned int uint;
#endif // T_UNDER_FW(T_FW_QT)

#if T_FW != T_FW_QT // TString Impl. on non-Qt environment. IT DOESN'T WORK!
#define T_STRING
class TString : public TBuiltinString {
	public:
	TString() :TBuiltinString() {}
	TString(const TBuiltinString& tbs) :TBuiltinString(tbs) {}
	TString(const char str[]) :TBuiltinString(str) {}
	TString(const char ch) :TBuiltinString(ch) {}
	int to_int(bool *ok = nullptr, int base = 10) const {
		#if T_UNDER_FW(T_FW_QT)
		return toInt(ok, base);
		#else
		if (!ok)*ok = true;
		return std::stoi(*this, 0, base);
		#endif
	}
	double to_double(bool *ok = nullptr) const {
		#if T_UNDER_FW(T_FW_QT)
		return TBuiltinString::toDouble(ok);
		#else
		if (!ok)*ok = true;
		return std::stod(*this);
		#endif
	}
	uint to_uint(bool *ok = nullptr, int base = 10) const {
		#if T_UNDER_FW(T_FW_QT)
		return toUInt(ok, base);
		#else
		if (!ok)*ok = true;
		return static_cast<uint>(std::stoi(*this, 0, base));
		#endif
	}
	float to_float(bool *ok = nullptr) const {
		#if T_UNDER_FW(T_FW_QT)
		return toFloat(ok);
		#else
		if (!ok)*ok = true;
		return std::stof(*this);
		#endif
	}
	#if T_UNDER_FW(T_FW_QT)
	operator std::string() {
		return toStdString();
	}

	#endif
	#if !T_UNDER_FW(T_FW_QT)
	TString& append(const TString& str) const {
		return (*this) += str;
	}
	char& at(int i) const {
		return (*this)[i];
	}
	template<typename Num> static
		TString number(Num n) {
		return std::string(std::to_string(n));
	}
	#endif
};
#if T_UNDER_FW(T_FW_QT)
# include <QMetaType>
Q_DECLARE_METATYPE(TString)
#endif
#else
typedef QString TString;
#endif // T_FW != T_FW_QT

#define OUT_STR_TY // DEPRECATE
#define COUT_COMPAT // DEPRECATE

#if T_UNDER_FW(T_FW_QT)
# define to_std_string(QSTR) QSTR.toStdString()
#else
# define to_std_string(STDSTR) STDSTR
#endif // T_UNDER_FW(T_FW_QT)

#define t_cast static_cast

//===PROJECT SPECIFIC MACROS===

#define T_UI_GUI 0x21
#define T_UI_CLI 0x22

//UI Configuration.
#define T_UI_Conf T_UI_CLI
#define IS_GUI_APP (T_UI_Conf == T_UI_GUI)
#define IS_CLI_APP (T_UI_Conf == T_UI_CLI)

#define PROJECT		("Mere Interpreter")
#define VERSION		("v0.1")
#define AUTHOR		("TheOakCode")
#define EMAIL		("theoaktree1040@gmail.com")
#define DESCRIPTION	("An interpreter for the scripting language Mere.")

#if IS_GUI_APP
#define App QApplication
#else
#define App QCoreApplication
#endif // T_UI_Conf == T_UI_GUI

#if IS_CLI_APP
#include <iostream>
#endif // IS_CLI_APP
//===END PROJECT SPECIFIC MACROS===

#endif // T_SPECS
