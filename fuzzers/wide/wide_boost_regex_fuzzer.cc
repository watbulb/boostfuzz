#include <boost/regex.hpp>
#include <vector>

extern "C" int AFLTestOneInput(const uint8_t *data, size_t size) 
{
	if(size < 2)
		return 0;
	std::vector<wchar_t> v(data, data + size);
	try{
		size_t len = (data[1] << 8) | data[0];
		if(len > size - 2) len = size - 2;
		std::wstring str(&v[0] + 2, len);
		std::wstring text(&v[0] + len, size - len);
		boost::wregex expression(str);
		boost::wsmatch what;
		boost::regex_search(text, what, expression,
			boost::match_default | boost::match_partial);

	} catch(const std::exception&) {
		return -1;
	}
	return 0;
}

