#ifndef OBJECT_H
#define OBJECT_H
#include <QVariant>
#include <QString>
#include "logger.h"
#include <functional>

using std::pair;
enum class TyTrait{
	Object,
	Ref,
	Function,
	Regex
};

#define is_regex is(TyTrait::Regex)

enum class Ty{		//storage class in c++
	Void	= -2,	//"[Empty object]"
	Struct	= -1,	//some specialized class which i need to create later
	Null,
	Real,			//double
	Char,			//char
	String,			//QString
	Bool,			//bool
	Array,			//QVector<Object>
	Map,			//QVector<std::pair<Object,Object>>
	Hash			//QHash<KeyTy(either numeral or string),Object>
  //Ptr,
  //Ref,
  //Set,
};

struct Trait{
	public:
		TyTrait ty_trait_;
		Ty ty;
		union{
				bool is_dynamic_;
				QRegExp* exp;
		};
		QString id_;
	public:
		static QVector<Trait> _nat_lookup;
		static Ty type_of(const QString&);
	public:
		~Trait(){}
		Trait():
			ty_trait_(TyTrait::Object),
			ty(Ty::Void),
			is_dynamic_(false),
			id_("void"){}
		Trait(const Trait& other):
			ty_trait_(other.ty_trait_),
			id_(other.id_){
			switch(other.ty_trait()){
				case TyTrait::Ref:
				case TyTrait::Object:
				case TyTrait::Function:
					ty = other.ty;
					is_dynamic_ = other.is_dynamic_;
					break;
				case TyTrait::Regex:
					exp = other.exp;
					break;
			}
		}
		Trait(const QRegExp& ex, Ty t):
			ty_trait_(TyTrait::Regex),
			ty(t),
			exp(new QRegExp(ex)),
			id_(ex.pattern()){}
		Trait(const QString& str):
			ty_trait_(TyTrait::Object),
			ty(type_of(str)),
			is_dynamic_(false),
			id_(str){}

		bool match(const QString& str) const{
			if (str.isEmpty() || !is_regex)
				return false;
			return exp->exactMatch(str);
		}

		bool is_typed()const{
			return !is_regex && !id_.isEmpty() && !(id_ == "void");
		}

		Ty type()const{
			if (is_regex)
				return Ty::Void;
			return ty;
		}

		TyTrait ty_trait() const{
			return ty_trait_;
		}

		bool is(const QString& _id)const{
			return _id == id_;
		}

		bool is(Ty t)const{
			if (ty_trait_ == TyTrait::Regex)
				return false;
			return ty == t;
		}

		bool is(TyTrait tt)const{
			return ty_trait_ == tt;
		}

		bool is(bool dynamicness)const{
			return is_dynamic_ == dynamicness;
		}

		void set_dynamic(bool d){
			is_dynamic_ = d;
		}

		bool is_dynamic()const{
			return is_dynamic_;
		}

		void set_ty_trait(TyTrait tt){
			ty_trait_ = tt;
		}

		QString id() const{
			return id_;
		}

		bool operator==(const Trait& other) const{
			if (is_regex)
				return match(other.id());
			if (other.is(TyTrait::Regex))
				return other == *this;
			return is(other.ty_trait()) &&
					is(other.id())&&
					is(other.is_dynamic()) &&
					is(other.type());
		}
};
class Void{

};
Q_DECLARE_METATYPE(Void)
class Object{
	public:
		union{
			QVariant* dat_;
			std::reference_wrapper<Object> ref;
		};
		Trait trait;
	public:
		Object():dat_(new QVariant()),trait(){}
		Object(const Object& other):trait(other.trait){
			if (other.trait.is(TyTrait::Ref))
				ref = other.ref;
			else
				dat_ = new QVariant(*(other.dat_));
		}
		Object& refer(Object& r){
			if (!trait.is(TyTrait::Ref))
				delete dat_;
			ref = r;
			trait = r.trait;
			trait.set_ty_trait(TyTrait::Ref);
			return *this;
		}
		QVariant dat() const{
			if (dat_)
				return *dat_;
			else return QVariant();
		}

		Object(const Trait& trt, const QVariant& qvar): dat_(new QVariant(qvar)),trait(trt){}
		Object(const QString& trt, const QVariant& qvar){
			Object(Trait(trt),qvar);
		}
		Object(bool b){
			Trait t;
			t.id_ = "bool";
			t.ty = Ty::Bool;
			dat_ = new QVariant(b);
		}
		Object(char c){
			Trait t;
			t.id_ = "char";
			t.ty = Ty::Char;
			dat_ = new QVariant(c);
		}
		Object(double d){
			Trait t;
			t.id_ = "real";
			t.ty = Ty::Real;
			dat_ = new QVariant(d);
		}
		Object(const QString& str){
			Trait t;
			t.id_ = "string";
			t.ty = Ty::String;
			dat_ = new QVariant(str);
		}

		~Object(){
			if (!trait.is(TyTrait::Ref))
				delete dat_;
		}

		template <typename T>
		T value() const{
			if (dat_)
				return (trait.is(TyTrait::Ref)?ref.get().value<T>():dat().value<T>());
			return T();
		}

		bool is_number() const{
			return trait.is(Ty::Real) ||
					trait.is(Ty::Bool) ||
					trait.is(Ty::Char) ||
					trait.is(Ty::Null);
		}

		QString to_string() const{
			if (!dat_)
				return "[ERROR_EMPTY_OBJECT]";
			switch(trait.ty){
				case Ty::String:
					return value<QString>();
				case Ty::Real:
					return QString::number(value<double>());
				case Ty::Bool:
					return value<bool>()?"true":"false";
				case Ty::Null:
					return "null";
				default:
					return "\"\"";
			}
		}

		bool to_bool() const{
			if (trait.is(Ty::Null))
				return false;
			if (is_number())
				return value<bool>();
			return true;
		}

		Object postfix(int i){
			if (!(trait.is(TyTrait::Ref) && !is_number()))
				return Object("void",0);
			double val = ref.get().value<double>();
			Object o(val);
			ref.get().dat_ptr()->setValue(val+i);
			return o;
		}

		Object prefix(int i) {
			if (!(trait.is(TyTrait::Ref) && !ref.get().is_number()))
				return Object();
			ref.get().dat_ptr()->setValue(ref.get().value<double>()+i);
			return *this;
		}

		QVariant* dat_ptr() const{
			return trait.is(TyTrait::Ref)?ref.get().dat_ptr():dat_;
		}

		bool operator==(const Object& other) const{
			if (!(trait==other.trait))
				return false;
			if (trait.is(TyTrait::Ref))
				return ref.get() == other.ref.get();
			if (!dat_)
				return !(other.dat_);
			return dat() == other.dat();
		}
		bool operator>(const Object& other) const{
			if (is_number()&&other.is_number())
				return value<double>() > other.value<double>();
			return false;
		}
		bool operator<(const Object& other) const{
			return other > *this;
		}
};
Q_DECLARE_METATYPE(Object)
#endif // OBJECT_H
