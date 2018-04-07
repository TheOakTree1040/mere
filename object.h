#ifndef OBJECT_H
#define OBJECT_H
#include <QVariant>
#include <QString>
#include <QDebug>
#include <functional>

using std::pair;
enum class TyTrait{
	Object,
	Ref,
	Function,
	Regex
};

#define is_regex() is(TyTrait::Regex)

enum class Ty{		//storage class in c++
	Void	= -2,	//"[Empty object]"
	Struct	= -1,	//some specialized class which i need to create later
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

class Trait{
	private:
		TyTrait ty_trait_;
		union{
				struct{
						Ty ty;
						bool is_dynamic_;
				};
				QRegExp exp;
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
			switch(trait){
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
		Trait(const QRegExp& ex, Ty i):
			ty_trait_(TyTrait::Regex),
			exp(ex),
			id_(ex.pattern()){}
		Trait(const QString& str):
			ty_trait_(TyTrait::Object),
			ty(type_of(str)),
			is_dynamic_(false),
			id_(str){}

		bool match(const QString& str) const{
			if (str.isEmpty() || trait != TyTrait::Regex)
				return false;
			return exp.exactMatch(str);
		}

		bool is_typed(){
			return !is_regex() && !id_.isEmpty() && !id_ == "void";
		}

		Ty type(){
			if (is_regex())
				return Ty::Void;
			return ty;
		}

		TyTrait ty_trait(){
			return ty_trait_;
		}

		bool is(const QString& _id){
			return _id == id_;
		}

		bool is(Ty t){
			if (ty_trait_ == TyTrait::Regex)
				return false;
			return ty == t;
		}

		bool is(TyTrait tt){
			return ty_trait_ == tt;
		}

		bool is(bool dynamicness){
			return is_dynamic_ == dynamic;
		}

		void set_dynamic(bool d){
			is_dynamic = d;
		}

		bool is_dynamic(){
			return is_dynamic_;
		}

		void set_ty_trait(TyTrait tt){
			ty_trait_ = tt;
		}

		QString id(){
			return id_;
		}

		bool operator==(const Trait& other) const{
			if (is_regex())
				return match(other.id());

			return is(other.ty_trait()) &&
					is(other.id())&&
					is(other.is_dynamic()) &&
					is(other.type());
		}
};

class Object{
	public:
		union{
			QVariant dat;
			std::reference_wrapper<Object> ref;
		};
		Trait trait;
	public:
		Object():dat("[Empty object]"),trait(){}
		Object(const Object& other):dat(other.dat),trait(other.trait){}
		Object refer(Object& r){
			ref = r;
			trait = r.trait;
			trait.set_ty_trait(TyTrait::Ref);
		}

		Object(const Trait& trt, const QVariant& qvar): dat(qvar),trait(trt){}
		Object(const QString& trt, const QVariant& qvar){
			Object(Trait(trt),qvar);
		}
		Object(bool b){
			Trait t;
			t.id_ = "bool";
			t.ty = Ty::Bool;
			dat = b;
		}
		Object(char c){
			Trait t;
			t.id_ = "char";
			t.ty = Ty::Char;
			dat = c;
		}
		Object(double d){
			Trait t;
			t.id_ = "real";
			t.ty = Ty::Real;
			dat = d;
		}
		Object(const QString& str){
			Trait t;
			t.id_ = "string";
			t.ty = Ty::String;
			dat = str;
		}

		~Object(){}

		template <typename T>
		T value() const{
			return (trait.is(TyTrait::Ref)?ref.get().value<T>():dat.value<T>());
		}

		bool is_number() const{
			return trait.is(Ty::Real) || trait.is(Ty::Bool) || trait.is(Ty::Char);
		}

		QString to_string() const{
			switch(trait.ty){
				case Ty::String:
					return value<QString>();
				case Ty::Real:
					return QString::number(value<double>());
				case Ty::Bool:
					return value<bool>()?"true":"false";
				default:
					return "\"\"";
			}
		}

		bool to_bool(){
			if (is_number())
				return value<bool>();
		}

		Object postfix(int i){
			if (!trait.is(TyTrait::Ref) || !is_number())
				return Object();
			double val = ref->value<double>();
			Object o(val);
			ref->dat.setValue(++val);
			return o;
		}

		Object prefix(int i){
			if (!trait.is(TyTrait::Ref) || !ref->is_number())
				return Object();
			ref->dat.setValue(ref->value<double>()+1);
			return *this;
		}


		bool operator==(const Object& other) const{
			if (!trait==other.trait)
				return false;
			if (is(TyTrait::Ref))
				return ref.get() == other.ref.get();
			return dat == other.dat;
		}
		bool operator>(const Object& other) const{
			if (is_number()&&other.is_number())
				return value<double>() > other.value<double>();
			return false;
		}
};
Q_DECLARE_METATYPE(Object)
#endif // OBJECT_H
