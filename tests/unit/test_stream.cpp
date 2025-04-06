#include <string.h>
#include <gtest/gtest.h>

#include <system/stream.hpp>
#include <system/filestream.hpp>
#include <system/stringstream.hpp>

using namespace sandvik;

TEST(stream, file) {
	std::unique_ptr<OFileStream> bin1_w = std::make_unique<OFileStream>("unit_bin1");

	for (int i=0; i<20; i++) {
		EXPECT_EQ((uint32_t)sizeof(i), (uint32_t)bin1_w->write((char*)&i, sizeof(i)));
	}
	bin1_w->flush();
	bin1_w->close();
	std::unique_ptr<IFileStream> bin1_r = std::make_unique<IFileStream>("unit_bin1");

	int read;
	for (int i=0; i<20; i++) {
		EXPECT_EQ((uint32_t)sizeof(read), (uint32_t)bin1_r->read((char*)&read, sizeof(read)));
		EXPECT_EQ((uint32_t)i, (uint32_t)read);
	}
	bin1_r->close();

	std::unique_ptr<IFileStream> bin1_rs = std::make_unique<IFileStream>("unit_bin1");
	bin1_rs->seek(0);
	EXPECT_EQ((uint32_t)sizeof(read), (uint32_t)bin1_rs->read((char*)&read, sizeof(read)));
	EXPECT_EQ((uint32_t)0, (uint32_t)read);
	bin1_rs->seek(4*2);
	EXPECT_EQ((uint32_t)sizeof(read), (uint32_t)bin1_rs->read((char*)&read, sizeof(read)));
	EXPECT_EQ((uint32_t)2, (uint32_t)read);
	bin1_rs->seek(4*17);
	EXPECT_EQ((uint32_t)sizeof(read), (uint32_t)bin1_rs->read((char*)&read, sizeof(read)));
	EXPECT_EQ((uint32_t)17, (uint32_t)read);
}

TEST(stream, string) {
	std::unique_ptr<OStringStream> bin1_w = std::make_unique<OStringStream>();
	for (int i=0; i<20; i++) {
		EXPECT_EQ((uint32_t)sizeof(i), (uint32_t)bin1_w->write((char*)&i, sizeof(i)));
	}
	bin1_w->flush();

	std::string obss = bin1_w->str();
	EXPECT_EQ((uint32_t)20*sizeof(int), (uint32_t)obss.size());

	int* ptr = (int*)obss.data();
	for (int i=0; i<20; i++) {
		EXPECT_EQ((int)i, ptr[i]);
	}

	std::unique_ptr<IStringStream> bin1_r = std::make_unique<IStringStream>(obss);
	int read;
	for (int i=0; i<20; i++) {
		EXPECT_EQ((uint32_t)sizeof(read), (uint32_t)bin1_r->read((char*)&read, sizeof(read)));
		EXPECT_EQ((uint32_t)i, (uint32_t)read);
	}
	std::unique_ptr<IStringStream> bin1_rs = std::make_unique<IStringStream>(obss);
	bin1_rs->seek(0);
	EXPECT_EQ((uint32_t)sizeof(read), (uint32_t)bin1_rs->read((char*)&read, sizeof(read)));
	EXPECT_EQ((uint32_t)0, (uint32_t)read);
	bin1_rs->seek(4*2);
	EXPECT_EQ((uint32_t)sizeof(read), (uint32_t)bin1_rs->read((char*)&read, sizeof(read)));
	EXPECT_EQ((uint32_t)2, (uint32_t)read);
	bin1_rs->seek(4*17);
	EXPECT_EQ((uint32_t)sizeof(read), (uint32_t)bin1_rs->read((char*)&read, sizeof(read)));
	EXPECT_EQ((uint32_t)17, (uint32_t)read);
}
