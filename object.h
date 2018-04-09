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
		~Trait(){
			Log << "   ~Trait() start";
			if (is(TyTrait::Regex))
				delete exp;
			Log << "   ~Trait() returned";
		}
		Trait():
			ty_trait_(TyTrait::Object),
			ty(Ty::Void),
			is_dynamic_(false),
			id_("void"){}
		Trait(const Trait& other):
			ty_trait_(other.ty_trait_),
			ty(other.ty),
			id_(other.id_){
			if (other.is(TyTrait::Regex)){
				exp = new QRegExp(*(other.exp));
			}
			else {
				is_dynamic_ = other.is_dynamic_;
			}
		}
		Trait& operator=(const Trait& other){
			ty_trait_ = other.ty_trait_;
			id_ = other.id_;
			ty = other.ty;
			if (other.is(TyTrait::Regex)){
				exp = new QRegExp(*(other.exp));
			}
			else {
				is_dynamic_ = other.is_dynamic_;
			}
			return *this;
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
			if (!is(TyTrait::Regex))
				return false;
			return exp&&!str.isEmpty()?exp->exactMatch(str):false;
		}
		bool is_typed()const{
			return !is(TyTrait::Regex) && !is("void") && id_.size();
		}
		Ty type()const{
			return ty;
		}
		TyTrait ty_trait() const{
			return ty_trait_;
		}
		bool is(const QString& _id)const{
			return id_ == _id;
		}
		bool is(Ty t)const{
			return type() == t;
		}
		bool is(TyTrait tt)const{
			return ty_trait() == tt;
		}
		bool is(bool dynamicness)const{
			if (is(TyTrait::Regex))
				return false;
			return is_dynamic() == dynamicness;
		}
		void set_dynamic(bool d){
			if (!is(TyTrait::Regex))
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
		bool operator==(const Trait& other)const{
			if (!(type() == other.type() &&
				  ty_trait() == other.ty_trait() &&
				  id() == other.id()))
				return false;
			return (!is(TyTrait::Regex))?is(other.is_dynamic()):true;
		}
};

class Object{
	public:
		union{
			QVariant* dat_;
			std::reference_wrapper<Object> ref;
		};
		Trait trait;
	public:
		Object(){
			trait = Trait("void");
			dat_ = new QVariant();
		}

		Object(const Object& other){
			this->trait = other.trait;
			if (other.trait.is(TyTrait::Ref))
				ref = other.ref;
			else
				dat_ = new QVariant(other.dat());
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
			else return QVariant("[VOID]");
		}

		Object(const Trait& trt, const QVariant& qvar){
			trait = trt;
			dat_ = new QVariant(qvar);
		}

		Object(const QString& trt, const QVariant& qvar){
			Object(Trait(trt),qvar);
		}
		Object(bool b){
			trait = Trait("bool");
			dat_ = new QVariant(b);
		}
		Object(char c){
			trait = Trait("char");
			dat_ = new QVariant(c);
		}
		Object(double d){
			trait = Trait("real");
			dat_ = new QVariant(d);
		}
		Object(const QString& str){
			trait = Trait("string");
			dat_ = new QVariant(str);
		}

		~Object(){
			LFn << trait.id();
			if (dat_)
				delete dat_;
			Logger::indent--;
			Log << "~Object() returned";
		}

		template <typename T>
		T value() const{
			return (trait.is(TyTrait::Ref)?ref.get().value<T>():dat().value<T>());
		}

		bool is_number() const{
			return trait.is(Ty::Real) ||
					trait.is(Ty::Bool) ||
					trait.is(Ty::Char) ||
					trait.is(Ty::Null);
		}

		QString to_string() const{
			if (trait.is(TyTrait::Ref))
				return ref.get().to_string();
			if (!dat_)
				return "[#BROKEN_OBJECT#]";
			switch(trait.ty){
				case Ty::String:
					return value<QString>();
				case Ty::Real:
					return QString::number(value<double>());
				case Ty::Bool:
					return value<bool>()?"true":"false";
				case Ty::Null:
					return "null";
				case Ty::Char:
					return QString(value<char>());
				case Ty::Void:
					return "[#VOID#]";
				default:
					return QString("[#").append(trait.id()).append(" instance]");
			}
		}

		bool to_bool() const{
			return is_number()?value<bool>():true;
		}

		Object postfix(int i){
			if (!trait.is(TyTrait::Ref) || !is_number())
				return Object();
			double val = ref.get().value<double>();
			Object o(val);
			ref.get().dat_ptr()->setValue(val+i);
			return o;
		}

		Object prefix(int i) {
			if (!trait.is(TyTrait::Ref) || !is_number())
				return Object();
			ref.get().dat_ptr()->setValue(ref.get().value<double>()+i);
			return *this;
		}

		QVariant* dat_ptr() const{
			return trait.is(TyTrait::Ref)?ref.get().dat_ptr():dat_;
		}
		Object& operator=(const Object& other){
			this->trait = other.trait;
			if (other.trait.is(TyTrait::Ref))
				ref = other.ref;
			else
				dat_ = new QVariant(other.dat());
			return *this;
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
