#include "object.h"
#include <vector>
#include "merecallable.h"

using namespace mere;
int Trait::default_traits = 0b10100100;
std::vector<TraitMatcher> Trait::type_lookup(
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

Trait::Trait():
	m_traits(default_traits),
	m_ty(Ty::Void),
	m_id("void"){}

Trait::Trait(const Trait& other):
	m_traits(other.traits()),
	m_ty(other.type()),
	m_id(other.id()){}

Trait::Trait(const TString& id):
	m_traits(default_traits),
	m_ty(type_of(id)),
	m_id(id){}

Trait& Trait::set(TraitIndex i, bool val){
	m_traits.set(static_cast<size_t>(i),val);
	return *this;
}

Trait& Trait::set(const TString& id, bool searches){
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

Trait& Trait::as_obj(){
	return set(TraitIndex::Function,false).set(TraitIndex::Object);
}

Trait& Trait::as_fn(){
	return set(TraitIndex::Object,false).set(TraitIndex::Function);
}

Trait& Trait::as_data(){
	return	 set(TraitIndex::Data)
			.set(TraitIndex::Accessor,false)
			.set(TraitIndex::Reference,false);
}

Trait& Trait::as_acsr(){
	return set(TraitIndex::Accessor)
			.set(TraitIndex::Data,false)
			.set_on_stack(false);
}

Trait& Trait::as_ref(){
	return set(TraitIndex::Reference)
			.set(TraitIndex::Data,false);
}

Trait& Trait::as_temp(){
	return set_on_stack(false).as_data();
}

Trait& Trait::set_dynamic(bool val){
	return set(TraitIndex::Dynamic,val);
}

Trait& Trait::set_on_stack(bool val){
	return set(TraitIndex::OnStack,val);
}

Trait& Trait::recv(const Trait& other){
	return set(TraitIndex::Object, other.is_obj())
			.set(TraitIndex::Function, other.is_fn())
			.set(other.type())
			.set(other.id());
}

Trait& Trait::as_ref_of(const Trait& other){
	return recv(other).as_ref();
}

bool Trait::is_number() const {
	return is(Ty::Real) || is(Ty::Bool) || is(Ty::Char) || is(Ty::Null);
}

bool Trait::is_lvalue() const {
	return is_acsr() || is_ref() || is_on_stack();
}

bool Trait::has_type_of(const Trait& other) const {
	return	(other.id() == id()) &&
			(other.type() == type()) &&
			(other.is_obj() == is_obj());

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
	m_onstack(o.trait().is_ref()?o.m_onstack.get():*this),
	m_trait(o.m_trait){
	Log ls("OBJECT CTOR DATA:") ls(to_string());
}

Var& Object::data() {
	return m_ptr?*m_ptr:
				 *(trait().as_data(),m_ptr = VPTR_INIT);
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

TString Object::to_string() const {
	if (!m_ptr)
		return "[UNALLOCATED_OBJECT]";
	switch(trait().type()){
		case Ty::String:
			return as<TString>();
		case Ty::Real:
			return TString::number(as<double>(),'g',10);
		case Ty::Bool:
			return as<bool>()?"true":"false";
		case Ty::Null:
			return "null";
		case Ty::Char:
			return TString(as<char>());
		case Ty::Void:
			return "[Void]";
		default:
			return TString("[").append(trait().id()).append(" instance]");
	}

}

bool Object::to_bool() const {
	return trait().is_number()?
				as<bool>():
				trait().is(Ty::String)?
					as<TString>().size():
					true;
}

Object Object::postfix(int i){
	if (!trait().is_lvalue() || !trait().is_number())
		return *this;
	double old = as<double>();
	Object copy(Trait(trait()).as_temp(),Var(old));
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
	m_onstack = other.trait().is_ref()?other.m_onstack.get():*this;
	return *this;
}

Object& Object::recv(const Object& obj){
	LFn;
	data() = obj.data();
	trait().recv(obj.trait());
	if (trait().is_ref()){
		referenced().trait().recv(trait());
	}
	LRet *this;
}

Object& Object::referenced() {
	return trait().is_on_stack()?*this:m_onstack.get().referenced();
}

Object& Object::as_ref_of(Object& dest){
	delete_ptr();//if acsr, deletes the stackvar's ptr, else deletes
	//this->ptr (as m_onstack is init'd w/ *this)
	(trait().is_acsr()?referenced():*this).trait().as_ref_of(dest.trait());
	(trait().is_acsr()?referenced().m_onstack:m_onstack) = dest.referenced();
	m_ptr = dest.ptr();
	if (trait().is_acsr()){
		referenced().delete_ptr();
		trait() = referenced().trait();
		trait().as_acsr();
		referenced().m_ptr = m_ptr;
	}
	return *this;
}

Object& Object::as_acsr_of(Object& stk_var){
	as_ref_of(stk_var).trait().as_acsr();
	return *this;
}

Object::Object(const Trait& trt, const Var& var):
	m_ptr(nullptr),
	m_onstack(*this),
	m_trait(trt){
	set(var);
}

Object::Object(const TString& id, const Var& var):
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

Object::Object(const TString& str):
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
	if (trait().is_fn()){
		Logp("Contacting MereCallable");
		reinterpret_cast<MereCallable*>(m_ptr->data())->set_onstack(trait().is_on_stack());
	}
	LRet *this;
}

bool Object::match(const Object& other){
	if (other.trait().is("void"))
		return true;
	if (!other.trait().has_type_of(other.trait()))
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

TraitMatcher::TraitMatcher(const TString& tid, Ty type):
	type_id(tid),
	type(type){}
