#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
//#include <QSharedPointer>

#include <QHash>
#include "config.hpp"

#define EnvPtr EnvImpl*/*QSharedPointer<EnvImpl>*/

namespace mere {
	class Token;
	class Object;
	typedef QString Identifier;
	class EnvImpl final{
		private_fields:
			EnvPtr enclosing;
			QHash<Identifier,Object> values;
		public_methods:
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
