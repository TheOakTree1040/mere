#ifndef OBJECT_H
#define OBJECT_H

#include <QVariant>
#include <functional>
#include <bitset>
#include "tlogger.h"

using std::pair;

namespace mere {
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
		Array,			//std::vector<Object>
		Map,			//std::vector<std::pair<Object,Object>>
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
		private:
			std::bitset<8> m_traits;
			Ty m_ty;
			TString m_id;

			static std::vector<TraitMatcher> type_lookup;
			static int default_traits;

		public://static fn
			static Ty type_of(const TString&);

			Trait();
			Trait(const Trait& other);
			Trait(const TString& id);
			//setters
			Trait& set(TraitIndex i, bool val = true);
			Trait& set(const TString& id, bool searches = true);
			Trait& set(Ty t) noexcept;
			Trait& reset();

			//bit getters
			bool is(TraitIndex i) const { return m_traits.test(static_cast<size_t>(i));}
			bool is_obj		() const { return is(TraitIndex::Object		); }
			bool is_fn		() const { return is(TraitIndex::Function	); }
			bool is_data	() const { return is(TraitIndex::Data		); }
			bool is_acsr	() const { return is(TraitIndex::Accessor	); }
			bool is_ref		() const { return is(TraitIndex::Reference	); }
			bool is_dynamic	() const { return is(TraitIndex::Dynamic	); }
			bool is_on_stack() const { return is(TraitIndex::OnStack	); }

			//field getters
			std::bitset<8> traits() const noexcept { return m_traits; }
			TString id() const noexcept { return m_id; }
			Ty type() const noexcept { return m_ty; }
			bool is(Ty ty) const noexcept { return m_ty == ty; }
			bool is(const TString& i) const noexcept { return m_id == i; }

			//bit setters
			Trait& as_obj();
			Trait& as_fn ();
			Trait& as_data();
			Trait& as_acsr();
			Trait& as_ref();
			Trait& as_temp();

			Trait& set_dynamic(bool val = true);
			Trait& set_on_stack(bool val = true);

			Trait& recv(const Trait& other);
			Trait& as_ref_of(const Trait& other);

			bool is_number() const;
			bool is_lvalue() const;
			bool has_type_of(const Trait& other) const;

			//operator
			bool operator==(const Trait& other) const;
			bool operator!=(const Trait& other) const { return !(*this == other); }
	};

	struct TraitMatcher{
		public:
			TString type_id;
			Ty type;
		public:
			TraitMatcher();
			TraitMatcher(const TString& tid, Ty type);

			bool match(const TString& id){ return type_id == id; }
	};

	class EnvImpl;
	class Object{
			friend class EnvImpl;
		private:
			Var* m_ptr;
			std::reference_wrapper<Object> m_onstack;
			Trait m_trait;
		private:
			void delete_ptr();
			Object& fn_init();
		public:
			Object();
			Object(const Object& o);
			Object(const Trait& trt, const Var& var);
			Object(const TString& id, const Var& var);
			Object(bool b);
			Object(char c);
			Object(double d);
			Object(const TString& str);
			static Object null();

			~Object(){ delete_ptr(); }

			//getters
			Var& data();
			Var& data() const { return *m_ptr; }
			Var* ptr() const { return m_ptr; }
			Trait& trait() { return m_trait; }
			const Trait& trait() const { return m_trait; }

			//setters - pure assignment
			Object& set(const Trait& t);
			Object& set(const Var& var);

			template <typename T> T as() { return data().value<T>(); }
			template <typename T> T as() const { return m_ptr->value<T>(); }

			TString to_string() const;
			bool to_bool() const;

			Object postfix(int i);
			Object prefix(int i);

			Object& referenced();

			Object& as_ref_of(Object& dest);
			Object& as_acsr_of(Object& stk_var);
			Object& recv(const Object& obj);

			bool match(const Object& other);

			Object& operator=(const Object& other);
			bool operator==(const Object& other) const;
			bool operator==(Object& other);
			bool operator>(const Object& other) const;
			bool operator<(const Object& other) const;
	};
}
Q_DECLARE_METATYPE(mere::Object)
#endif // OBJECT_H
