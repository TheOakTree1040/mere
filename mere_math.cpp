#ifndef MERE_MATH_H
#define MERE_MATH_H
#include <QFile>
#include <QVector>
#include <QMessageBox>
#include <QWidget>
#include <QTextEdit>
#include "tokenizer.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "interpreter.h"
#include "parser.h"
#include "astprinter.hpp"
#include "natives.h"

Interpreter* MereMath::interpreter = nullptr;

QVector<MereMath::Error> MereMath::errors{};

void MereMath::init_once(){
	LFn;
	if (!QMetaType::registerComparators<Object>()){
		QMessageBox::critical(nullptr,"Fatal Internal Failure","Failed to register Object comparators.");
#if _DEBUG
        Log << "Failed registering comparators.";
#endif
		abort();
    }
	interpreter = new Interpreter();
	LVd;
}

void MereMath::run(const TString& src, bool show_tok, bool show_syn){
//	Token* tok = new Token();
//	delete tok;
	Stmts stmts;
	{
		Tokens tokens = Tokenizer(src).scan_tokens();
		if (show_tok){
			TString str = "";
			int size = tokens.size();
			for (int i = 0; i != size; i++){
				str.append(tokens[i].to_string());
			}
			QMessageBox::information(nullptr,"",str);
		}
		if (errors.size()){
			show_errors();
			errors.clear();
			return;
		}
		stmts = Parser(tokens).parse();
	}
	if (errors.size()){
		show_errors();
		errors.clear();
		return;
	}
	if (show_syn){
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
	}
    interpreter->interpret(stmts);
	errors.clear();
#if _DEBUG
	Log << "End of run";
#endif
}

void MereMath::run_file(QFile & file){
	run(TBuiltinString(file.readAll()));
}

void MereMath::error(int ln, const TString& msg){
	report(ln, "", TString("Error: ") + msg);
}

void MereMath::error(const Token& tok, const TString& msg){
	if (tok.ty == Tok::END) {
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
	if (!p.endsWith("."))
		p.append(".");
	errors.append(Error(ln,p));
	QMessageBox::critical(nullptr,"Error",p);
}

void MereMath::runtime_error(const RuntimeError &re){
	TString str = "";
	str.append("[Ln ").append(TString::number(re.tok.ln+1)).append("] ").append(re.msg);
	errors.append(Error(re.tok.ln+1,re.msg));
	QMessageBox::critical(nullptr,"Runtime Error",str);
}

void MereMath::show_errors(){
	if (!errors.size()){
		QMessageBox::information(nullptr,"Info","0 error recorded.");
	}

	TString error_text = "";
	for (int i = 0; i != errors.size(); i++){
		error_text.append("<===========Error===========>\n");
		error_text.append(errors.at(i).msg);
		error_text.append("\n");
	}

	QWidget* wind = new QWidget();
	QTextEdit* edt = new QTextEdit();
	QVBoxLayout* vlay = new QVBoxLayout();

	edt->setPlainText(error_text);
	vlay->addWidget(edt);
	wind->setLayout(vlay);
	wind->setAttribute(Qt::WA_DeleteOnClose);
	wind->show();
}

void MereMath::reset_intp(){
	interpreter->reset();
}

#endif // MERE_MATH_H
