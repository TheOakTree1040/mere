#ifndef MERECALLABLE_H
#define MERECALLABLE_H
#include <functional>
#include "object.h"
#include "stmt.h"
#include <QVector>
enum class Call {
	Reg,
	Nat,
	OnStack,
	__
};
class Interpreter;
typedef std::function<Object(Interpreter&,const QVector<Object>&)> Callable;
#define CALLABLE [](Interpreter& interpreter, const QVector<Object>& arguments)
class Return : public std::runtime_error{
	private:
		Object obj;
	public:
		Return(const Object& obj):std::runtime_error("return $;"),obj(obj){}
		Object value(){
			return obj;
		}
};

//
class MereCallable{
	private:
		std::bitset<4> m_traits;
		static int default_traits; // [REG:1][REF:0][NAT:0][___:0]
		union{
				Callable m_callable;
				Stmt m_fn;
		};
		int m_arity = 0;
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
		MereCallable(const MereCallable& mc):m_traits(mc.m_traits),m_arity(mc.m_arity){
			LFn;
			if (mc.is(Call::Reg)){
				m_fn = mc.m_fn;
			}
			else {
				m_callable = mc.m_callable;
			}
			LVd;
		}

		MereCallable():m_traits(default_traits),m_fn(NullStmt()){}
		MereCallable(Callable& clb):m_traits(default_traits),m_callable(clb){
			as_nat();
		}
		MereCallable(Stmt fn_def):m_traits(default_traits),m_fn(fn_def),m_arity(fn_def->fn_params->size()){
			as_reg();
		}

		int arity(){
			return m_arity;
		}

		MereCallable& set_arity(int i){
			if (!is(Call::Reg)){
				m_arity = i;
			}
			return *this;
		}

		Object call(Interpreter&, QVector<Object>&);

		bool is(Call ci) const {
			return m_traits.test(static_cast<size_t>(ci));
		}

		~MereCallable(){
			LFn;
			if (is(Call::Reg) && is(Call::OnStack)){
				//QMessageBox::information(nullptr,"","Deleting m_fn...");
				Log << "!!!==== DELETING M_FN ====!!!";
				delete m_fn;
			}
			LVd;
		}

};
Q_DECLARE_METATYPE(MereCallable)

#endif // MERECALLABLE_H
