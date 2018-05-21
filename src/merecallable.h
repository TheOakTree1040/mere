#ifndef MERECALLABLE_H
#define MERECALLABLE_H

#include "stmt.h"

#define CALLABLE [](Interpreter& interpreter, std::vector<Object>& arguments) -> Object

namespace mere{
	enum class Call : char {
		Reg,
		Nat,
		OnStack,
		__
	};

	class Interpreter;
	typedef std::function<Object(Interpreter&,std::vector<Object>&)> Callable;

	class Return : public std::runtime_error{
		protected_fields:
			Object obj;
		public_methods:
			Return(const Object& obj):std::runtime_error("_mere_return_"),obj(obj){}
			Object value() const { return obj; }
	};

	class MereCallable{
			friend class Interpreter;

		private_fields:
			std::bitset<4> m_traits;
			static int default_traits; // [REG:1][REF:0][NAT:0][___:0]
			union {
					Callable* m_callable;
					sptr m_fn;
			};
			int m_arity = 0;

		private_methods:
			MereCallable& set_arity(int i);
			Stmt& function() const { return *m_fn; }

			Object call(Interpreter&, std::vector<Object>&);

		public_fields:// return type at 0;
			MereCallable& set(Call ind, bool val = true);

			MereCallable& as_reg(){ return set(Call::Reg).set(Call::Nat,false); }
			MereCallable& as_nat(){ return set(Call::Reg,false).set(Call::Nat); }

			MereCallable& set_onstack(bool val = true){ return set(Call::OnStack,val); }

			MereCallable(const MereCallable& mc);

			MereCallable();
			MereCallable(const Callable& clb);
			MereCallable(const Stmt& fn_def);

			int arity() const { return m_arity; }
			bool is(Call ci) const { return m_traits.test(t_cast<size_t>(ci)); }

			~MereCallable();
	};
}
Q_DECLARE_METATYPE(mere::MereCallable)

#endif // MERECALLABLE_H
