#ifndef STMT_H
#define STMT_H
#include "expr.h"
#include "utils.h"

namespace mere {
	//For MatchStmt
	struct Branch;

	class Stmt;
#define sptr Stmt*

	struct Branch {
		private:
			mutable bool m_handled = false;
			eptr m_expr;
			sptr m_stmt;
		public:
			Branch():
				m_expr(nullptr),
				m_stmt(nullptr){}
			Branch(const Expr& ex, const Stmt& st);
			Branch(const Branch& other):
				m_expr(other.m_expr),
				m_stmt(other.m_stmt){
				other.m_handled = true;
			}
			Branch& operator=(const Branch& other);

			~Branch();

			Expr& expr() const {
				return *m_expr;
			}
			Stmt& stmt() const {
				return *m_stmt;
			}
	};

	/*!
 * \brief The Stmt class
 */
	class Stmt{
			friend class Interpreter;
		public: // nested declarations
			/*!
			 * \brief The stmt_type enum
			 */
			enum stmt_type {
				Invalid,
				Empty,
				ExprStmt,
				VarDecl,
				Block,
				If,
				While,
				Print,
				Println,
				Function,
				Return,
				Assert,
				Match
			};
			class stmt_fields{
				private:
					stmt_fields& operator=(const stmt_fields&) = delete;
					stmt_fields(const stmt_fields&) = delete;
				protected:
					stmt_fields(){}
				public:
					virtual ~stmt_fields(){}
			};
			class var_decl_fields : public stmt_fields {
				private:
					eptr m_init;
					Token* m_var_name;
				public:
					var_decl_fields(const Expr& ini, const Token& name):
						m_init(new Expr(ini)),
						m_var_name(new Token(name)){
					}

					~var_decl_fields() override{
						delete m_init;
						delete m_var_name;
					}

					Expr& init(){
						return *m_init;
					}
					Token& name(){
						return *m_var_name;
					}
					void set_init(const Expr& init){
						delete m_init;
						m_init = new Expr (init);
					}
					void set_name(const Token& tok){
						delete m_var_name;
						m_var_name = new Token(tok);
					}
			};
			class if_stmt_fields : public stmt_fields {
				private:
					eptr m_condition;
					sptr m_if_block;
					sptr m_else_block;
				public:
					if_stmt_fields(const Expr& condit, const Stmt& if_blk, const Stmt& else_blk):
						m_condition(new Expr(condit)),
						m_if_block(new Stmt(if_blk)),
						m_else_block(new Stmt(else_blk)){}

					~if_stmt_fields() override {
						delete m_condition;
						delete m_if_block;
						delete m_else_block;
					}

					Expr& condition(){
						return *m_condition;
					}
					Stmt& if_block(){
						return *m_if_block;
					}
					Stmt& else_block(){
						return *m_else_block;
					}

					void set_condition(const Expr& c){
						delete m_condition;
						m_condition = new Expr(c);
					}
					void set_if(const Stmt& i){
						delete m_if_block;
						m_if_block = new Stmt(i);
					}
					void set_else(const Stmt& e){
						delete m_else_block;
						m_else_block = new Stmt(e);
					}
			};
			class while_stmt_fields : public stmt_fields {
				private:
					eptr m_condition;
					sptr m_block;
				public:
					while_stmt_fields(const Expr& condit, const Stmt& block):
						m_condition(new Expr(condit)),
						m_block(new Stmt(block)){}
					~while_stmt_fields() override {
						delete m_condition;
						delete m_block;
					}

					Expr& condition(){
						return *m_condition;
					}
					Stmt& block(){
						return *m_block;
					}

					void set_condition(const Expr& condit){
						delete m_condition;
						m_condition = new Expr(condit);
					}
					void set_block(const Stmt& block){
						delete m_block;
						m_block = new Stmt(block);
					}
			};
			class block_fields : public stmt_fields {
				private:
					std::vector<sptr>* m_block;
				public:
					block_fields(const std::vector<Stmt>& b):
						m_block(nullptr){
						set_block(b);
					}
					~block_fields() override {
						int s = m_block->size();
						for (int i = 0; i != s; i++){
							delete (*m_block)[i];
						}
						delete m_block;
					}

					std::vector<Ref<Stmt>> block(){
						int size = m_block->size();
						std::vector<Ref<Stmt>> stmts;
						for (int i = 0; i != size; i++){
							stmts.push_back(Ref<Stmt>(*((*m_block)[i])));
						}
						return stmts;
					}
					void set_block(const std::vector<Stmt>& b){
						delete m_block;
						int size = b.size();
						m_block = new std::vector<sptr>(size);
						std::vector<sptr>& m_blk = *m_block;
						for (int i = 0; i != size; i++){
							m_blk[i] = new Stmt(b[i]);
						}
					}
			};
			class expr_fields : public stmt_fields {
				private:
					eptr m_expr;
				public:
					expr_fields(const Expr& expr):
						m_expr(new Expr(expr)){}

					~expr_fields() override {
						delete m_expr;
					}

					Expr& expr(){
						return *m_expr;
					}
					void set_expr(const Expr& expr){
						delete m_expr;
						m_expr = new Expr(expr);
					}
			};
			class print_fields : public expr_fields {
				public:
					print_fields(const Expr& ex) : expr_fields(ex){}
			};
			class println_fields : public expr_fields {
				public:
					println_fields(const Expr& ex) : expr_fields(ex){}
			};
			class fn_decl_fields : public stmt_fields {
				private:
					Token* m_name;
					std::vector<Token*>* m_params;
					sptr m_body;
				public:
					fn_decl_fields(const Token& n, const std::vector<Token>& p, const Stmt& b):
						m_name(new Token(n)),
						m_params(nullptr),
						m_body(new Stmt(b)){
						set_params(p);
					}
					~fn_decl_fields() override {
						delete m_name;
						int s = m_params->size();
						for (int i = 0; i != s; i++){
							delete (*m_params)[i];
						}
						delete m_params;
						delete m_body;
					}

					Token& name(){
						return *m_name;
					}
					std::vector<Ref<Token>> params(){
						int size = m_params->size();
						std::vector<Ref<Token>> p;
						for (int i = 0; i != size; i++){
							p.push_back(Ref<Token>(*((*m_params)[i])));
						}
						return p;
					}
					Stmt& body(){
						return *m_body;
					}

					void set_name(const Token& n){
						delete m_name;
						m_name = new Token(n);
					}
					void set_body(const Stmt& b){
						delete m_body;
						m_body = new Stmt(b);
					}
					void set_params(const std::vector<Token>& p){
						delete m_params;
						int size = p.size();
						m_params = new std::vector<Token*>(size);
						std::vector<Token*>& m_p = *m_params;
						for (int i = 0; i != size; i++){
							m_p[i] = new Token(p[i]);
						}
					}
			};
			class ret_stmt_fields : public stmt_fields {
				private:
					eptr m_val;
					Token* m_keyword;
				public:
					ret_stmt_fields(const Expr& v, const Token& op):
						m_val(new Expr(v)),
						m_keyword(new Token(op)){}
					~ret_stmt_fields() override {
						delete m_val;
						delete m_keyword;
					}

					Expr& value(){
						return *m_val;
					}
					Token& keyword(){
						return *m_keyword;
					}

					void set_value(const Expr& v){
						delete m_val;
						m_val = new Expr(v);
					}
					void set_keyword(const Token& keywd){
						delete m_keyword;
						m_keyword = new Token(keywd);
					}
			};
			class assert_stmt_fields : public stmt_fields {
				private:
					eptr m_value;
					int m_code;
					TString m_msg;
				public:
					assert_stmt_fields(const Expr& val, int code, const TString& msg):
						m_value(new Expr(val)),
						m_code(code),
						m_msg(msg){}
					~assert_stmt_fields() override {
						delete m_value;
					}

					Expr& value(){
						return *m_value;
					}
					int code(){
						return m_code;
					}
					TString message(){
						return m_msg;
					}

					void set_value(const Expr& value){
						delete m_value;
						m_value = new Expr(value);
					}
					void set_code(int code){
						m_code = code;
					}
					void set_message(const TString& message){
						m_msg = message;
					}
			};
			class match_stmt_fields : public stmt_fields {
				private:
					eptr m_match;
					std::vector<Branch>* m_branches;
				public:
					match_stmt_fields(const Expr& match, const std::vector<Branch>& br):
						m_match(new Expr(match)),
						m_branches(new std::vector<Branch>(br)){}
					~match_stmt_fields() override {
						delete m_match;
						delete m_branches;
					}

					Expr& match(){
						return *m_match;
					}
					std::vector<Branch>& branches(){
						return *m_branches;
					}

					void set_match(const Expr& match){
						delete m_match;
						m_match = new Expr(match);
					}
					void set_branches(const std::vector<Branch>& br){
						delete m_branches;
						m_branches = new std::vector<Branch>(br);
					}
			};
			class invalid_fields : public stmt_fields {
				public:
					invalid_fields(){}
					~invalid_fields() override {}
			};
			class empty_fields : public stmt_fields {
				public:
					empty_fields(){}
					~empty_fields() override {}
			};

		private: // members
			mutable bool m_handled = false;
			stmt_type m_type;
			mutable stmt_fields* m_fields;
		private:
			void set_handled() const;
			void handle() const ;
		public:

			FieldsGetter(expr_fields,expr)
			FieldsGetter(var_decl_fields,var)
			FieldsGetter(block_fields,block)
			FieldsGetter(if_stmt_fields,if_else)
			FieldsGetter(while_stmt_fields,while_loop)
			FieldsGetter(fn_decl_fields,fn)
			FieldsGetter(ret_stmt_fields,ret)
			FieldsGetter(assert_stmt_fields,assertion)
			FieldsGetter(match_stmt_fields,match)
			FieldsGetter(print_fields,print)
			FieldsGetter(println_fields,println)
			FieldsGetter(empty_fields,empty)
			FieldsGetter(invalid_fields,invalid)

			stmt_fields* data() const {
				return m_fields;
			}

			stmt_type type() const {
				return m_type;
			}
			bool is(stmt_type t) const {
				return type() == t;
			}

			Stmt();
			Stmt(stmt_type t, stmt_fields* fields);
			Stmt(const Stmt&);

			~Stmt();

			bool is_handled() const;

			Stmt& operator=(const Stmt&) = delete;
			Stmt& operator*=(const Stmt& rhs){
				this->m_handled = rhs.m_handled;
				rhs.set_handled();
				this->m_type = rhs.m_type;
				this->m_fields = rhs.m_fields;
				return *this;
			}
	};

	typedef std::vector<Stmt> Stmts;

#define ExprStmt(EXPR)					Stmt(Stmt::ExprStmt,ptr_cast<Stmt::stmt_fields*>(new Stmt::expr_fields(EXPR)))
#define VarDeclStmt(INIT,NAME)			Stmt(Stmt::VarDecl,ptr_cast<Stmt::stmt_fields*>(new Stmt::var_decl_fields(INIT,NAME)))
#define NullStmt()						Stmt(Stmt::Empty,ptr_cast<Stmt::stmt_fields*>(new Stmt::empty_fields))
#define BlockStmt(BLOCK)				Stmt(Stmt::Block,ptr_cast<Stmt::stmt_fields*>(new Stmt::block_fields(BLOCK)))
#define IfStmt(CONDIT,IF,ELSE)			Stmt(Stmt::If,ptr_cast<Stmt::stmt_fields*>(new Stmt::if_stmt_fields(CONDIT,IF,ELSE)))
#define WhileStmt(CONDIT,BLOCK)			Stmt(Stmt::While,ptr_cast<Stmt::stmt_fields*>(new Stmt::while_stmt_fields(CONDIT,BLOCK)))
#define PrintStmt(EXPR)					Stmt(Stmt::Print,ptr_cast<Stmt::stmt_fields*>(new Stmt::print_fields(EXPR)))
#define FnStmt(NAME,PARAMS,BODY)		Stmt(Stmt::Function,ptr_cast<Stmt::stmt_fields*>(new Stmt::fn_decl_fields(NAME,PARAMS,BODY)))
#define RetStmt(VALUE,KEYWORD)			Stmt(Stmt::Return,ptr_cast<Stmt::stmt_fields*>(new Stmt::ret_stmt_fields(VALUE,KEYWORD)))
#define AssertStmt(VALUE,CODE,MESSAGE)	Stmt(Stmt::Assert,ptr_cast<Stmt::stmt_fields*>(new Stmt::assert_stmt_fields(VALUE,CODE,MESSAGE)))
#define MatchStmt(MATCH,BRANCHES)		Stmt(Stmt::Match,ptr_cast<Stmt::stmt_fields*>(new Stmt::match_stmt_fields(MATCH,BRANCHES)))
#define PrintlnStmt(EXPR)				Stmt(Stmt::Println,ptr_cast<Stmt::stmt_fields*>(new Stmt::println_fields(EXPR)))
}
#endif // STMT_H
