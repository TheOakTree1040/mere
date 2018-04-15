#include "object.h"
#include <QVector>
int Trait::default_traits = 0b10100100;
QVector<TraitMatcher> Trait::type_lookup(
	{
		TraitMatcher(									),//void (default args)
		TraitMatcher(QString("null"		),	Ty::Null	),
		TraitMatcher(QString("real"		),	Ty::Real	),//real
		TraitMatcher(QString("char"		),	Ty::Char	),
		TraitMatcher(QString("string"	),	Ty::String	),//string
		TraitMatcher(QString("bool"		),	Ty::Bool	),//bool
		TraitMatcher(QString("array"	),	Ty::Array	),//array
		TraitMatcher(QString("map"		),	Ty::Map		),//map
		TraitMatcher(QString("hash"		),	Ty::Hash	)//map
	}
);
Ty Trait::type_of(const QString& str){
	int ty_sz = type_lookup.size();
	for (int i = 0; i != ty_sz; i++){
		if (type_lookup[i].match(str))
			return type_lookup[i].type;
	}
	return Ty::Struct;
}
