#include <boost/regex.hpp>
extern "C" int AFLTestOneInput(const uint8_t *data, size_t size)
{
    if(size < 2)
        return 0;
    try{
        std::string text((char*)(data), size);
        boost::regex expression(text);
        boost::match_results<std::string::const_iterator> what;
        boost::regex_match(text, what, expression,
            boost::match_default | boost::match_partial);

            //regex_search(text, what, e, match_default | match_partial);
    } catch(const std::exception&) {
        return -1;
    }
    return 0;
}