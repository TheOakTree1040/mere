#ifndef MERE_MATH_H
#define MERE_MATH_H

#include "config.h"

#include <QFile>

#if T_GUI
#include <QMessageBox>
#include <QWidget>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#endif

#include "tokenizer.h"
#include "interpreter.h"
#include "parser.h"
#include "astprinter.h"
#include "runtimeerror.h"

#include <iostream>

using namespace mere;

Interpreter* Core::interpreter = nullptr;///< Note: Initialized in Core::init_once
bool Core::intd = false;///< \brief Whether Core was initialized

std::vector<Core::Error> Core::errors{};///< \brief It stores all the errors occurred

struct Core::Error{
		Error():loc(),msg(""){}
		Error(const errloc_t e, const QString& m):loc(e),msg(m){}
		errloc_t loc;///< \brief Location of error
		QString msg;///< \brief The error message
};


/**
 * \brief This function initializes the necessary resources.
 */
void Core::init_once(){
	LFn;
	if (intd){//Checks whether this function had been called
		LVd;
	}
	//Register the comparator otherwise the interpreter won't work
	if (!QMetaType::registerComparators<Object>()){
#if T_GUI
		QMessageBox::critical(nullptr,"Fatal Internal Failure","Failed to register Object comparators.");
#else
		std::cerr << "  > Fatal Internal Error: Failed to register Object comparators\n";
#endif
		Logp("Failed registering comparators.");
		abort();
	}
	interpreter = new Interpreter();
	intd = true;
	LVd;
}

/**
 * \param src The source.
 * \param show_tok Whether to show the tokens.
 * \param show_syn Whether to pretty-print the syntax tree
 * \returns whether there is an error during the process.
 * \brief A function that takes & interpret the source.
 */
bool Core::run(const QString& src, bool show_tok, bool show_syn){
	if (src.isEmpty())
		return false;
	Core::init_once();
	Stmts stmts;
	{
		Tokens tokens = Tokenizer(src).scan_tokens();
		if (show_tok){
#if T_GUI
			QMessageBox::information(nullptr,"",tokens.to_string());
#else
			std::cout << "  > " << tokens.to_string().toStdString() << std::endl;
#endif
		}
		if (errors.size()){
			show_errors();
			errors.clear();
			return false;
		}
		stmts = Parser(tokens).parse();
	}

	if (errors.size()){
		show_errors();
		errors.clear();
		return false;
	}

	if (show_syn){
		std::cout << "  > AST Printer broken.\n";
#if T_GUI && AST_PRINTER_FIXED
		QString str = ASTPrinter(stmts).AST();
		QWidget* wnd = new QWidget();
		QTextEdit* edt = new QTextEdit();
		QVBoxLayout* layout = new QVBoxLayout();

		QFont font;

		font.setFamily("Courier");
		font.setStyleHint(QFont::Monospace);
		font.setFixedPitch(true);
		font.setPointSize(16);
		edt->setText(str);
		edt->setFont(font);
		edt->setTabStopWidth(4*QFontMetrics(font).width(" "));
		layout->addWidget(edt);
		wnd->setLayout(layout);
		wnd->setAttribute(Qt::WA_DeleteOnClose);
		wnd->show();
#elif AST_PRINTER_FIXED
		std::cout << "  > " + ASTPrinter(stmts).AST().toStdString() + "\n";
#endif
	}
	auto res = interpreter->interpret(stmts);

	errors.clear();
	Logp("End of run");
	return res;
}

/**
 * \brief A function that reads a file & calls Core::run(const QString&,bool,bool).
 * \param file The file to read (must be opened before calling)
 * \see mere::Core::run(const QString&, bool, bool)
 */
bool Core::run(QFile & file){
	return run(file.readAll());
}

/**
 * \brief A function used to report an error on a line.
 * \param loc The line on which the error ocurred.
 * \param msg The error message.
 */
void Core::error(const errloc_t& loc, const QString& msg){
	report(loc, QString("Error: ") + msg);
}

/**
 * \brief A function used to report an error w/o line #. (Used during interpretation)
 * \param msg The error message.
 */
void Core::error(const QString & msg){
	errors.push_back(Error(errloc_t(),msg));//TODO
#if T_GUI
	QMessageBox::critical(nullptr, "Error", msg);
#else
	std::cout << " x> " << msg.toStdString() << "\n";
#endif
}

/**
 * \brief A function that adds the given error to the \c errors vector.
 * \param loc The source location.
 * \param msg The error message.
 */
void Core::report(const errloc_t& loc, const QString& msg){
	std::cout << " x> ERROR generated.\n";
	QString p = QString("[%1:%2]: %3")
				.arg(QString::number(loc.line))
				.arg(QString::number(loc.col))
				.arg(msg);
	errors.push_back(Error(loc,p));
#if T_GUI
	QMessageBox::critical(nullptr,"Error",p);
#endif
}

/**
 * \brief This function adds a \c RuntimeError to \c errors.
 * \param re The RuntimeError that was generated.
 */
void Core::runtime_error(const RuntimeError &re){
	QString str = "";
	str.append("[Ln ").append(QString::number(re.tok.line())).append("] ").append(re.msg);
	errors.push_back(Error(errloc_t(re.tok.loc(),0),re.msg));
#if T_GUI
	QMessageBox::critical(nullptr,"Runtime Error",str);
#else
	std::cout << "  > Runtime Error: " << re.msg.toStdString() + "\n";
#endif
}

/**
 * \brief This function shows the errors that had been recorded.
 */
void Core::show_errors(){
	std::cout << "  > Errors\n";
	if (!errors.size()){
#if T_GUI
		QMessageBox::information(nullptr,"Info","0 error recorded.");
#else
		std::cout << "    > 0 error recorded.\n";
#endif
		return;
	}

	QString error_text = "";
	for (uint i = 0u; i != errors.size(); i++){
		error_text.append(QString("    > ") + errors.at(i).msg);
		error_text.append("\n");
	}
#if T_GUI
	QWidget* wind = new QWidget();
	QTextEdit* edt = new QTextEdit();
	QVBoxLayout* vlay = new QVBoxLayout();

	edt->setPlainText(error_text);
	vlay->addWidget(edt);
	wind->setLayout(vlay);
	wind->setAttribute(Qt::WA_DeleteOnClose);
	wind->show();
#else
	std::cout << error_text.toStdString() << std::endl;
#endif
}

/**
 * \brief This function resets the interpreter.
 * \see mere::Interpreter::reset(mere::EnvImpl*)
 */
void Core::reset_intp(){
	interpreter->reset();
}


#endif // MERE_MATH_H
