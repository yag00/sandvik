#include <string.h>
#include <gtest/gtest.h>

#include <system/zip.hpp>

using namespace sandvik;


TEST(compression, zip) {
	std::string data = "Hello World";
	std::string data2 = "Good Bye!";
	ZipWriter zipw;
	zipw.open("ziptest.zip");
	zipw.addFromMemory("test.txt", data.c_str(), data.size());
	zipw.addFromMemory("test2.txt", data2.c_str(), data2.size());
	zipw.close();

	ZipReader zipr;
	zipr.open("ziptest.zip");

	uint64_t size = 0;
	char* ptr = zipr.extractToMemory("test.txt", size);
	std::string rdata(ptr, size);
	free(ptr);

	ptr = zipr.extractToMemory("test2.txt", size);
	std::string rdata2(ptr, size);
	free(ptr);

	zipr.close();

	EXPECT_EQ(data, rdata);
	EXPECT_EQ(data2, rdata2);
}
