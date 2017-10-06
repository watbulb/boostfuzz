#include <assert.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include <boost/regex.hpp>


#include <fstream>
#include <iostream>
#include <vector>

extern "C" int AFLTestOneInput(const u_int8_t *data, size_t size) 
{
    if(size < 2)
        return 0;
    try{
        std::string text((char*)(data), size);
        std::cout << text << std::endl;
        std::cout << size << std::endl;
        boost::regex expression(text);
        boost::match_results<std::string::const_iterator> what;
        boost::regex_match(text, what, expression,
            boost::match_default | boost::match_partial);
            
            //regex_search(text, what, e, match_default | match_partial);
    } catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0; 
}
    
    