#include "interpretationunit.h"

#include <QFile>

#include <iostream>

#include "tokenizer.h"
#include "parser.h"

using namespace mere;

bool InterpretationUnit::_show_snippet = false;

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
	if (!issues.size()){ return; }
	std::cout << "  > " << issues.size() << " issue";
	std::cout << (issues.size() == 1? "\n":"s\n");

	QString error_text = "";
	for (uint i = 0u; i != issues.size(); i++) {
		const Issue& issue = issues[i];
		QString str = QString("    > (%1) ").arg(i+1);
		int indent = str.size();
		error_text.append(
					(str + QString("%1\n%2\n"))
					.arg(issue.msg)
					.arg(snippet_at(indent,issue.loc))
					);
	}
	std::cout << error_text.toStdString();
}

QString InterpretationUnit::snippet_at(int indent, const srcloc_t& loc) const {
	LFn;
	using std::cout;

	if (loc.line == 0 || !_show_snippet || m_source.isEmpty())
		return "";
	QString indent_str = "";
	QString string =  "";
	for (int i = 0; i != indent; i++)
		indent_str += " ";
	string += indent_str + "snippet: ";
	int bound = m_line_indices[loc.line + 1] - 1;
	for (uint i = m_line_indices[loc.line]; i <= bound; i++){
		string += m_source[i];
	}

	if (!string.endsWith('\n')) string += "\n";
	string += indent_str + "         ";
	for (uint i = 1u; i != loc.col; i++){
		string += ' ';
	}
	string += "^";
	LRet string;
}

void InterpretationUnit::report(const srcloc_t& loc, const QString& type, const QString& msg) {
	QString m = QString("%1:%2:%3: %4: %5")
				.arg(m_filename)
				.arg(QString::number(loc.line))
				.arg(QString::number(loc.col))
				.arg(type)
				.arg(msg);
	issues.push_back({loc,m});
	//std::cout << " !> " << m << "\n";
}

void InterpretationUnit::report(const QString& type, const QString& msg) {
	QString m = QString("%1: %2: %3")
				.arg(m_filename)
				.arg(type)
				.arg(msg);
	issues.push_back({srcloc_t(0,0),m});//TODO
	//std::cout << " !> " << m << "\n";
}
