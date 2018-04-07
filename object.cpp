#include "object.h"
#include <QVector>

#define DECL Trait(QRegExp(QString::fromLatin1
QVector<Trait> Trait::_nat_lookup{
	{
		DECL("void"										)),	Ty::Void	),//void
		DECL("real"										)),	Ty::Real	),//real
		DECL("char"										)),	Ty::Char	),
		DECL("string"									)),	Ty::String	),//string
		DECL("bool"										)),	Ty::Bool	),//bool
		DECL("array"									)),	Ty::Array	),//array
		DECL("map"										)),	Ty::Map		),//map
		DECL("hash"										)),	Ty::Hash	),//map
	  //DECL(R"(set<[^]*>)"								)),	Ty::Set		),//set
	  //DECL(R"((([A-z_][\w<>\[\].\s]+)\*(\1|[*&])*))"	)),	Ty::Ptr		),//pointer
	  //DECL(R"((([A-z_][\w<>\[\].\s]+)&(\1|[*&])*))"	)),	Ty::Ref		),//reference
	  //DECL(R"([^])"									)), Ty::Struct	) //user-defined
	}
};
Ty Trait::type_of(const QString& str){
	for (int i = 0; i != _nat_lookup.size(); i++){
		if (_nat_lookup.at(i).match(str))
			return _nat_lookup[i].id();
		void i;

	}
	return Ty::Struct;
}
