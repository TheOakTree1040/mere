#ifndef EXPR_H
#define EXPR_H

#include <QVariant>
#include <vector>
#include <functional>

#include "tlogger.h"
#include "token.hpp"
#include "object.h"
#include "utils.h"

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

	class invalid_access : std::runtime_error{
		public:
			invalid_access(const std::string& where):std::runtime_error(where){}
	};

	class Expr {
			friend class Interpreter;
		public:
			enum expr_type {
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
				private:
					expr_fields& operator=(const expr_fields&) = delete;
					expr_fields(const expr_fields&);
				protected:
					expr_fields(){}
				public:
					virtual ~expr_fields(){}
			};

			class nested_expr_fields : virtual public expr_fields {
				protected:
					eptr m_expr;
				public:
					nested_expr_fields(const Expr& ex):m_expr(new Expr(ex)){}
					virtual ~nested_expr_fields() override {
						delete m_expr;
					}

					Expr& expr(){
						return *m_expr;
					}

					void set_expr(const Expr& ex){
						delete m_expr;
						m_expr = new Expr(ex);
					}
			};
			class operator_fields : virtual public expr_fields {
				protected:
					Token* m_op;
					operator_fields(const Token& op):m_op(new Token(op)){}
					virtual ~operator_fields() override {
						delete m_op;
					}

				public:
					Token& op(){
						return *m_op;
					}
					void set_op(const Token& o){
						delete m_op;
						m_op = new Token(o);
					}
			};
			class unary_fields : public nested_expr_fields, public operator_fields {
				public:
					unary_fields(const Expr& operand, const Token& op):
						nested_expr_fields(operand),
						operator_fields(op){}
			};
			class binary_fields : public unary_fields {
				protected:
					eptr m_right;
				public:
					binary_fields(const Expr& left, const Token& op, const Expr& right):
						unary_fields(left,op),
						m_right(new Expr(right)){}
					~binary_fields() override {
						delete m_right;
					}

					Expr& left(){
						return expr();
					}
					Expr& right(){
						return *m_right;
					}

					void set_left(const Expr& left){
						set_expr(left);
					}
					void set_right(const Expr& right){
						delete m_right;
						m_right = new Expr(right);
					}
			};
			class logical_fields : public binary_fields {
				public:
					logical_fields(const Expr& left, const Token& op, const Expr& right):
					binary_fields(left,op,right){}
			};
			class array_fields : public expr_fields {
				protected:
					std::vector<eptr>* m_array;//array
				public:
					array_fields(const std::vector<Expr>& arr):
						m_array(nullptr){
						set_array(arr);
					}

					~array_fields() override {
						int s = m_array->size();
						for (int i = 0; i != s; i++){
							delete (*m_array)[i];
						}
						delete m_array;
					}

					std::vector<Ref<Expr>> array(){
						int size = m_array->size();
						std::vector<Ref<Expr>> ret;
						for (int i = 0; i != size; i++){
							ret.push_back(Ref<Expr>(*((*m_array)[i])));
						}
						return ret;
					}

					void set_array(const std::vector<Expr>& arr){
						delete m_array;
						int size = arr.size();
						m_array = new std::vector<eptr>(size);
						std::vector<eptr>& m_arr = *m_array;
						for (int i = 0; i != size; i++){
							m_arr[i] = new Expr(arr[i]);
						}
					}
			};
			class pair_vector_fields : public expr_fields {
				protected:
					using Data =  std::vector<std::pair<eptr,eptr>> ;

					Data* m_data;
					pair_vector_fields(const std::vector<std::pair<Expr,Expr>>& dat):
						m_data(nullptr){
						set_data(dat);
					}

				public:

					~pair_vector_fields() override {
						int size = m_data->size();
						for (int i = 0; i != size; i++){
							std::pair<eptr,eptr>& p = (*m_data)[i];
							delete p.first;
							delete p.second;
						}
						delete m_data;
					}

					void set_data(const std::vector<std::pair<Expr,Expr>>& dat){
						delete m_data;
						int size = dat.size();
						m_data = new Data;
						Data& m_d = *m_data;
						for (int i = 0; i != size; i++){
							const std::pair<Expr,Expr>& p = dat[i];
							m_d.push_back(std::pair<eptr,eptr>(new Expr(p.first),new Expr(p.second)));
						}
					}

					std::vector<std::pair<Ref<Expr>,Ref<Expr>>>
					data(){
						int size = m_data->size();
						std::vector<std::pair<Ref<Expr>,Ref<Expr>>> ret;
						for (int i = 0; i != size; i++){
							std::pair<eptr,eptr>& p = (*m_data)[i];
							ret.push_back(std::pair<Ref<Expr>,Ref<Expr>>(*p.first,*p.second));
						}
						return ret;
					}

			};
			class map_fields : public pair_vector_fields {
				public:
					map_fields(const std::vector<std::pair<Expr,Expr>>& map):
						pair_vector_fields(map){}

					auto map(){
						return data();
					}

					void set_map(const std::vector<std::pair<Expr,Expr>>& map){
						set_data(map);
					}
			};
			class hash_fields : public pair_vector_fields {
				public:
					hash_fields(const std::vector<std::pair<Expr,Expr>>& map):
						pair_vector_fields(map){}

					auto hash(){
						return data();
					}

					void set_hash(const std::vector<std::pair<Expr,Expr>>& hash){
						set_data(hash);
					}
			};
			class var_acsr_fields : public expr_fields {
				protected:
					Token* m_var_acsr;
				public:
					var_acsr_fields(const Token& acsr) :
						m_var_acsr(new Token(acsr)){}
					~var_acsr_fields() override {
						delete m_var_acsr;
					}

					Token& accessor(){
						return *m_var_acsr;
					}

					void set_accessor(const Token& tok){
						delete m_var_acsr;
						m_var_acsr = new Token(tok);
					}
			};
			class lit_fields : public expr_fields {
				protected:
					Object* m_lit; //literal
				public:
					lit_fields(const Object& lit):
						m_lit(new Object(lit)){}
					lit_fields(Object* lit):
						m_lit(lit){}
					~lit_fields() override {
						delete m_lit;
					}

					Object& lit() {
						return *m_lit;
					}
					void set_lit(const Object& p_lit){
						delete m_lit;
						m_lit = new Object(p_lit);
					}
			};
			class call_fields : public operator_fields {
				protected:
					eptr m_callee;
					std::vector<eptr>* m_arguments;
					//Token* op/paren;
				public:
					call_fields(const Expr& callee, const std::vector<Expr>& args, const Token& paren):
						operator_fields(paren),
						m_callee(new Expr(callee)),
						m_arguments(nullptr){
						set_args(args);
					}
					~call_fields() override {
						delete m_callee;
						int s = m_arguments->size();
						for (int i = 0; i != s; i++){
							delete (*m_arguments)[i];
						}
						delete m_arguments;
					}

					Expr& callee(){
						return *m_callee;
					}
					std::vector<Ref<Expr>> arguments(){
						int size = m_arguments->size();
						std::vector<Ref<Expr>> ret;
						for (int i = 0; i != size; i++){
							ret.push_back(Ref<Expr>(*((*m_arguments)[i])));
						}
						return ret;
					}

					void set_callee(const Expr& cal){
						delete m_callee;
						m_callee = new Expr(cal);
					}
					void set_args(const std::vector<Expr>& args){
						delete m_arguments;
						int size = args.size();
						m_arguments = new std::vector<eptr>(size);
						std::vector<eptr>& m_args = *m_arguments;
						for (int i = 0; i != size; i++){
							m_args[i] = new Expr(args[i]);
						}
					}
			};
			class refer_fields : public binary_fields {
				public:
					refer_fields(const Expr& left, const Token& op, const Expr& right):
						binary_fields(left,op,right){}
			};
			class assign_fields : public binary_fields {
				public:
					assign_fields(const Expr& left, const Token& op, const Expr& right):
						binary_fields(left,op,right){}
			};
			class ternary_fields : public expr_fields {
				protected:
					eptr m_condition;
					eptr m_left;
					eptr m_right;
				public:
					ternary_fields(const Expr& condit, const Expr& lft, const Expr& rt):
						m_condition(new Expr(condit)),
						m_left(new Expr(lft)),
						m_right(new Expr(rt)){}
					~ternary_fields() override {
						delete m_condition;
						delete m_left;
						delete m_right;
					}

					Expr& condition(){
						return *m_condition;
					}
					Expr& left(){
						return *m_left;
					}
					Expr& right(){
						return *m_right;
					}

					void set_condition(const Expr& c){
						delete m_condition;
						m_condition = new Expr(c);
					}
					void set_left(const Expr& lft){
						delete m_left;
						m_left = new Expr(lft);
					}
					void set_right(const Expr& rt){
						delete m_right;
						m_right = new Expr(rt);
					}
			};
			class cs_fields : public expr_fields {
				protected:
					std::vector<eptr>* m_exprs;
				public:
					cs_fields(const std::vector<Expr>& cs_exprs):
						m_exprs(nullptr){
						set_exprs(cs_exprs);
					}
					~cs_fields() override {
						int s = m_exprs->size();
						for (int i = 0; i != s; i++){
							delete (*m_exprs)[i];
						}
						delete m_exprs;
					}

					std::vector<Ref<Expr>> exprs(){
						int size = m_exprs->size();
						std::vector<Ref<Expr>> ret;
						for (int i = 0; i != size; i++){
							ret.push_back(Ref<Expr>(*((*m_exprs)[i])));
						}
						return ret;
					}

					void set_exprs(const std::vector<Expr>& cs_exprs){
						delete m_exprs;
						int size = cs_exprs.size();
						m_exprs = new std::vector<eptr>(size);
						std::vector<eptr>& m_ex = *m_exprs;
						for (int i = 0; i != size; i++){
							m_ex[i] = new Expr(cs_exprs[i]);
						}
					}
			};
			class lambda_fields : public expr_fields {
				protected:
					MereCallable* m_callable;
				public:
					lambda_fields(MereCallable* c):
						m_callable(c){}

					MereCallable& callable(){
						return *m_callable;
					}
			};
			class empty_fields : public expr_fields {
				public:
					empty_fields() = default;
			};

			typedef nested_expr_fields group_fields;

		private:
			expr_type m_type;
			mutable expr_fields* m_fields;
			mutable bool m_handled = false;
		private:
			void set_handled() const {
				m_handled = true;
			}
			expr_fields* data() const {
				return m_fields;
			}

			void handle() const;

		public:
			Expr():
			m_type(Empty),
			m_fields(new empty_fields){}

			Expr(const Expr & expr):
			m_type(expr.type()),
			m_fields(expr.data()){
				expr.set_handled();
			}

			Expr(expr_type t, expr_fields* f):
				m_type(t),
				m_fields(f){}

			~Expr();


			bool is_handled() const {
				return m_handled;
			}

			expr_type type() const {
				return m_type;
			}

			bool is(expr_type t) const {
				return m_type == t;
			}

			Expr& operator=(const Expr&) = delete;
			Expr& operator*=(const Expr& rhs){
				this->m_handled = rhs.m_handled;
				rhs.set_handled();
				this->m_type = rhs.m_type;
				this->m_fields = rhs.m_fields;
				return *this;
			}

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
