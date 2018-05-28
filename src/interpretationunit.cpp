#include "interpretationunit.h"

#include <QFile>

#include <iostream>

#include "tokenizer.h"
#include "parser.h"

using namespace mere;

InterpretationUnit:: InterpretationUnit(const QString& filename):
	m_filename(filename){
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	m_source = f.readAll();
	Tokenizer(this).scan_tokens();
	if (!success())
		return;
	Parser(this).parse();
	f.close();
}

InterpretationUnit::InterpretationUnit(QFile& file):
m_filename(file.fileName()){
	if (!file.isOpen())
		file.open(QIODevice::ReadOnly);
	m_source = file.readAll();
	Tokenizer(this).scan_tokens();
	if (issues.size())
		return;
	Parser(this).parse();
}

IntpUnit InterpretationUnit::from_source(const QString& src){
	IntpUnit unit = new InterpretationUnit;
	unit->m_filename = "src";
	unit->m_source = src;
	Tokenizer(unit).scan_tokens();
	Parser(unit).parse();
	return unit;
}

void InterpretationUnit::print_tokens() const {
	std::cout << m_toks.to_string("\n").toStdString() + "\n";
}

void InterpretationUnit::print_issues() const {
	std::cout << "  > Issues\n";
	if (!issues.size()){
		std::cout << "    > 0 issues recorded.\n";
		return;
	}

	QString error_text = "";
	for (uint i = 0u; i != issues.size(); i++){
		error_text.append(QString("    > ") + issues.at(i).msg);
		error_text.append("\n");
	}
	std::cout << error_text.toStdString();
}

void InterpretationUnit::report(const srcloc_t& loc, const QString& type, const QString& msg) {
	QString m = QString("%1:%2:%3: %4: %5")
				.arg(m_filename)
				.arg(QString::number(loc.line))
				.arg(QString::number(loc.col))
				.arg(type)
				.arg(msg);
	issues.push_back(m);
	//std::cout << " !> " << m << "\n";
}

void InterpretationUnit::report(const QString& type, const QString& msg) {
	QString m = QString("%1: %2: %3")
				.arg(m_filename)
				.arg(type)
				.arg(msg);
	issues.push_back(m);//TODO
	//std::cout << " !> " << m << "\n";
}
