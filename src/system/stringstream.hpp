#ifndef __SYSTEM_STRING_STREAM_HPP__
#define __SYSTEM_STRING_STREAM_HPP__

#include <stdint.h>

#include <system/stream.hpp>
#include <vector>

namespace sandvik {
	class IStringStream : public Stream {
		public:
			IStringStream(const std::string& data);
			IStringStream(const std::vector<uint8_t>& data);

			virtual ~IStringStream();

			virtual long write(const char* buf_, long count_) override;
			virtual void close() override;
	};

	class OStringStream : public Stream {
		public:
			OStringStream();
			virtual ~OStringStream();

			virtual long read(char* buf_, long count_) override;
			virtual void close() override;

			/** @return buffer */
			std::string str() const;
	};
}  // namespace sandvik

#endif  // __SYSTEM_STRING_STREAM_HPP__