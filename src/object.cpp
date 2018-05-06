#include "object.h"
#include <QVector>
#include "merecallable.h"
int Trait::default_traits = 0b10100100;
QVector<TraitMatcher> Trait::type_lookup(
	{
		TraitMatcher(									),//void (default args)
		TraitMatcher(TString("null"		),	Ty::Null	),
		TraitMatcher(TString("real"		),	Ty::Real	),//real
		TraitMatcher(TString("char"		),	Ty::Char	),
		TraitMatcher(TString("string"	),	Ty::String	),//string
		TraitMatcher(TString("bool"		),	Ty::Bool	),//bool
		TraitMatcher(TString("array"	),	Ty::Array	),//array
		TraitMatcher(TString("map"		),	Ty::Map		),//map
		TraitMatcher(TString("hash"		),	Ty::Hash	),//map
		TraitMatcher(TString("function"	),	Ty::Function)
	}
);
Ty Trait::type_of(const TString& str){
	int ty_sz = type_lookup.size();
	for (int i = 0; i != ty_sz; i++){
		if (type_lookup[i].match(str))
			return type_lookup[i].type;
	}
	return Ty::Struct;
}
void Object::delete_ptr(){
    if (trait().is_data() && m_ptr){
		delete m_ptr;
    }
    m_ptr = nullptr;
}

Object& Object::fn_init(){
	LFn;
	if (trait().is_fn()){
#if _DEBUG
		Log << "Contacting MereCallable";
#endif
        reinterpret_cast<MereCallable*>(m_ptr->data())->set_onstack(trait().is_on_stack());
	}
	LRet *this;
}
