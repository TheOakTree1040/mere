#pragma once

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
		String,			//QString
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
			QString m_id;

			static std::vector<TraitMatcher> type_lookup;
			static int default_traits;

		public://static fn
			static Ty type_of(const QString&);

			Trait();
			Trait(const Trait& other);
			Trait(const QString& id);
			//setters
			Trait& set(TraitIndex i, bool val = true);
			Trait& set(const QString& id, bool searches = true);
			Trait& set(Ty t) noexcept;
			Trait& reset();

			//bit getters
			bool is(TraitIndex i) const { return m_traits.test(static_cast<size_t>(i));}
			bool is_object		() const { return is(TraitIndex::Object		); }
			bool is_function	() const { return is(TraitIndex::Function	); }
			bool is_data		() const { return is(TraitIndex::Data		); }
			bool is_accessor	() const { return is(TraitIndex::Accessor	); }
			bool is_reference	() const { return is(TraitIndex::Reference	); }
			bool is_dynamic		() const { return is(TraitIndex::Dynamic	); }
			bool is_on_stack	() const { return is(TraitIndex::OnStack	); }

			//getters
			std::bitset<8> traits() const noexcept { return m_traits; }
			QString id() const noexcept { return m_id; }
			Ty type() const noexcept { return m_ty; }
			bool is(Ty ty) const noexcept { return m_ty == ty; }
			bool is(const QString& i) const noexcept { return m_id == i; }

			//bit setters
			Trait& make_object();
			Trait& make_function ();
			Trait& make_data();
			Trait& make_accessor();
			Trait& make_reference();
			Trait& make_temp();

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
			QString type_id;
			Ty type;
		public:
			TraitMatcher();
			TraitMatcher(const QString& tid, Ty type);

			bool match(const QString& id){ return type_id == id; }
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
			Object(const QString& id, const Var& var);
			Object(bool b);
			Object(char c);
			Object(double d);
			Object(const QString& str);
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

			QString to_string() const;
			bool to_bool() const;

			Object postfix(int i);
			Object prefix(int i);

			Object& referenced();
			const Object& referenced() const;

			Object& make_reference_of(Object& dest);
			Object& make_accessor_of(Object& stk_var);
			Object& receive_from(const Object& obj);

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
