#include <boost/regex.hpp>
extern "C" int AFLTestOneInput(const uint8_t *data, size_t size)
{
	if(size < 2)
		return 0;
	try{
		size_t len = (data[1] << 8) | data[0];
		if(len > size - 2) len = size - 2;
		std::string str((char*)(data + 2), len);
		std::string text((char*)(data + len), size - len);
		boost::regex expression(str);
		boost::smatch what;
		boost::regex_search(text, what, expression,
			boost::match_default | boost::match_partial);
	} catch(const std::exception&) {
		return -1;
	}
	return 0;
}

