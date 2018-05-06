#ifndef MERE_MATH_H
#define MERE_MATH_H

#include <QFile>
#include <QVector>

#if IS_GUI_APP
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
#include "astprinter.hpp"
#include "natives.h"

Interpreter* MereMath::interpreter = nullptr;

QVector<MereMath::Error> MereMath::errors{};

void MereMath::init_once(){
	LFn;
	if (!QMetaType::registerComparators<Object>()){
#if IS_GUI_APP
		QMessageBox::critical(nullptr,"Fatal Internal Failure","Failed to register Object comparators.");
#else
		std::cout << "  > Fatal Internal Error: Failed to register Object comparators\n";
#endif
#if _DEBUG
        Log << "Failed registering comparators.";
#endif
		abort();
    }
	interpreter = new Interpreter();
	LVd;
}

bool MereMath::run(const TString& src, bool show_tok, bool show_syn){
//	Token* tok = new Token();
//	delete tok;
	if (src.isEmpty())
		return false;
	Stmts stmts;
	{
		Tokens tokens = Tokenizer(src).scan_tokens();
		if (show_tok){
			TString str = "";
			int size = tokens.size();
			for (int i = 0; i != size; i++){
				str.append(tokens[i].to_string());
			}
#if IS_GUI_APP
			QMessageBox::information(nullptr,"",str);
#else
			std::cout << "  > " << str.toStdString() << std::endl;
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
#if IS_GUI_APP
		Log1("Showing Syntax tree");
		TString str = ASTPrinter(stmts).AST();
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
#else
		std::cout << "  > " + ASTPrinter(stmts).AST().toStdString() + "\n";
#endif
	}
	auto res = interpreter->interpret(stmts);

	errors.clear();
#if _DEBUG
	Log << "End of run";
#endif
	return res;
}

bool MereMath::run_file(QFile & file){
	return run(TBuiltinString(file.readAll()));
}

void MereMath::error(int ln, const TString& msg){
	report(ln, "", TString("Error: ") + msg);
}

void MereMath::error(const TString & msg){
	errors.append(Error(-1,msg));
#if IS_GUI_APP
	QMessageBox::critical(nullptr, "Error", msg);
#else
	std::cout << "  > " << msg.toStdString() << "\n";
#endif
}

void MereMath::error(const Token& tok, const TString& msg){
	if (tok.ty == Tok::_eof_) {
		report(tok.ln, "at end", msg);
	}
	else {
		report(tok.ln, (TString)"at '" + tok.lexeme + "'", msg);
	}
}

void MereMath::report(int ln, const TString& loc, const TString& msg, bool is_idx){
	if (is_idx)
		ln++;
	TString p = TString("[Ln ").append(TString::number(ln))
				.append("] ").append(loc)
				.append(": ").append(msg);
	if (!p.endsWith('.'))
		p.append(".");
	errors.append(Error(ln,p));
#if IS_GUI_APP
	QMessageBox::critical(nullptr,"Error",p);
#endif
}

void MereMath::runtime_error(const RuntimeError &re){
	TString str = "";
	str.append("[Ln ").append(TString::number(re.tok.ln+1)).append("] ").append(re.msg);
	errors.append(Error(re.tok.ln+1,re.msg));
#if IS_GUI_APP
	QMessageBox::critical(nullptr,"Runtime Error",str);
#else
	std::cout << "  > Runtime Error: " << re.msg.toStdString() + "\n";
#endif
}

void MereMath::show_errors(){
	if (!errors.size()){
#if IS_GUI_APP
		QMessageBox::information(nullptr,"Info","0 error recorded.");
#else
		std::cout << "  > 0 error recorded.\n";
#endif
	}

	TString error_text = "\n";
	for (int i = 0; i != errors.size(); i++){
		error_text.append("Error:\n");
		error_text.append(TString("\t") + errors.at(i).msg);
		error_text.append("\n");
	}
#if IS_GUI_APP
	QWidget* wind = new QWidget();
	QTextEdit* edt = new QTextEdit();
	QVBoxLayout* vlay = new QVBoxLayout();

	edt->setPlainText(error_text);
	vlay->addWidget(edt);
	wind->setLayout(vlay);
	wind->setAttribute(Qt::WA_DeleteOnClose);
	wind->show();
#else
	std::cout << error_text.toStdString() /*<< std::endl*/;
#endif
}

void MereMath::reset_intp(){
	interpreter->reset();
}

#endif // MERE_MATH_H
