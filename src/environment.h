#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
//#include <QSharedPointer>

#include <QHash>

#include "object.h"
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
		void define(const Token&, const Object&);
		void define(const TString&, const Object&);
		Object& access(const Token&);
		Object& assign(const Token&, const Object&);
};
typedef EnvPtr Environment;
#endif // ENVIRONMENT_H
