#ifndef EXPR_H
#define EXPR_H

#include <vector>

#include "tlogger.h"
#include "object.h"

namespace mere {
#define eptr Expr*

#define FieldsGetter(TYPE,NAME)\
	TYPE& NAME() const {\
	return *ptr_cast<TYPE*>(m_fields);\
}\
	TYPE* NAME ## _data() const {\
	return ptr_cast<TYPE*>(m_fields);\
}

	template <typename T>
	using Ref = std::reference_wrapper<T>;
	using std::pair;

	class MereCallable;
	class Token;

	class invalid_access : std::runtime_error{
		public_methods:
			invalid_access(const std::string& where):std::runtime_error(where){}
	};

	class Expr {
			friend class Interpreter;
		public_decls:
			enum expr_type : char {
				Empty,
				Group,
				Literal,
				Binary,
				Prefix,
				Postfix,
				Map,
				Hash,
				Assoc,
				Array,
				FuncCall,
				CommaEx,
				VarAcsr,
				Assign,
				Conditional,
				Lambda,
				Logical,
				Refer
			};

			class expr_fields {
				private_methods:
					expr_fields& operator=(const expr_fields&) = delete;
					expr_fields(const expr_fields&);
				protected_methods:
					expr_fields(){}
				public_methods:
					virtual ~expr_fields(){}
			};

			class nested_expr_fields : virtual public expr_fields {
				protected_fields:
					eptr m_expr;
				public_methods:
					nested_expr_fields(const Expr& ex);
					virtual ~nested_expr_fields() override;

					Expr& expr() const;
					void set_expr(const Expr& ex);
			};
			class operator_fields : virtual public expr_fields {
				protected_fields:
					Token* m_op;
				protected_methods:
					operator_fields(const Token& op);
					virtual ~operator_fields() override;
				public_methods:
					Token& op() const;
					void set_op(const Token& o);
			};
			class unary_fields : public nested_expr_fields, public operator_fields {
				public_methods:
					unary_fields(const Expr& operand, const Token& op);
			};
			class binary_fields : public unary_fields {
				protected_fields:
					eptr m_right;
				public_methods:
					binary_fields(const Expr& left, const Token& op, const Expr& right);
					~binary_fields() override;

					Expr& left();
					Expr& right();

					void set_left(const Expr& left);
					void set_right(const Expr& right);
			};
			class logical_fields : public binary_fields {
				public_methods:
					logical_fields(const Expr& left, const Token& op, const Expr& right):
					binary_fields(left,op,right){}
			};
			class array_fields : public expr_fields {
				protected_fields:
					std::vector<eptr>* m_array;//array
				public_methods:
					array_fields(const std::vector<Expr>& arr);
					~array_fields() override;

					std::vector<Ref<Expr>> array() const ;
					void set_array(const std::vector<Expr>& arr);
			};
			class pair_vector_fields : public expr_fields {
				protected_decls:
					using Data =  std::vector<std::pair<eptr,eptr>> ;
				protected_fields:
					Data* m_data;
				protected_methods:
					pair_vector_fields(const std::vector<std::pair<Expr,Expr>>& dat);
				public_methods:
					~pair_vector_fields() override;

					void set_data(const std::vector<std::pair<Expr,Expr>>& dat);
					std::vector<std::pair<Ref<Expr>,Ref<Expr>>>
					data() const;

			};
			class map_fields : public pair_vector_fields {
				public_methods:
					map_fields(const std::vector<std::pair<Expr,Expr>>& map):
						pair_vector_fields(map){}

					auto map() { return data(); }

					void set_map
					(const std::vector<std::pair<Expr,Expr>>& map)
					{ set_data(map); }
			};
			class hash_fields : public pair_vector_fields {
				public_methods:
					hash_fields(const std::vector<std::pair<Expr,Expr>>& map):
						pair_vector_fields(map){}

					auto hash() const { return data(); }

					void set_hash
					(const std::vector<std::pair<Expr,Expr>>& hash)
					{ set_data(hash); }
			};
			class var_acsr_fields : public expr_fields {
				protected_fields:
					Token* m_var_acsr;
				public_methods:
					var_acsr_fields(const Token& acsr);
					~var_acsr_fields() override;

					Token& accessor() const { return *m_var_acsr; }
					void set_accessor(const Token& tok);
			};
			class lit_fields : public expr_fields {
				protected_fields:
					Object* m_lit; //literal
				public_methods:
					lit_fields(const Object& lit) : m_lit(new Object(lit)){}
					lit_fields(Object* lit) : m_lit(lit){}
					~lit_fields() override { delete m_lit; }

					Object& lit() const { return *m_lit; }
					void set_lit(const Object& p_lit);
			};
			class call_fields : public operator_fields {
				protected_fields:
					eptr m_callee;
					std::vector<eptr>* m_arguments;
				public_methods:
					call_fields(const Expr& callee, const std::vector<Expr>& args, const Token& paren);
					~call_fields() override;

					Expr& callee() const;
					std::vector<Ref<Expr>> arguments() const;

					void set_callee(const Expr& cal);
					void set_args(const std::vector<Expr>& args);
			};
			class refer_fields : public binary_fields {
				public_methods:
					refer_fields(const Expr& left, const Token& op, const Expr& right):
						binary_fields(left,op,right){}
			};
			class assign_fields : public binary_fields {
				public_methods:
					assign_fields(const Expr& left, const Token& op, const Expr& right):
						binary_fields(left,op,right){}
			};
			class ternary_fields : public expr_fields {
				protected_fields:
					eptr m_condition;
					eptr m_left;
					eptr m_right;
				public_methods:
					ternary_fields(const Expr& condit, const Expr& lft, const Expr& rt):
						m_condition(new Expr(condit)),
						m_left(new Expr(lft)),
						m_right(new Expr(rt)){}
					~ternary_fields() override;

					Expr& condition() const { return *m_condition; }
					Expr& left() const { return *m_left; }
					Expr& right() const { return *m_right; }

					void set_condition(const Expr& c);
					void set_left(const Expr& lft);
					void set_right(const Expr& rt);
			};
			class cs_fields : public expr_fields {
				protected_fields:
					std::vector<eptr>* m_exprs;
				public_methods:
					cs_fields(const std::vector<Expr>& cs_exprs);
					~cs_fields() override;

					std::vector<Ref<Expr>> exprs();

					void set_exprs(const std::vector<Expr>& cs_exprs);
			};
			class lambda_fields : public expr_fields {
				protected_fields:
					MereCallable* m_callable;
				public_methods:
					lambda_fields(MereCallable* c) : m_callable(c){}
					~lambda_fields();

					MereCallable& callable() const { return *m_callable; }
			};
			class empty_fields : public expr_fields {
				public_methods:
					empty_fields() = default;
			};

			typedef nested_expr_fields group_fields;

		private_fields:
			expr_type m_type;
			mutable expr_fields* m_fields;
			mutable bool m_handled = false;
		private_methods:
			void set_handled() const { m_handled = true; }
			expr_fields* data() const { return m_fields; }
			void handle() const;
		public_methods:
			Expr();
			Expr(const Expr & expr);
			Expr(expr_type t, expr_fields* f);

			~Expr();

			bool is_handled() const { return m_handled; }
			expr_type type() const { return m_type; }
			bool is(expr_type t) const { return m_type == t; }

			Expr& operator=(const Expr&) = delete;
			Expr& operator*=(const Expr& rhs);

			FieldsGetter(lit_fields,lit)
			FieldsGetter(binary_fields,bin)
			FieldsGetter(logical_fields,logical)
			FieldsGetter(assign_fields,assign)
			FieldsGetter(group_fields,group)
			FieldsGetter(unary_fields,unary)
			FieldsGetter(cs_fields,cs)
			FieldsGetter(ternary_fields,ternary)
			FieldsGetter(var_acsr_fields,var)
			FieldsGetter(call_fields,call)
			FieldsGetter(lambda_fields,lambda)
			FieldsGetter(array_fields,array)
			FieldsGetter(hash_fields,hash)
			FieldsGetter(refer_fields,ref)
	};


#define NullExpr()						Expr(Expr::Empty,ptr_cast<Expr::expr_fields*>(new Expr::empty_fields()))

#define LitExpr(LIT)					Expr(Expr::Literal,ptr_cast<Expr::expr_fields*>(new Expr::lit_fields(LIT)))
#define BinExpr(LEFT,OP,RIGHT)			Expr(Expr::Binary,ptr_cast<Expr::expr_fields*>(new Expr::binary_fields(LEFT,OP,RIGHT)))
#define LogicalExpr(LEFT,OP,RIGHT)		Expr(Expr::Logical,ptr_cast<Expr::expr_fields*>(new Expr::logical_fields(LEFT,OP,RIGHT)))
#define AssignExpr(LEFT,OP,RIGHT)		Expr(Expr::Assign,ptr_cast<Expr::expr_fields*>(new Expr::assign_fields(LEFT,OP,RIGHT)))
#define GroupExpr(EXPR)					Expr(Expr::Group,ptr_cast<Expr::expr_fields*>(new Expr::group_fields(EXPR)))
#define PstfxExpr(OPERAND,OP)			Expr(Expr::Postfix,ptr_cast<Expr::expr_fields*>(new Expr::unary_fields(OPERAND,OP)))
#define PrefxExpr(OPERAND,OP)			Expr(Expr::Prefix,ptr_cast<Expr::expr_fields*>(new Expr::unary_fields(OPERAND,OP)))
#define CSExpr(EXPRS)					Expr(Expr::CommaEx,ptr_cast<Expr::expr_fields*>(new Expr::cs_fields(EXPRS)))
#define CndtnlExpr(CONDIT,LFT,RT)		Expr(Expr::Conditional,ptr_cast<Expr::expr_fields*>(new Expr::ternary_fields(CONDIT,LFT,RT)))

#define VarAcsrExpr(ACSR)				Expr(Expr::VarAcsr,ptr_cast<Expr::expr_fields*>(new Expr::var_acsr_fields(ACSR)))

#define FnCallExpr(CALLEE,ARGS,PAREN)	Expr(Expr::FuncCall,ptr_cast<Expr::expr_fields*>(new Expr::call_fields(CALLEE,ARGS,PAREN)))
#define LambdaExpr(CALLABLE)			Expr(Expr::Lambda,ptr_cast<Expr::expr_fields*>(new Expr::lambda_fields(CALLABLE)))

#define ArrayExpr(ARRAY)				Expr(Expr::Array,ptr_cast<Expr::expr_fields*>(new Expr::array_fields(ARRAY)))
#define AssocExpr(ASSOC)				Expr(Expr::Assoc,ptr_cast<Expr::expr_fields*>(new Expr::hash_fields(ASSOC)))
#define HashExpr(HASH)					Expr(Expr::Hash,ptr_cast<Expr::expr_fields*>(new Expr::hash_fields(HASH)))

#define RefExpr(LEFT,OP,RIGHT)			Expr(Expr::Refer,ptr_cast<Expr::expr_fields*>(new Expr::refer_fields(LEFT,OP,RIGHT)))
}
#endif // EXPR_H
