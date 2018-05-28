#pragma once

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
//#include <QSharedPointer>

#include <QHash>

#define EnvPtr EnvImpl* /*QSharedPointer<EnvImpl>*/

namespace mere {

	class Token;
	class Object;

	class EnvImpl final {
		private:
			EnvPtr enclosing; ///< The enclosing (outer) Environment
			QHash<QString, Object> values; ///< The values table (Identifier to Object)

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
