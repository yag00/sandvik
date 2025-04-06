#ifndef __SYSTEM_FILESTREAM_HPP__
#define __SYSTEM_FILESTREAM_HPP__

#include <system/stream.hpp>

namespace sandvik {
	class IFileStream : public Stream {
		public:
			IFileStream(const std::string& filename);

			virtual ~IFileStream();

			virtual long write(const char* buf_, long count_) override;
			virtual void close() override;
	};

	class OFileStream : public Stream {
		public:
			OFileStream(const std::string& filename);
			virtual ~OFileStream();

			virtual long read(char* buf_, long count_) override;
			virtual void close() override;
	};
}  // namespace sandvik

#endif  // __SYSTEM_FILESTREAM_HPP__