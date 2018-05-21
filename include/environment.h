#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
//#include <QSharedPointer>

#include <QHash>
#include "config.h"

#define EnvPtr EnvImpl*/*QSharedPointer<EnvImpl>*/

namespace mere {
	class Token;
	class Object;
	typedef QString Identifier;
	class EnvImpl final{
		private:
			EnvPtr enclosing;
			QHash<Identifier,Object> values;
		public:
			EnvImpl();
			EnvImpl(EnvPtr);
			void define(const Token&, const Object&);
			void define(const QString&, const Object&);
			Object& access(const Token&);
			Object& assign(const Token&, const Object&);
	};
	typedef EnvPtr Environment;

}

#endif // ENVIRONMENT_H
