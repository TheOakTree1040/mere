#ifndef RUNTIMEERROR_H
#define RUNTIMEERROR_H
#include <QException>
#include "token.h"
class RuntimeError : public QException{
	public:
		QString msg = "UnknownRuntimeError";
		Token tok = Token();
		void raise() const{
			throw *this;
		}
		RuntimeError* clone() const{
			return new RuntimeError(*this);
		}
		RuntimeError(){}
		RuntimeError(const Token& t, const QString& m) : msg(m), tok(t){}
};
#endif // RUNTIMEERROR_H
