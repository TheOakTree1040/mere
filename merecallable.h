#ifndef MERECALLABLE_H
#define MERECALLABLE_H
#include <functional>
#include "object.h"
#include "stmt.h"
#include <QVector>
#include
enum class MC {
	Reg,
	Ref,
	Nat
};
class MereCallable{
	public:
		MC m_ty;
		union{
				std::function<Object(const QVector<Object>&)> m_fn;
				MereCallable* m_mc;
				Stmts m_block;
		};
		int m_arity;
		QVector<Trait> m_signature;// return type at 0;
		Object call(const QVector<Object>& vec){
			if (ty == MC::Nat)
				return m_fn(vec);
			if (ty == MC::Ref)
				return m_mc->call(vec);
			return Object();
		}

};

#endif // MERECALLABLE_H
