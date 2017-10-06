#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <boost/regex.hpp>
#include <assert.h>

#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>

// Notify AFL about deferred forkserver.
static volatile char AFL_DEFER_FORKSVR[] = "##SIG_AFL_DEFER_FORKSRV##";
extern "C" void  __afl_manual_init();
static volatile char suppress_warning1 = AFL_DEFER_FORKSVR[0];

// Notify AFL about persistent mode.
static volatile char AFL_PERSISTENT[] = "##SIG_AFL_PERSISTENT##";
extern "C" int __afl_persistent_loop(unsigned int);
static volatile char suppress_warning2 = AFL_PERSISTENT[0];

static const size_t kAFL_INBUF_MAX = 1024*1024;
static int afl_input_buffer[kAFL_INBUF_MAX];


extern "C" int AFLTestOneInput(const u_int8_t *data, size_t size) 
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
    }
    catch(const std::exception&){}
    return 0;
}

int main(int argc, char **argv)
{
    if (argc > 1) {
        std::streampos begin, end;        
        for (int i = 1; i < argc; i++) {
            std::ifstream file(argv[i]);

            begin = file.tellg();
            file.seekg (0, std::ios_base::end);
            
            end = file.tellg();
            file.seekg (0, file.beg);

            long length = (end-begin);

            std::cout << "Reading " << length << " bytes from " << argv[i] << std::endl;

            // Allocate exactly length bytes so that we reliably catch buffer overflows.
            std::vector<char> bytes(length);
            file.read(bytes.data(), bytes.size());
            assert(file);
            AFLTestOneInput(reinterpret_cast<const uint8_t *>(bytes.data()),
                                   bytes.size());
            std::cout << "Execution successfull" << std::endl;
        }
        return 0;
    }

    __afl_manual_init();
    
	int counter = 0;
    while (__afl_persistent_loop(1000))
    {
        ssize_t inbuf = read(0, afl_input_buffer, kAFL_INBUF_MAX);

        if (afl_input_buffer < (int*)1)
        {
            std::cout << "Received no data from afl-fuzz!" << endl;
            return -1;
        } else {
			// Copy AflInputBuf into a separate buffer to let asan find buffer
			// overflows. Don't use unique_ptr/etc to avoid extra dependencies.
			uint8_t *copy = new uint8_t[inbuf];
			memcpy(copy, afl_input_buffer, inbuf);

			AFLTestOneInput(copy, inbuf);

            delete[] copy;
            counter++;    
		}
	}
	std::cout argv[0] <<  ": successfully executed " << counter << " input(s)" << endl;
}