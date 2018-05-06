//t_gen - 180506
#ifndef T_SPECS
#define T_SPECS
#if _DEBUG
#undef _DEBUG
#endif
#define T_QT 1
#define T_OSX 1
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
#define PROJECT ("Mere Interpreter")
#define VERSION ("v0.1")
#define AUTHOR ("TheOakCode")
#define EMAIL ("theoaktree1040@gmail.com")
#define DESCRIPTION ("An interpreter for the scripting language Mere.")
#define BUILD ("1 (180506)")
#define App QCoreApplication
#include <iostream>
#endif // T_SPECS