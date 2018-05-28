
#include <vector>

#include "stmt.h"
#include "merecallable.h"
#include "token.h"
#include "object.h"

using namespace mere;

// ================== Expr ==================
Expr::~Expr(){
	//LFn;
	handle();
	//LVd;
}

Expr& Expr::operator*=(const Expr& rhs){
	this->m_handled = rhs.m_handled;
	rhs.set_handled();
	this->m_type = rhs.m_type;
	this->m_fields = rhs.m_fields;
	return *this;
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

Expr::Expr():
	m_type(Empty),
	m_fields(new empty_fields){}

Expr::Expr(const Expr& expr):
	m_type(expr.type()),
	m_fields(expr.data()){
	expr.set_handled();
}

Expr::Expr(Expr::expr_type t, Expr::expr_fields* f):
	m_type(t),
	m_fields(f){}

// ================== Expr::unary_fields ==================
Expr::unary_fields::unary_fields(const Expr& operand, const Token& op):
	nested_expr_fields(operand),
	operator_fields(op){}

Expr::binary_fields::binary_fields(const Expr& left, const Token& op, const Expr& right):
	unary_fields(left,op),
	m_right(new Expr(right)){}

// ================== Expr::binary_fields ==================
Expr::binary_fields::~binary_fields() {
	delete m_right;
}

Expr& Expr::binary_fields::left(){
	return expr();
}

Expr& Expr::binary_fields::right(){
	return *m_right;
}

void Expr::binary_fields::set_left(const Expr& left){
	set_expr(left);
}

void Expr::binary_fields::set_right(const Expr& right){
	delete m_right;
	m_right = new Expr(right);
}

// ================== Expr::nested_expr_fields ==================
Expr::nested_expr_fields::nested_expr_fields(const Expr& ex):m_expr(new Expr(ex)){}

Expr::nested_expr_fields::~nested_expr_fields() {
	delete m_expr;
}

Expr& Expr::nested_expr_fields::expr() const {
	return *m_expr;
}

void Expr::nested_expr_fields::set_expr(const Expr& ex){
	delete m_expr;
	m_expr = new Expr(ex);
}

// ================== Expr::operator_fields ==================
Expr::operator_fields::operator_fields(const Token& op):m_op(new Token(op)){}

Expr::operator_fields::~operator_fields() {
	delete m_op;
}

Token& Expr::operator_fields::op() const {
	return *m_op;
}

void Expr::operator_fields::set_op(const Token& o){
	delete m_op;
	m_op = new Token(o);
}

// ================== Expr::array_fields ==================

Expr::array_fields::array_fields(const std::vector<Expr>& arr):
	m_array(nullptr){
	set_array(arr);
}

Expr::array_fields::~array_fields() {
	int s = m_array->size();
	for (int i = 0; i != s; i++){
		delete (*m_array)[i];
	}
	delete m_array;
}

std::vector<Ref<Expr> > Expr::array_fields::array() const {
	int size = m_array->size();
	std::vector<Ref<Expr>> ret;
	for (int i = 0; i != size; i++){
		ret.push_back(Ref<Expr>(*((*m_array)[i])));
	}
	return ret;
}

void Expr::array_fields::set_array(const std::vector<Expr>& arr){
	delete m_array;
	int size = arr.size();
	m_array = new std::vector<eptr>(size);
	std::vector<eptr>& m_arr = *m_array;
	for (int i = 0; i != size; i++){
		m_arr[i] = new Expr(arr[i]);
	}
}

// ================== Expr::pair_vector_fields ==================

Expr::pair_vector_fields::pair_vector_fields(const std::vector<std::pair<Expr, Expr> >& dat):
	m_data(nullptr){
	set_data(dat);
}

Expr::pair_vector_fields::~pair_vector_fields() {
	int size = m_data->size();
	for (int i = 0; i != size; i++){
		std::pair<eptr,eptr>& p = (*m_data)[i];
		delete p.first;
		delete p.second;
	}
	delete m_data;
}

void Expr::pair_vector_fields::set_data(const std::vector<std::pair<Expr, Expr>>& dat){
	delete m_data;
	int size = dat.size();
	m_data = new Data;
	Data& m_d = *m_data;
	for (int i = 0; i != size; i++){
		const std::pair<Expr,Expr>& p = dat[i];
		m_d.push_back(std::pair<eptr,eptr>(new Expr(p.first),new Expr(p.second)));
	}
}

std::vector<std::pair<Ref<Expr>, Ref<Expr>>> Expr::pair_vector_fields::data() const {
	int size = m_data->size();
	std::vector<std::pair<Ref<Expr>,Ref<Expr>>> ret;
	for (int i = 0; i != size; i++){
		std::pair<eptr,eptr>& p = (*m_data)[i];
		ret.push_back(std::pair<Ref<Expr>,Ref<Expr>>(*p.first,*p.second));
	}
	return ret;
}

// ================== Expr::lit_fields ==================
Expr::lit_fields::lit_fields(const Object& lit) : m_lit(new Object(lit)){}

Expr::lit_fields::~lit_fields() { delete m_lit; }

void Expr::lit_fields::set_lit(const Object& p_lit){
	delete m_lit;
	m_lit = new Object(p_lit);
}

// ================== Expr::call_fields ==================
Expr::call_fields::call_fields(const Expr& callee, const std::vector<Expr>& args, const Token& paren):
	operator_fields(paren),
	m_callee(new Expr(callee)),
	m_arguments(nullptr){
	set_args(args);
}

Expr::call_fields::~call_fields() {
	delete m_callee;
	int s = m_arguments->size();
	for (int i = 0; i != s; i++){
		delete (*m_arguments)[i];
	}
	delete m_arguments;
}

Expr& Expr::call_fields::callee() const{
	return *m_callee;
}

std::vector<Ref<Expr> > Expr::call_fields::arguments() const{
	int size = m_arguments->size();
	std::vector<Ref<Expr>> ret;
	for (int i = 0; i != size; i++){
		ret.push_back(Ref<Expr>(*((*m_arguments)[i])));
	}
	return ret;
}

void Expr::call_fields::set_callee(const Expr& cal){
	delete m_callee;
	m_callee = new Expr(cal);
}

void Expr::call_fields::set_args(const std::vector<Expr>& args){
	delete m_arguments;
	int size = args.size();
	m_arguments = new std::vector<eptr>(size);
	std::vector<eptr>& m_args = *m_arguments;
	for (int i = 0; i != size; i++){
		m_args[i] = new Expr(args[i]);
	}
}

// ================== Expr::ternary_fields ==================
Expr::ternary_fields::~ternary_fields() {
	delete m_condition;
	delete m_left;
	delete m_right;
}

void Expr::ternary_fields::set_condition(const Expr& c){
	delete m_condition;
	m_condition = new Expr(c);
}

void Expr::ternary_fields::set_left(const Expr& lft){
	delete m_left;
	m_left = new Expr(lft);
}

void Expr::ternary_fields::set_right(const Expr& rt){
	delete m_right;
	m_right = new Expr(rt);
}

// ================== Expr::cs_fields ==================
Expr::cs_fields::cs_fields(const std::vector<Expr>& cs_exprs):
	m_exprs(nullptr){
	set_exprs(cs_exprs);
}

Expr::cs_fields::~cs_fields() {
	int s = m_exprs->size();
	for (int i = 0; i != s; i++){
		delete (*m_exprs)[i];
	}
	delete m_exprs;
}

std::vector<Ref<Expr> > Expr::cs_fields::exprs(){
	int size = m_exprs->size();
	std::vector<Ref<Expr>> ret;
	for (int i = 0; i != size; i++){
		ret.push_back(Ref<Expr>(*((*m_exprs)[i])));
	}
	return ret;
}

void Expr::cs_fields::set_exprs(const std::vector<Expr>& cs_exprs){
	delete m_exprs;
	int size = cs_exprs.size();
	m_exprs = new std::vector<eptr>(size);
	std::vector<eptr>& m_ex = *m_exprs;
	for (int i = 0; i != size; i++){
		m_ex[i] = new Expr(cs_exprs[i]);
	}
}

// ================== Expr::lambda_fields ==================
Expr::lambda_fields::~lambda_fields(){ delete m_callable; }

// ================== Expr::var_acsr_fields ==================
Expr::var_acsr_fields::var_acsr_fields(const Token& acsr) :
	m_var_acsr(new Token(acsr)){}

Expr::var_acsr_fields::~var_acsr_fields() {
	delete m_var_acsr;
}

void Expr::var_acsr_fields::set_accessor(const Token& tok){
	delete m_var_acsr;
	m_var_acsr = new Token(tok);
}
