
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <boost/regex.hpp>

#include <iostream>

using namespace std;
using namespace boost;

// Notify AFL about deferred forkserver.
static volatile char AFL_DEFER_FORKSVR[] = "##SIG_AFL_DEFER_FORKSRV##";
extern "C" void  __afl_manual_init();
static volatile char suppress_warning1 = AFL_DEFER_FORKSVR[0];

// Notify AFL about persistent mode.
static volatile char AFL_PERSISTENT[] = "##SIG_AFL_PERSISTENT##";
extern "C" int __afl_persistent_loop(unsigned int);
static volatile char suppress_warning2 = AFL_PERSISTENT[0];

// Input buffer.
static const size_t kMaxAflInputSize = 1 << 20;
static uint8_t AflInputBuf[kMaxAflInputSize];


static const size_t AFL_INBUF_MAX = 1024*1024
static uint8_t afl_input_buffer[AFL_INBUF_MAX];



extern "C" int AFLTestOneInput(int argc, char **argv) 
{
    if(size < 2)
        return 0;
    try{
        string text((char*)(data), size);
        regex expression(str);
        match_result<const_iterator> what;
        regex_match(text, what, expression,
            match_default | match_partial);

        //regex_search(text, what, e, match_default | match_partial);
    }
    catch(const exception&){}
    return 0;
}

// Execute any files provided as parameters.
int ExecuteFilesOnyByOne(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
      std::ifstream in(argv[i]);
      in.seekg(0, in.end);
      size_t length = in.tellg();
      in.seekg (0, in.beg);
      std::cout << "Reading " << length << " bytes from " << argv[i] << std::endl;
      // Allocate exactly length bytes so that we reliably catch buffer overflows.
      std::vector<char> bytes(length);
      in.read(bytes.data(), bytes.size());
      assert(in);
      LLVMFuzzerTestOneInput(reinterpret_cast<const uint8_t *>(bytes.data()),
                             bytes.size());
      std::cout << "Execution successfull" << std::endl;
    }
    return 0;
  }

int main(int argc, char **argv)
{
    if(size < 2)
        return 0;

    __afl_manual_init();

    if (argc < 1)
        return cerr << "No testcase specified!" << endl
    else if (argc == 1)
        return ExecuteTestCase(argc, argv)
    else if (argc > 1)
        for (size_t i = 1; i < argc; i++)
        {
            ExecuteTestCase(argc[i], argv)
        }

	int counter = 0;
    while (__afl_persistent_loop(1000))
    {
        ssize_t afl_input_buffer = read(0, afl_input_buffer, AFL_INBUF_MAX);

		if (afl_input_buffer < 1)
            return cerr << "Received no data from afl-fuzz!" << endl
        else
        {
			// Copy AflInputBuf into a separate buffer to let asan find buffer
			// overflows. Don't use unique_ptr/etc to avoid extra dependencies.
			uint8_t *copy = new uint8_t[n_read];
			memcpy(copy, AflInputBuf, n_read);

			struct timeval unit_start_time;
			CHECK_ERROR(gettimeofday(&unit_start_time, NULL) == 0,
						"Calling gettimeofday failed");

			num_runs++;
			LLVMFuzzerTestOneInput(copy, n_read);

			struct timeval unit_stop_time;
			CHECK_ERROR(gettimeofday(&unit_stop_time, NULL) == 0,
						"Calling gettimeofday failed");

			// Update slowest_unit_time_secs if we see a new max.
			unit_time_secs = unit_stop_time.tv_sec - unit_start_time.tv_sec;
			if (slowest_unit_time_secs < unit_time_secs)
				slowest_unit_time_secs = unit_time_secs;

			delete[] copy;

			counter = counter + 1;
		}
	}
	fprintf(stderr, "%s: successfully executed %d input(s)\n", argv[0], num_runs);
}