#ifndef PROJECT_H
#define PROJECT_H

#define PROJ_NAME		"cmm"
#define PROJ_VER		"0.0.1"
#ifdef _WIN32
#	define FILE_NAME	"cmm.exe"
#else
#	define FILE_NAME	"cmm"
#endif

#define VERSION			"0.0.1"
#define DATE			"04-03-18"

/** [Contents]
  * 1. Project Notation
  * 2. Major Events
  * 3. Known Bugs
  * 4. Usage
  *		Cmd opts
  *
  */

/** 1 [Project Notation]
  * @name		cmm / C++ Mere Math
  * @author		Zhenkai Weng
  * @version	As in macro VERSION
  * @date		As in macro DATE
  * @brief		An interpreter for MereMath
  */

/** 2 [Major Events]
  *
  *  Date	Type    Details
  *
  * 04-01	Event	Project file created.
  *					v0.0.1
  *
  *
  */

/** 3 [Known bugs]
  *  Date	Level	Details
  *
  */

/** 4 [Usage]
	* Command line options
		* -f=<filename>	source
		* -editor		opens source editor
		* -mode=<mode>	sets mode {"i":"interpret","mmx":"make-mere-exec","p":"parse(-only, displays syntax tree)"}
  *
  */
#endif // PROJECT_H
