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

Interpreter* MereMath::interpreter = new Interpreter();

QVector<MereMath::Error> MereMath::errors{};

void MereMath::run(const QString& src){
//	Token* tok = new Token();
//	delete tok;
	Stmts stmts;
	{
		Tokens tokens = Tokenizer(src).scan_tokens();
		{
			QString str = "";
			int size = tokens.size();
			for (int i = 0; i != size; i++){
				str.append(tokens.at(i).to_string());
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

	QString str = ASTPrinter(stmts).AST();
	QMessageBox::information(nullptr,"AST Printer",str);
	//interpreter->interpret(stmts);
	for (int i = 0; i != stmts.size(); i++){
		delete stmts[i];
	}
	Log << "Clearing";
	errors.clear();
	Log << "End of run";
}

void MereMath::run_file(QFile & file){
	run(QString(file.readAll()));
}

void MereMath::error(int ln, const QString& msg){
	report(ln, "", QString("Error: ") + msg);
}

void MereMath::error(const Token& tok, const QString& msg){
	if (tok.ty == Tok::END) {
		report(tok.ln, "at end", msg);
	}
	else {
		report(tok.ln, (QString)"at '" + tok.lexeme + "'", msg);
	}
}

void MereMath::report(int ln, const QString& loc, const QString& msg, bool is_idx){
	if (is_idx)
		ln++;
	QString p = QString("[Ln ").append(QString::number(ln))
				.append("] ").append(loc)
				.append(": ").append(msg);
	if (!p.endsWith("."))
		p.append(".");
	errors.append(Error(ln,p));
	QMessageBox::critical(nullptr,"Error",p);
}

void MereMath::runtime_error(const RuntimeError &re){
	QString str = "";
	str.append("[Ln ").append(QString::number(re.tok.ln)).append("]").append(re.msg);
	errors.append(Error(re.tok.ln,re.msg));
	QMessageBox::critical(nullptr,"Runtime Error",str);
}

void MereMath::show_errors(){
	if (!errors.size()){
		QMessageBox::information(nullptr,"Info","0 error recorded.");
	}

	QString error_text = "";
	for (int i = 0; i != errors.size(); i++){
		error_text.append("<============Error============>\n");
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

#endif // MERE_MATH_H
