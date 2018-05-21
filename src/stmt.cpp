#include "stmt.h"
#include "token.h"

using namespace mere;

Branch::Branch():
	m_expr(nullptr),
	m_stmt(nullptr){}

Branch::Branch(const Expr& ex, const Stmt& st):
	m_expr(new Expr(ex)),
	m_stmt(new Stmt(st)){}

Branch::Branch(const Branch& other):
	m_expr(other.m_expr),
	m_stmt(other.m_stmt){
	other.m_handled = true;
}

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

Stmt&Stmt::operator*=(const Stmt& rhs){
	this->m_handled = rhs.m_handled;
	rhs.set_handled();
	this->m_type = rhs.m_type;
	this->m_fields = rhs.m_fields;
	return *this;
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

// ================== Stmt::var_decl_fields ==================
Stmt::var_decl_fields::var_decl_fields(const Expr& ini, const Token& name):
	m_init(new Expr(ini)),
	m_var_name(new Token(name)){
}

Stmt::var_decl_fields::~var_decl_fields(){
	delete m_init;
	delete m_var_name;
}

void Stmt::var_decl_fields::set_init(const Expr& init){
	delete m_init;
	m_init = new Expr (init);
}

void Stmt::var_decl_fields::set_name(const Token& tok){
	delete m_var_name;
	m_var_name = new Token(tok);
}

// ================== Stmt::if_stmt_fields ==================
Stmt::if_stmt_fields::if_stmt_fields(const Expr& condit, const Stmt& if_blk, const Stmt& else_blk):
	m_condition(new Expr(condit)),
	m_if_block(new Stmt(if_blk)),
	m_else_block(new Stmt(else_blk)){}

Stmt::if_stmt_fields::~if_stmt_fields() {
	delete m_condition;
	delete m_if_block;
	delete m_else_block;
}

void Stmt::if_stmt_fields::set_condition(const Expr& c){
	delete m_condition;
	m_condition = new Expr(c);
}

void Stmt::if_stmt_fields::set_if(const Stmt& i){
	delete m_if_block;
	m_if_block = new Stmt(i);
}

void Stmt::if_stmt_fields::set_else(const Stmt& e){
	delete m_else_block;
	m_else_block = new Stmt(e);
}

// ================== Stmt::while_stmt_fields ==================

Stmt::while_stmt_fields::while_stmt_fields(const Expr& condit, const Stmt& block):
	m_condition(new Expr(condit)),
	m_block(new Stmt(block)){}

Stmt::while_stmt_fields::~while_stmt_fields() {
	delete m_condition;
	delete m_block;
}

void Stmt::while_stmt_fields::set_condition(const Expr& condit){
	delete m_condition;
	m_condition = new Expr(condit);
}

void Stmt::while_stmt_fields::set_block(const Stmt& block){
	delete m_block;
	m_block = new Stmt(block);
}

// ================== Stmt::block_fields ==================
Stmt::block_fields::block_fields(const std::vector<Stmt>& b):
	m_block(nullptr){
	set_block(b);
}

Stmt::block_fields::~block_fields() {
	int s = m_block->size();
	for (int i = 0; i != s; i++){
		delete (*m_block)[i];
	}
	delete m_block;
}

std::vector<Ref<Stmt> > Stmt::block_fields::block() const {
	int size = m_block->size();
	std::vector<Ref<Stmt>> stmts;
	for (int i = 0; i != size; i++){
		stmts.push_back(Ref<Stmt>(*((*m_block)[i])));
	}
	return stmts;
}

void Stmt::block_fields::set_block(const std::vector<Stmt>& b){
	delete m_block;
	int size = b.size();
	m_block = new std::vector<sptr>(size);
	std::vector<sptr>& m_blk = *m_block;
	for (int i = 0; i != size; i++){
		m_blk[i] = new Stmt(b[i]);
	}
}

// ================== Stmt::expr_fields ==================
void Stmt::expr_fields::set_expr(const Expr& expr){
	delete m_expr;
	m_expr = new Expr(expr);
}

// ================== Stmt::fn_decl_fields ==================
Stmt::fn_decl_fields::fn_decl_fields(const Token& n, const std::vector<Token>& p, const Stmt& b):
	m_name(new Token(n)),
	m_params(nullptr),
	m_body(new Stmt(b)){
	set_params(p);
}

Stmt::fn_decl_fields::~fn_decl_fields() {
	delete m_name;
	int s = m_params->size();
	for (int i = 0; i != s; i++){
		delete (*m_params)[i];
	}
	delete m_params;
	delete m_body;
}

std::vector<Ref<Token> > Stmt::fn_decl_fields::params() const {
	int size = m_params->size();
	std::vector<Ref<Token>> p;
	for (int i = 0; i != size; i++){
		p.push_back(Ref<Token>(*((*m_params)[i])));
	}
	return p;
}

void Stmt::fn_decl_fields::set_name(const Token& n){
	delete m_name;
	m_name = new Token(n);
}

void Stmt::fn_decl_fields::set_body(const Stmt& b){
	delete m_body;
	m_body = new Stmt(b);
}

void Stmt::fn_decl_fields::set_params(const std::vector<Token>& p){
	delete m_params;
	int size = p.size();
	m_params = new std::vector<Token*>(size);
	std::vector<Token*>& m_p = *m_params;
	for (int i = 0; i != size; i++){
		m_p[i] = new Token(p[i]);
	}
}

// ================== Stmt::ret_stmt_fields ==================
Stmt::ret_stmt_fields::ret_stmt_fields(const Expr& v, const Token& op):
	m_val(new Expr(v)),
	m_keyword(new Token(op)){}

Stmt::ret_stmt_fields::~ret_stmt_fields() {
	delete m_val;
	delete m_keyword;
}

void Stmt::ret_stmt_fields::set_value(const Expr& v){
	delete m_val;
	m_val = new Expr(v);
}

void Stmt::ret_stmt_fields::set_keyword(const Token& keywd){
	delete m_keyword;
	m_keyword = new Token(keywd);
}

// ================== Stmt::assert_stmt_fields ==================
Stmt::assert_stmt_fields::assert_stmt_fields(const Expr& val, int code, const TString& msg):
	m_value(new Expr(val)),
	m_code(code),
	m_msg(msg){}

void Stmt::assert_stmt_fields::set_value(const Expr& value){
	delete m_value;
	m_value = new Expr(value);
}

void Stmt::assert_stmt_fields::set_code(int code){
	m_code = code;
}

void Stmt::assert_stmt_fields::set_message(const TString& message){
	m_msg = message;
}

// ================== Stmt::match_stmt_fields ==================
Stmt::match_stmt_fields::match_stmt_fields(const Expr& match, const std::vector<Branch>& br):
	m_match(new Expr(match)),
	m_branches(new std::vector<Branch>(br)){}

Stmt::match_stmt_fields::~match_stmt_fields() {
	delete m_match;
	delete m_branches;
}

void Stmt::match_stmt_fields::set_match(const Expr& match){
	delete m_match;
	m_match = new Expr(match);
}

void Stmt::match_stmt_fields::set_branches(const std::vector<Branch>& br){
	delete m_branches;
	m_branches = new std::vector<Branch>(br);
}
