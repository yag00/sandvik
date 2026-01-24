#ifndef __STRING_POOL_H__
#define __STRING_POOL_H__

#include <mutex>
#include <string>
#include <unordered_map>

#include "object.hpp"
#include "system/singleton.hpp"

namespace sandvik {
	/** @class StringPool
	 * @brief Singleton class to manage interned Java String objects.
	 *
	 * The StringPool class provides functionality to intern strings, ensuring that
	 * identical strings share the same ObjectRef instance.
	 */
	class StringPool : public Singleton<StringPool> {
			friend class Singleton<StringPool>;

		public:
			/** Retrieve or add a string to the pool
			 * @param classloader_ Reference to the ClassLoader
			 * @param str_ String to intern
			 * @return Reference to the interned String object
			 */
			ObjectRef intern(ClassLoader& classloader_, const std::string& str_) {
				std::lock_guard lock(_mutex);

				auto it = _pool.find(str_);
				if (it != _pool.end()) {
					return it->second;
				}

				// Create new String object
				ObjectRef obj = Object::make(classloader_, str_);
				_pool[str_] = obj;
				return obj;
			}

			/** Mark all interned strings as reachable for GC */
			void markAll() {
				std::lock_guard lock(_mutex);
				for (auto& pair : _pool) {
					pair.second->setMarked(true);
				}
			}

		private:
			StringPool() = default;
			~StringPool() = default;

			std::unordered_map<std::string, ObjectRef> _pool;
			std::mutex _mutex;  // protects _pool
	};

}  // namespace sandvik

#endif  // __STRING_POOL_H__
