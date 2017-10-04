#include <boost/regex.hpp>

using namespace std;
using namespace boost;

extern "C" int AFLTestOneInput(const uint8_t *data, size_t size) 
{
	if(size < 2)
		return 0;
	vector<wchar_t> v(data, data + size);
	try{
		size_t len = (data[1] << 8) | data[0];
		if(len > size - 2) len = size - 2;
		wstring str(&v[0] + 2, len);
		wstring text(&v[0] + len, size - len);
		wregex expression(str);
		wsmatch what;
		regex_search(text, what, expression,
			match_default | match_partial);
	}
	catch(const exception&){}
	return 0;
}

