#ifndef T_SPECS
#define T_SPECS

/**
  * @author TheOakCode
  * @brief Configuration file for mere-interpreter
  */

#ifndef _DEBUG
//#define _DEBUG 0
#endif

#define T_QT				1
//#define T_MSVC			1
//#define T_NO_FW			1

//#define T_PF_WIN			1
#define T_PF_OSX			1

#ifdef _WIN32
# define T_WIN32 1
#else
# define T_OSX 1
#endif // _WIN32

#ifndef __GNUC__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#if T_QT
#include <QString>
typedef QString TString;
#else
#error Implementation for TString is required.
#endif // T_QT

#define t_cast static_cast

//===PROJECT SPECIFIC MACROS===

//#define T_GUI 1
#define T_CLI 1

#define PROJECT		("Mere Interpreter")
#define VERSION		("v0.1")
#define AUTHOR		("TheOakCode")
#define EMAIL		("theoaktree1040@gmail.com")
#define DESCRIPTION	("An interpreter for the scripting language Mere.")
#define BUILD		("1 (180506)")

#if T_GUI
#define App QApplication
#else
#define App QCoreApplication
#include <iostream>
#endif // T_GUI

#endif // T_SPECS
