#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
//#include <QSharedPointer>
#include "object.h"
#include <QHash>
#include "runtimeerror.h"
#define EnvPtr EnvImpl*/*QSharedPointer<EnvImpl>*/
typedef TString Identifier;
class EnvImpl final
{
	private:
		EnvPtr enclosing;
		QHash<Identifier,Object> values;
	public:
		EnvImpl();
		EnvImpl(EnvPtr);
		void define(const Token&, const Object&)throw(RuntimeError);
		void define(const TString&, const Object&)throw(RuntimeError);
		Object& access(const Token&)throw(RuntimeError);
		Object& assign(const Token&, const Object&)throw(RuntimeError);
};
typedef EnvPtr Environment;
#endif // ENVIRONMENT_H
