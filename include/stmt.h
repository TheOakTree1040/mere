#ifndef STMT_H
#define STMT_H
#include "expr.h"

namespace mere {
	class Stmt;

#define sptr Stmt*

	struct Branch {
		private_fields:
			mutable bool m_handled = false;
			eptr m_expr;
			sptr m_stmt;
		public_methods:
			Branch();
			Branch(const Expr& ex, const Stmt& st);
			Branch(const Branch& other);
			Branch& operator=(const Branch& other);

			~Branch();

			Expr& expr() const { return *m_expr; }
			Stmt& stmt() const { return *m_stmt; }
	};

	/*!
	 * \brief The Stmt class
	 */
	class Stmt {
			friend class Interpreter;
		public_decls:
			/*!
			 * \brief The stmt_type enum
			 */
			enum stmt_type : char {
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
				Match,
				Run
			};
			class stmt_fields{
				private_methods:
					stmt_fields& operator=(const stmt_fields&) = delete;
					stmt_fields(const stmt_fields&) = delete;
				protected_methods:
					stmt_fields(){}
				public_methods:
					virtual ~stmt_fields(){}
			};
			class var_decl_fields : public stmt_fields {
				private_fields:
					eptr m_init;
					Token* m_var_name;
				public_methods:
					var_decl_fields(const Expr& ini, const Token& name);

					~var_decl_fields() override;

					Expr& init() const { return *m_init; }
					const Token& name() const { return *m_var_name; }
					void set_init(const Expr& init);
					void set_name(const Token& tok);
			};
			class if_stmt_fields : public stmt_fields {
				private_fields:
					eptr m_condition;
					sptr m_if_block;
					sptr m_else_block;
				public_methods:
					if_stmt_fields(const Expr& condit, const Stmt& if_blk, const Stmt& else_blk);
					~if_stmt_fields() override;

					Expr& condition() const { return *m_condition; }
					Stmt& if_block() const { return *m_if_block; }
					Stmt& else_block() const { return *m_else_block; }

					void set_condition(const Expr& c);
					void set_if(const Stmt& i);
					void set_else(const Stmt& e);
			};
			class while_stmt_fields : public stmt_fields {
				private_fields:
					eptr m_condition;
					sptr m_block;
				public_methods:
					while_stmt_fields(const Expr& condit, const Stmt& block);
					~while_stmt_fields() override;

					Expr& condition() const { return *m_condition; }
					Stmt& block() const { return *m_block; }

					void set_condition(const Expr& condit);
					void set_block(const Stmt& block);
			};
			class block_fields : public stmt_fields {
				private_fields:
					std::vector<sptr>* m_block;
				public_methods:
					block_fields(const std::vector<Stmt>& b);
					~block_fields() override;

					std::vector<Ref<Stmt>> block() const;
					void set_block(const std::vector<Stmt>& b);
			};
			class expr_fields : public stmt_fields {
				private_fields:
					eptr m_expr;
				public_methods:
					expr_fields(const Expr& expr):m_expr(new Expr(expr)){}
					~expr_fields() override { delete m_expr; }

					Expr& expr() const { return *m_expr; }
					void set_expr(const Expr& expr);
			};
			class print_fields : public expr_fields {
				public_methods:
					print_fields(const Expr& ex) : expr_fields(ex){}
			};
			class println_fields : public expr_fields {
				public_methods:
					println_fields(const Expr& ex) : expr_fields(ex){}
			};
			class fn_decl_fields : public stmt_fields {
				private_fields:
					Token* m_name;
					std::vector<Token*>* m_params;
					sptr m_body;
				public_methods:
					fn_decl_fields(const Token& n, const std::vector<Token>& p, const Stmt& b);
					~fn_decl_fields() override;

					const Token& name() const { return *m_name; }
					std::vector<Ref<Token>> params() const;
					Stmt& body() const { return *m_body; }

					void set_name(const Token& n);
					void set_body(const Stmt& b);
					void set_params(const std::vector<Token>& p);
			};
			class ret_stmt_fields : public stmt_fields {
				private_fields:
					eptr m_val;
					Token* m_keyword;
				public_methods:
					ret_stmt_fields(const Expr& v, const Token& op);
					~ret_stmt_fields() override;

					Expr& value() const { return *m_val; }
					const Token& keyword() const { return *m_keyword; }

					void set_value(const Expr& v);
					void set_keyword(const Token& keywd);
			};
			class assert_stmt_fields : public stmt_fields {
				private_fields:
					eptr m_value;
					int m_code;
					TString m_msg;
				public_methods:
					assert_stmt_fields(const Expr& val, int code, const TString& msg);
					~assert_stmt_fields() override { delete m_value; }

					Expr& value() const { return *m_value; }
					const int& code() const { return m_code; }
					const TString& message() const { return m_msg; }

					void set_value(const Expr& value);
					void set_code(int code);
					void set_message(const TString& message);
			};
			class match_stmt_fields : public stmt_fields {
				private_fields:
					eptr m_match;
					std::vector<Branch>* m_branches;
				public_methods:
					match_stmt_fields(const Expr& match, const std::vector<Branch>& br);
					~match_stmt_fields() override;

					Expr& match() const { return *m_match; }
					const std::vector<Branch>& branches() const { return *m_branches; }

					void set_match(const Expr& match);
					void set_branches(const std::vector<Branch>& br);
			};
			class invalid_fields : public stmt_fields {
				public_methods:
					invalid_fields(){}
					~invalid_fields() override {}
			};
			class empty_fields : public stmt_fields {
				public_methods:
					empty_fields(){}
					~empty_fields() override {}
			};
			class run_fields : public stmt_fields {
				private_fields:
					TString m_filename;
				public_methods:
					run_fields(const TString& fn) : m_filename(fn){}
					const TString& filename() const { return m_filename; }
			};

		private_fields: // members
			mutable bool m_handled = false;
			stmt_type m_type;
			mutable stmt_fields* m_fields;
		private_methods:
			void set_handled() const;
			void handle() const;
		public_methods:
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
			FieldsGetter(run_fields,run)

			stmt_fields* data() const { return m_fields; }
			stmt_type type() const { return m_type; }
			bool is(stmt_type t) const { return type() == t; }
			bool is_handled() const;

			Stmt();
			Stmt(stmt_type t, stmt_fields* fields);
			Stmt(const Stmt&);

			~Stmt();

			Stmt& operator=(const Stmt&) = delete;
			Stmt& operator*=(const Stmt& rhs);
	};

	typedef std::vector<Stmt> Stmts;

#define ExprStmt(EXPR)					Stmt(Stmt::ExprStmt,new Stmt::expr_fields(EXPR))
#define VarDeclStmt(INIT,NAME)			Stmt(Stmt::VarDecl,new Stmt::var_decl_fields(INIT,NAME))
#define NullStmt()						Stmt(Stmt::Empty,new Stmt::empty_fields)
#define BlockStmt(BLOCK)				Stmt(Stmt::Block,new Stmt::block_fields(BLOCK))
#define IfStmt(CONDIT,IF,ELSE)			Stmt(Stmt::If,new Stmt::if_stmt_fields(CONDIT,IF,ELSE))
#define WhileStmt(CONDIT,BLOCK)			Stmt(Stmt::While,new Stmt::while_stmt_fields(CONDIT,BLOCK))
#define PrintStmt(EXPR)					Stmt(Stmt::Print,new Stmt::print_fields(EXPR))
#define FnStmt(NAME,PARAMS,BODY)		Stmt(Stmt::Function,new Stmt::fn_decl_fields(NAME,PARAMS,BODY))
#define RetStmt(VALUE,KEYWORD)			Stmt(Stmt::Return,new Stmt::ret_stmt_fields(VALUE,KEYWORD))
#define AssertStmt(VALUE,CODE,MESSAGE)	Stmt(Stmt::Assert,new Stmt::assert_stmt_fields(VALUE,CODE,MESSAGE))
#define MatchStmt(MATCH,BRANCHES)		Stmt(Stmt::Match,new Stmt::match_stmt_fields(MATCH,BRANCHES))
#define PrintlnStmt(EXPR)				Stmt(Stmt::Println,new Stmt::println_fields(EXPR))
#define RunStmt(FILENAME)				Stmt(Stmt::Run,new Stmt::run_fields(FILENAME))
}
#endif // STMT_H
