#include "object.h"
#include <vector>
#include "merecallable.h"

using namespace mere;
int Trait::default_traits = 0b10100100;
std::vector<TraitMatcher> Trait::type_lookup
({
	 TraitMatcher(										),//void (default args)
	 TraitMatcher(QString("null"		),	Ty::Null	),
	 TraitMatcher(QString("real"		),	Ty::Real	),//real
	 TraitMatcher(QString("char"		),	Ty::Char	),
	 TraitMatcher(QString("string"		),	Ty::String	),//string
	 TraitMatcher(QString("bool"		),	Ty::Bool	),//bool
	 TraitMatcher(QString("array"		),	Ty::Array	),//array
	 TraitMatcher(QString("map"			),	Ty::Map		),//map
	 TraitMatcher(QString("hash"		),	Ty::Hash	),//map
	 TraitMatcher(QString("function"	),	Ty::Function)
});
Ty Trait::type_of(const QString& str){
	int ty_sz = type_lookup.size();
	for (int i = 0; i != ty_sz; i++){
		if (type_lookup[i].match(str))
			return type_lookup[i].type;
	}
	return Ty::Struct;
}

Trait::Trait():
	m_traits(default_traits),
	m_ty(Ty::Void),
	m_id("void"){}

Trait::Trait(const Trait& other):
	m_traits(other.traits()),
	m_ty(other.type()),
	m_id(other.id()){}

Trait::Trait(const QString& id):
	m_traits(default_traits),
	m_ty(type_of(id)),
	m_id(id){}

Trait& Trait::set(TraitIndex i, bool val){
	m_traits.set(static_cast<size_t>(i),val);
	return *this;
}

Trait& Trait::set(const QString& id, bool searches){
	m_id = id;
	return searches?set(type_of(id)):*this;
}

Trait& Trait::set(Ty t) noexcept {
	m_ty = t;
	return *this;
}

Trait& Trait::reset() {
	m_traits = std::bitset<8>(default_traits);
	set("void",false);
	set(Ty::Void);
	return *this;
}

Trait& Trait::make_object(){
	return set(TraitIndex::Function,false).set(TraitIndex::Object);
}

Trait& Trait::make_function(){
	return set(TraitIndex::Object,false).set(TraitIndex::Function);
}

Trait& Trait::make_data(){
	return	 set(TraitIndex::Data)
			.set(TraitIndex::Accessor,false)
			.set(TraitIndex::Reference,false);
}

Trait& Trait::make_accessor(){
	return set(TraitIndex::Accessor)
			.set(TraitIndex::Data,false)
			.set_on_stack(false);
}

Trait& Trait::make_reference(){
	return set(TraitIndex::Reference)
			.set(TraitIndex::Data,false);
}

Trait& Trait::make_temp(){
	return set_on_stack(false).make_data();
}

Trait& Trait::set_dynamic(bool val){
	return set(TraitIndex::Dynamic,val);
}

Trait& Trait::set_on_stack(bool val){
	return set(TraitIndex::OnStack,val);
}

Trait& Trait::recv(const Trait& other){
	return set(TraitIndex::Object, other.is_object())
			.set(TraitIndex::Function, other.is_function())
			.set(other.type())
			.set(other.id());
}

Trait& Trait::as_ref_of(const Trait& other){
	return recv(other).make_reference();
}

bool Trait::is_number() const {
	return is(Ty::Real) || is(Ty::Bool) || is(Ty::Char) || is(Ty::Null);
}

bool Trait::is_lvalue() const {
	return is_accessor() || is_reference() || is_on_stack();
}

bool Trait::has_type_of(const Trait& other) const {
	return	(other.id() == id()) &&
			(other.type() == type()) &&
			(other.is_object() == is_object());

}

bool Trait::operator==(const Trait& other) const {
	return traits() == other.traits() &&
			id() == other.id() &&
			type() == other.type();
}

void Object::delete_ptr(){
	LFn;
	if (trait().is_data() && m_ptr){
		delete m_ptr;
	}
	m_ptr = nullptr;
	LVd;
}

Object::Object():
	m_ptr(VPTR_INIT),
	m_onstack(*this),
	m_trait(){
	LCtor("OBJ_DEFLT_CTOR");
}

Object::Object(const Object& o):
	m_ptr(o.trait().is_data()?new Var(o.data()):o.ptr()),
	m_onstack(o.trait().is_reference()?o.m_onstack.get():*this),
	m_trait(o.m_trait){
	Log ls("OBJECT CTOR DATA:") ls(to_string());
}

Var& Object::data() {
	return m_ptr?*m_ptr:
				 *(trait().make_data(),m_ptr = VPTR_INIT);
}

Object& Object::set(const Trait& t){
	LFn;
	m_trait = t;
	LRet *this;
}

Object& Object::set(const Var& var){
	LFn;
	delete_ptr();
	m_ptr = new Var(var);
	fn_init();
	LRet *this;
}

QString Object::to_string() const {
	if (!m_ptr)
		return "[UNALLOCATED_OBJECT]";
	switch(trait().type()){
		case Ty::String:
			return as<QString>();
		case Ty::Real:
			return QString::number(as<double>(),'g',10);
		case Ty::Bool:
			return as<bool>()?"true":"false";
		case Ty::Null:
			return "null";
		case Ty::Char:
			return QString(as<char>());
		case Ty::Void:
			return "[Void]";
		default:
			return QString("[").append(trait().id()).append(" instance]");
	}

}

bool Object::to_bool() const {
	return trait().is_number()?
				as<bool>():
				trait().is(Ty::String)?
					as<QString>().size():
					true;
}

Object Object::postfix(int i){
	if (!trait().is_lvalue() || !trait().is_number())
		return *this;
	double old = as<double>();
	Object copy(Trait(trait()).make_temp(),Var(old));
	data() = Var(old+i);
	return copy;
}

Object Object::prefix(int i) {
	if (!trait().is_number() || !trait().is_lvalue())
		return *this;
	data() = Var(as<double>()+i);
	return *this;
}

Object& Object::operator=(const Object& other){
	set(other.trait());
	if (other.trait().is_data()){
		set(other.data());
	}
	else{
		delete_ptr();
		m_ptr = other.ptr();
	}
	m_onstack = other.trait().is_reference()?other.m_onstack.get():*this;
	return *this;
}

Object& Object::recv(const Object& obj){
	LFn;
	data() = obj.data();
	trait().recv(obj.trait());
	if (trait().is_reference()){
		referenced().trait().recv(trait());
	}
	LRet *this;
}

Object& Object::referenced() {
	return trait().is_on_stack()?*this:m_onstack.get().referenced();
}

Object& Object::as_ref_of(Object& dest){
	delete_ptr();//if acsr, deletes the stackvar's ptr, else deletes
				 //this->ptr in [1] (as m_onstack is init'd w/ *this)
	referenced().trait().as_ref_of(dest.trait());//[3]
	referenced().m_onstack = dest.referenced();
	//Equivalent to:
	//(trait().is_accessor()?referenced():*this).trait().as_ref_of(dest.trait());
	//(trait().is_accessor()?referenced().m_onstack:m_onstack) = dest.referenced();
	m_ptr = dest.ptr();// [2]
	if (trait().is_accessor()){
		//Delete the pointer of the referenced.
		//this->delete_ptr() won't do anything
		referenced().delete_ptr(); // [1]
		//Copies trait from the variable in the Environment
		trait() = referenced().trait(); //See [3]
		//Have to make_accessor because currently
		//the trait is the same as the one of the
		//Object in the Environment
		trait().make_accessor();
		//the referenced copies the new pointer [2]
		referenced().m_ptr = m_ptr;
	}
	return *this;
}

Object& Object::as_acsr_of(Object& stk_var){
	as_ref_of(stk_var).trait().make_accessor();
	return *this;
}

Object::Object(const Trait& trt, const Var& var):
	m_ptr(nullptr),
	m_onstack(*this),
	m_trait(trt){
	set(var);
}

Object::Object(const QString& id, const Var& var):
	m_ptr(nullptr),
	m_onstack(*this),
	m_trait(Trait(id))
{
	set(var);
}

Object::Object(bool b):
	m_ptr(nullptr),
	m_onstack(*this),
	m_trait(Trait("bool")){
	set(Var(b));
}

Object::Object(char c):
	m_ptr(nullptr),
	m_onstack(*this),
	m_trait(Trait("char")){
	set(Var(c));
}

Object::Object(double d):
	m_ptr(nullptr),
	m_onstack(*this),
	m_trait(Trait("real")){
	LFn;
	set(Var(d));
	LVd;
}

Object::Object(const QString& str):
	m_ptr(nullptr),
	m_onstack(*this),
	m_trait(Trait("string")){
	set(Var(str));
}

Object Object::null(){
	Object ret;
	ret.trait().set(Ty::Null);
	return ret;
}

Object& Object::fn_init(){
	LFn;
	if (trait().is_function()){
		Logp("Contacting MereCallable");
		reinterpret_cast<MereCallable*>(m_ptr->data())->set_onstack(trait().is_on_stack());
	}
	LRet *this;
}

bool Object::match(const Object& other){
	if (other.trait().is("void"))
		return true;
	if (!trait().has_type_of(other.trait()))
		return false;
	return data() == other.data();
}

bool Object::operator==(Object& other) {
	return trait().has_type_of(other.trait()) && data() == other.data();
}

bool Object::operator>(const Object& other) const {
	return trait().is_number() && other.trait().is_number()?
				as<double>() > other.as<double>():false;
}

bool Object::operator<(const Object& other) const {
	return trait().is_number() && other.trait().is_number()?
				as<double>() < other.as<double>():false;
}

bool Object::operator==(const Object& other) const {
	return trait().has_type_of(other.trait()) && data() == other.data();
}


TraitMatcher::TraitMatcher():
	type_id("void"),
	type(Ty::Void){}

TraitMatcher::TraitMatcher(const QString& tid, Ty type):
	type_id(tid),
	type(type){}
