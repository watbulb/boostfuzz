#include <boost/regex.hpp>

using namespace std;
using namespace boost;

extern "C" int AFLTestOneInput(const uint8_t *data, size_t size) 
{
  if(size < 2)
    return 0;
  try{
    size_t len = (data[1] << 8) | data[0];
    if(len > size - 2) len = size - 2;
    string str((char*)(data + 2), len);
    string text((char*)(data + len), size - len);
    regex expression(str);
    smatch what;
    regex_search(text, what, expression,
        match_default | match_partial);
  }
  catch(const exception&){}
  return 0;
}

