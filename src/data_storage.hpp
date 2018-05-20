#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <QHash>

#include "object.h"

namespace mere {
	typedef std::vector<pair<Object,Object>> ObjectAssocData;
	class ObjectAssoc final{
			typedef ObjectAssocData OADat;
			OADat m_data;
		public:
			ObjectAssoc(OADat d):m_data(d){}
			ObjectAssoc():m_data(){}
			Object value(const Object& k){
				for (uint i = 0u; i != m_data.size(); i++){
					if (m_data.at(i).first == k)
						return m_data.at(i).second;
				}
				return Object();
			}
			bool contains(const Object& k){
				for (uint i = 0u; i != m_data.size(); i++){
					if (m_data.at(i).first == k)
						return true;
				}
				return false;
			}
			Object key(const Object& val){
				for (uint i = 0u; i != m_data.size(); i++){
					if (m_data.at(i).second == val)
						return m_data.at(i).first;
				}
				return Object();
			}
			void push_back(const Object& k, const Object& val){
				m_data.push_back(pair<Object,Object>(k,val));
			}
			int size(){
				return m_data.size();
			}
	};
	typedef ObjectAssoc Map;

	template <typename Key>
	class HashMap final{
		private:
			QHash<Key,Object> map;
		public:
			HashMap():map(){}
			HashMap(QHash<Key,Object> m):map(m){}
			auto insert(const Key& k, const Object& o){
				return map.insert(k,o);
			}
			bool contains(const Key& k){
				return map.contains(k);
			}
			int remove(const Key& k){
				return map.remove(k);
			}
			Object& operator[](const Key& k)const{
				return map[k];
			}
			int size(){
				return map.size();
			}
	};
}


#endif // DATA_STORAGE_H
