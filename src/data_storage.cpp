#include "data_storage.h"
//================================ObjectAssoc=============================
mere::Object mere::ObjectAssoc::value(const mere::Object& k) const {
	for (uint i = 0u; i != m_data.size(); i++){
		if (m_data.at(i).first == k)
			return m_data.at(i).second;
	}
	return Object();
}

bool mere::ObjectAssoc::contains(const mere::Object& k) const {
	for (uint i = 0u; i != m_data.size(); i++){
		if (m_data.at(i).first == k)
			return true;
	}
	return false;
}

mere::Object mere::ObjectAssoc::key(const mere::Object& val) const {
	for (uint i = 0u; i != m_data.size(); i++){
		if (m_data.at(i).second == val)
			return m_data.at(i).first;
	}
	return Object();
}

void mere::ObjectAssoc::push_back(const mere::Object& k, const mere::Object& val) {
	m_data.push_back(pair<Object,Object>(k,val));
}

//================================HashMap===============================
