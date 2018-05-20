#ifndef MERECALLABLE_H
#define MERECALLABLE_H
#include <functional>
#include "object.h"
#include "stmt.h"
#include <vector>

namespace mere{
	enum class Call : char{
		Reg,
		Nat,
		OnStack,
		__
	};
	class Interpreter;
	typedef std::function<Object(Interpreter&,std::vector<Object>&)> Callable;
#define CALLABLE [](Interpreter& interpreter, std::vector<Object>& arguments) -> Object
	class Return : public std::runtime_error{
		protected:
			Object obj;
		public:
			Return(const Object& obj):std::runtime_error("_mere_return_"),obj(obj){}
			Object value(){
				return obj;
			}
	};

	//
	class MereCallable{
			friend class Interpreter;
		private:
			std::bitset<4> m_traits;
			static int default_traits; // [REG:1][REF:0][NAT:0][___:0]
			union {
					Callable* m_callable;
					sptr m_fn;
			};
			int m_arity = 0;
			MereCallable& set_arity(int i){
				if (!is(Call::Reg)){
					m_arity = i;
				}
				return *this;
			}
			Stmt& function(){
				return *m_fn;
			}

			Object call(Interpreter&, std::vector<Object>&);
		public:// return type at 0;
			MereCallable& set(Call ind, bool val = true){
				m_traits.set(static_cast<size_t>(ind),val);
				return *this;
			}

			MereCallable& as_reg(){
				return set(Call::Reg).set(Call::Nat,false);
			}
			MereCallable& as_nat(){
				return set(Call::Reg,false).set(Call::Nat);
			}

			MereCallable& set_onstack(bool val = true){
				return set(Call::OnStack,val);
			}

			MereCallable(const MereCallable& mc):
				m_traits(mc.m_traits),
				m_arity(mc.m_arity){
				LFn;
				if (mc.is(Call::Reg)){
					m_fn = mc.m_fn;
				}
				else {
					m_callable = mc.m_callable;
				}
				LVd;
			}

			MereCallable():
				m_traits(default_traits),
				m_fn(new NullStmt()){}
			MereCallable(const Callable& clb):
				m_traits(default_traits),
				m_callable(new Callable(clb)){
				as_nat();
			}
			MereCallable(const Stmt& fn_def):
				m_traits(default_traits),
				m_fn(new Stmt(fn_def)),
				m_arity(fn_def.fn().params().size()){
				as_reg();
			}

			int arity(){
				return m_arity;
			}


			bool is(Call ci) const {
				return m_traits.test(t_cast<size_t>(ci));
			}

			~MereCallable(){
				LFn;
				if (is(Call::OnStack)){
					if (is(Call::Reg)){
						Logp("<---== deleting m_fn ==--->");
						delete m_fn;
					}
					else if (is(Call::Nat)){
						Logp("<---== deleting m_callable ==--->");
						delete m_callable;
					}
				}
				LVd;
			}



	};

}
Q_DECLARE_METATYPE(mere::MereCallable)

#endif // MERECALLABLE_H
