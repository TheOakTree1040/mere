#ifndef OBJECT_H
#define OBJECT_H
#include <QVariant>
#include "tlogger.h"
#include <functional>
#include <bitset>
using std::pair;
typedef QVariant Var;

#define VPTR_INIT new Var(false)

enum class Ty{		//storage class in c++
	Void	= -2,	//"[Empty object]"
	Struct	= -1,	//some specialized class which i need to create later
	Null,
	Function,		//MereCallable
	Real,			//double
	Char,			//char
	String,			//TString
	Bool,			//bool
	Array,			//QVector<Object>
	Map,			//QVector<std::pair<Object,Object>>
	Hash			//QHash<KeyTy(either numeral or string),Object>
};

//defines the trait of the object
enum class TraitIndex{

//Type Trait
	Object = 0,	//1
	Function,	//0
//Object Type
	Data,		//1
	Accessor,	//0
	Reference,	//0
	Dynamic,	//1
	OnStack,	//0
	_			//0
};
struct TraitMatcher;

class Trait{
	private://member fn
		void _reset_(){
			set(TraitIndex::Object		,	true	);
			set(TraitIndex::Function	,	false	);
			set(TraitIndex::Data		,	true	);
			set(TraitIndex::Accessor	,	false	);
			set(TraitIndex::Reference	,	false	);
			set(TraitIndex::Dynamic		,	true	);
			set(TraitIndex::OnStack		,	false	);
			set(Ty::Void);
			set("void",false);
		}

	private://fields
		std::bitset<8> m_traits;
		Ty m_ty;
		TString m_id;

	private://static data
		static QVector<TraitMatcher> type_lookup;
		static int default_traits;

	public://static fn
		static Ty type_of(const TString&);

	public://member fn
		Trait():
		m_traits(default_traits),
		m_ty(Ty::Void),
		m_id("void"){}

		Trait(const Trait& other):
		m_traits(other.traits()),
		m_ty(other.type()),
		m_id(other.id()){}

		Trait(const TString& id):
			m_traits(default_traits),
			m_ty(type_of(id)),
			m_id(id){}
		//setters
		Trait& set(TraitIndex i, bool val = true){
			m_traits.set(static_cast<size_t>(i),val);
			return *this;
		}
		Trait& set(const TString& id, bool searches = true){
			m_id = id;
			return searches?set(type_of(id)):*this;
		}
		Trait& set(Ty t) noexcept {
			m_ty = t;
			return *this;
		}
		Trait& reset() {
			m_traits = std::bitset<8>(default_traits);
			set("void",false);
			set(Ty::Void);
			return *this;
		}

		//bit getters
		bool is(TraitIndex i) const {
			return m_traits.test(static_cast<size_t>(i));
		}
		bool is_obj		() const {return is(TraitIndex::Object		);}
		bool is_fn		() const {return is(TraitIndex::Function	);}
		bool is_data	() const {return is(TraitIndex::Data		);}
		bool is_acsr	() const {return is(TraitIndex::Accessor	);}
		bool is_ref		() const {return is(TraitIndex::Reference	);}
		bool is_dynamic	() const {return is(TraitIndex::Dynamic	);}
		bool is_on_stack() const {return is(TraitIndex::OnStack	);}

		//field getters
		std::bitset<8> traits() const noexcept {
			return m_traits;
		}
		TString id() const noexcept {
			return m_id;
		}
		Ty type() const noexcept {
			return m_ty;
		}
		bool is(Ty ty) const noexcept {
			return m_ty == ty;
		}
		bool is(const TString& i) const noexcept {
			return m_id == i;
		}

		//bit setters
		Trait& as_obj(){
			return set(TraitIndex::Function,false)
					.set(TraitIndex::Object);
		}
		Trait& as_fn (){
			return set(TraitIndex::Object,false)
					.set(TraitIndex::Function);
		}
		Trait& as_data(){
			return	 set(TraitIndex::Data)
					.set(TraitIndex::Accessor,false)
					.set(TraitIndex::Reference,false);
		}
		Trait& as_acsr(){
			return set(TraitIndex::Accessor)
					.set(TraitIndex::Data,false)
					.set_on_stack(false);
		}
		Trait& as_ref(){
			return set(TraitIndex::Reference)
					.set(TraitIndex::Data,false);
		}
		Trait& as_temp(){
			return set_on_stack(false).as_data();
		}

		Trait& set_dynamic(bool val = true){
			return set(TraitIndex::Dynamic,val);
		}
		Trait& set_on_stack(bool val = true){
			return set(TraitIndex::OnStack,val);
		}

		Trait& recv(const Trait& other){
			return set(TraitIndex::Object, other.is_obj())
					.set(TraitIndex::Function, other.is_fn())
					.set(other.type())
					.set(other.id());
		}
		Trait& as_ref_of(const Trait& other){
			return recv(other).as_ref();
		}

		bool is_number() const {
			return is(Ty::Real) ||
					is(Ty::Bool) ||
					is(Ty::Char) ||
					is(Ty::Null);
		}
		bool is_lvalue() const {
			return is_acsr() || is_ref() || is_on_stack();
		}
		bool has_type_of(const Trait& other) const {
			return	(other.id() == id()) &&
					(other.type() == type()) &&
					(other.is_obj() == is_obj());

		}

		//operator
		bool operator==(const Trait& other) const {
			return traits() == other.traits() &&
					id() == other.id() &&
					type() == other.type();
		}
		bool operator!=(const Trait& other) const {
			return !(*this == other);
		}
};

struct TraitMatcher{
		TString type_id;
		Ty type;

		TraitMatcher():
			type_id("void"),
			type(Ty::Void){}

		TraitMatcher(const TString& tid, Ty type):
			type_id(tid),
			type(type){}

		bool match(const TString& id){
			return type_id == id;
		}
};

class Object{
	private://fields
		Var* m_ptr;
		std::reference_wrapper<Object> m_onstack;
		Trait m_trait;
	private://member fn
		void delete_ptr();
	public:
		Object():
		m_ptr(VPTR_INIT),
		m_onstack(*this),
		m_trait(){
#if _DEBUG
            Log << "OBJ_DEFLT_CTOR";
#endif
		}

		Object(const Object& o):
		m_ptr(o.trait().is_data()?new Var(o.data()):o.ptr()),
		m_onstack(o.trait().is_ref()?o.m_onstack.get():*this),
		m_trait(o.m_trait){
#if _DEBUG
			Log << "OBJECT CTOR DATA:" << to_string();
#endif
		}

		//getters
		Var& data() {
			return m_ptr?*m_ptr:
						 *(trait().as_data(),m_ptr = VPTR_INIT);
		}
		Var& data() const {
			return *m_ptr;
		}
		Var* ptr() const {
			return m_ptr;
		}
		Trait& trait() {
			return m_trait;
		}
		const Trait& trait() const {
			return m_trait;
		}

		//setters - pure assignment
		Object& set(const Trait& t){
			LFn;
			m_trait = t;
			LRet *this;
		}
        Object& set(const Var& var){
            delete_ptr();
            m_ptr = new Var(var);
			fn_init();
			LRet *this;
		}

		//ctor
		Object(const Trait& trt, const Var& var):
			m_ptr(nullptr),
			m_onstack(*this),
			m_trait(trt){
			set(var);
		}
		Object(const TString& id, const Var& var):
			m_ptr(nullptr),
			m_onstack(*this),
			m_trait(Trait(id))
		{
			set(var);
		}
		//ctor from literals
		Object(bool b):
			m_ptr(nullptr),
			m_onstack(*this),
			m_trait(Trait("bool")){
			set(Var(b));
		}
		Object(char c):
			m_ptr(nullptr),
			m_onstack(*this),
			m_trait(Trait("char")){
			set(Var(c));
		}
		Object(double d):
			m_ptr(nullptr),
			m_onstack(*this),
			m_trait(Trait("real")){
			LFn;
			set(Var(d));
			LVd;
		}
		Object(const TString& str):
			m_ptr(nullptr),
			m_onstack(*this),
			m_trait(Trait("string")){
			set(Var(str));
		}
		static Object null(){
			Object ret;
			ret.trait().set(Ty::Null);
			return ret;
		}

		//dtor
		~Object(){
			delete_ptr();
		}

		template <typename T>
		T as() {
			//LFn;
			return data().value<T>();
		}

		template <typename T>
		T as() const {
			return m_ptr->value<T>();
		}

		TString to_string() {
//			LFn;
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

		bool to_bool() {
			return trait().is_number()?
						as<bool>():
						trait().is(Ty::String)?
							as<TString>().size():
							true;
		}

		Object postfix(int i){
			if (!trait().is_lvalue() || !trait().is_number())
				return *this;
			double old = as<double>();
			Object copy(Trait(trait()).as_temp(),Var(old));
			data() = Var(old+i);
			return copy;
		}

		Object prefix(int i) {
			if (!trait().is_number() || !trait().is_lvalue())
				return *this;
			data() = Var(as<double>()+i);
			return *this;
		}

		//copy
		Object& operator=(const Object& other){
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

		Object& recv(const Object& obj){
			LFn;
			data() = obj.data();
			trait().recv(obj.trait());
			if (trait().is_ref()){
				referenced().trait().recv(trait());
			}
			LRet *this;
		}
		Object& referenced(){
			return trait().is_on_stack()?*this:m_onstack.get().referenced();
		}

		Object& as_ref_of(Object& dest){
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
		Object& as_acsr_of(Object& stk_var){
			as_ref_of(stk_var).trait().as_acsr();
			return *this;
		}

		Object& fn_init();

		bool operator==(const Object& other) const {
			return trait().has_type_of(other.trait()) && data() == other.data();
		}
		bool operator==(Object& other) {
			return trait().has_type_of(other.trait()) && data() == other.data();
		}
		bool operator>(const Object& other) const {
			return trait().is_number()&&other.trait().is_number()?
				as<double>() > other.as<double>():false;
		}
		bool operator<(const Object& other) const {
			return other > *this;
		}
};

Q_DECLARE_METATYPE(Object)
#endif // OBJECT_H
