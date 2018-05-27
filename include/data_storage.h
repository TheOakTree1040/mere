#pragma once

#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

template <class Key, class T>
class QHash;

#include <vector>

namespace mere {
	class Object;

	using ORelation = std::vector<std::pair<Object,Object>>;

	template <typename Key>
	using OHash = QHash<Key,Object>;
}


#endif // DATA_STORAGE_H
