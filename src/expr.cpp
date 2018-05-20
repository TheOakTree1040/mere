#include "expr.h"
#include "stmt.h"
#include "merecallable.h"
using namespace mere;
Expr::~Expr(){
	//LFn;
	handle();
	//LVd;
}
void Expr::handle() const {
	//LFn;
	if (!m_handled){
		delete m_fields;
		m_handled = true;
		m_fields = nullptr;
	}
	//LVd;
}
