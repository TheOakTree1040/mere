//t_gen - 180511
#ifndef T_SPECS
#define T_SPECS
#if _DEBUG
#undef _DEBUG
#endif
#define T_QT 1
#ifdef _WIN32
#define T_WIN32 1
#else
#define T_DARWIN 1
#endif
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
#define T_CLI 1
#define App QCoreApplication
#include <iostream>
#define PROJECT ("Mere Interpreter")
#define VERSION ("v0.1")
#define AUTHOR ("TheOakCode")
#define EMAIL ("theoaktree1040@gmail.com")
#define DESCRIPTION ("An interpreter for the scripting language Mere.")
#define BUILD ("4 (180511)")
#endif // T_SPECS