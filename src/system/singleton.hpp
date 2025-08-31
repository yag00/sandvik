#ifndef __SYSTEM_SINGLETON_H__
#define __SYSTEM_SINGLETON_H__

namespace sandvik {
	/** @class Singleton
	 *  @brief This class provides a template singleton object. */
	template <class T>
	class Singleton {
		public:
			/** @return singleton intance */
			static T& getInstance() {
				static T instance;
				return instance;
			}

			/** prevent copies */
			Singleton(const Singleton&) = delete;
			Singleton& operator=(const Singleton&) = delete;

		protected:
			Singleton() {
			}
			virtual ~Singleton() {
			}

		private:
			Singleton(Singleton&);
			void operator=(Singleton&);
	};
}  // namespace sandvik

#endif  // __SYSTEM_SINGLETON_H__
