#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <QHash>
#include "object.h"

namespace mere {
	typedef std::vector<std::pair<Object,Object>> ObjectAssocData;
	class ObjectAssoc final{
		private_methods:
			typedef ObjectAssocData OADat;
			OADat m_data;
		public_methods:
			ObjectAssoc(const OADat& d):m_data(d){}
			ObjectAssoc():m_data(){}

			void push_back(const Object& k, const Object& val);

			Object value(const Object& k) const;
			bool contains(const Object& k) const;
			Object key(const Object& val) const;

			int size() const { return m_data.size(); }
	};
	typedef ObjectAssoc Map;

	template <typename Key>
	class HashMap final{
		private_fields:
			QHash<Key,Object> map;
		public_methods:
			HashMap():map(){}
			HashMap(const QHash<Key,Object>& m):map(m){}

			auto insert(const Key& k, const Object& o){ return map.insert(k,o); }
			int remove(const Key& k){ return map.remove(k); }

			bool contains(const Key& k) const { return map.contains(k); }
			int size() const { return map.size(); }

			Object& operator[](const Key& k) const { return map[k]; }
	};
}


#endif // DATA_STORAGE_H
