#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include "isingleton.h"
#include <typeindex>
#include <unordered_map>

namespace Singleton
{
	template <typename T>
	T* GetInstance()
	{
		static std::unordered_map<std::type_index, ISingleton*> _instances;

		if (!std::is_base_of<ISingleton, T>::value) {
			return nullptr;
		}

		auto it = _instances.find(typeid(T));
		T* instance = (it != _instances.end()) ? static_cast<T*>(it->second) : nullptr;

		if (instance == nullptr) {
			instance = new T;
			_instances[typeid(T)] = instance;
		}

		return instance;
	}
}

#endif
