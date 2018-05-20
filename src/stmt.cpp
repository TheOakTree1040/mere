#include "stmt.h"

namespace mere {
	Branch::Branch(const Expr& ex, const Stmt& st):
		m_expr(new Expr(ex)),
		m_stmt(new Stmt(st)){}

	Branch::~Branch(){
		LFn;
		if(!m_handled){
			delete m_expr;
			delete m_stmt;
		}
		LVd;
	}

	Branch& Branch::operator=(const Branch& other){
		delete m_expr;
		delete m_stmt;
		m_expr = other.m_expr;
		m_stmt = other.m_stmt;
		other.m_handled = true;
		return *this;
	}

	bool Stmt::is_handled() const {
		return m_handled;
	}

	void Stmt::set_handled() const {
		m_handled = true;
	}

	Stmt::Stmt():
		m_type(Invalid),
		m_fields(nullptr){
		m_type = Invalid;
		set_handled();
	}

	Stmt::Stmt(Stmt::stmt_type t, Stmt::stmt_fields* fields):
		m_type(t),
		m_fields(fields){}

	Stmt::Stmt(const Stmt& other):
		m_type(other.type()),
		m_fields(other.data()){
		other.set_handled();
	}

	Stmt::~Stmt(){
		//LFn;
		handle();
		//LVd;
	}

	void Stmt::handle() const {
		//LFn;
		if(!m_handled){
			delete m_fields;
			m_handled = true;
			m_fields = nullptr;
		}
		//LVd;
	}
}
