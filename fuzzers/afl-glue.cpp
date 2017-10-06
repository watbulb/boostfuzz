#include <assert.h>
#include <signal.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>


#include <fstream>
#include <iostream>
#include <vector>

#ifdef __linux__
#define LIBFUZZER_LINUX 1
#define LIBFUZZER_APPLE 0
#elif __APPLE__
#define LIBFUZZER_LINUX 0
#define LIBFUZZER_APPLE 1
#else
#error "Support for your platform has not been implemented"
#endif

// libFuzzer interface is thin, so we don't include any libFuzzer headers.
extern "C" int AFLTestOneInput(const uint8_t *Data, size_t Size);

// Notify AFL about deferred forkserver.
static volatile char AFL_DEFER_FORKSVR[] = "##SIG_AFL_DEFER_FORKSRV##";
extern "C" void  __afl_manual_init();
static volatile char suppress_warning1 = AFL_DEFER_FORKSVR[0];

// Notify AFL about persistent mode.
static volatile char AFL_PERSISTENT[] = "##SIG_AFL_PERSISTENT##";
extern "C" int __afl_persistent_loop(unsigned int);
static volatile char suppress_warning2 = AFL_PERSISTENT[0];

static const size_t kAFL_INBUF_MAX = 1 << 20;
static int afl_input_buffer[kAFL_INBUF_MAX];

static time_t slowest_unit_time_secs = 0;


// Used to avoid repeating error checking boilerplate. If cond is false, a
// fatal error has occured in the program. In this event print error_message
// to stderr and abort(). Otherwise do nothing. Note that setting
// AFL_DRIVER_STDERR_DUPLICATE_FILENAME may cause error_message to be appended
// to the file as well, if the error occurs after the duplication is performed.
#define CHECK_ERROR(cond, error_message)                                      \
	if (!(cond)) {                                                            \
  		fprintf(stderr, (error_message));                                     \
  		abort();                                                              \
	}


int main(int argc, char **argv)
{
	if (argc > 1) {
		std::streampos begin, end;
		
		uint8_t result;
		long length;
		
		for (int i = 1; i < argc; i++) {
			std::ifstream file(argv[i]);
			
			begin = file.tellg();
			file.seekg (0, std::ios_base::end);
			
			end = file.tellg();
			file.seekg (0, file.beg);
			
			length = (end-begin);
			if (length == 0) {
				std::cerr << "Skipping read of invalid file: " << argv[i] << std::endl;
				continue;
			}
			
			std::cout << "Reading " << length << " bytes from " << argv[i] << std::endl;
			
			// Allocate exactly length bytes so that we reliably catch buffer overflows.
			std::vector<char> bytes(length);
			file.read(bytes.data(), bytes.size());
			
			assert(file);
			
			result = AFLTestOneInput(reinterpret_cast<const uint8_t *>(bytes.data()),
			bytes.size());
			
			if (result != 0) {
				std::cerr << "Execution of file: " << argv[i] << " failed!" << std::endl;
				continue;
			}
			
			std::cout << "Execution successfull" << std::endl;
			
		}
		return 0;
	}
	
	
	__afl_manual_init();
	
	int N = 1000;
	if (argc == 2 && argv[1][0] == '-')
		N = atoi(argv[1] + 1);
	else if(argc == 2 && (N = atoi(argv[1])) > 0)
		fprintf(stderr, "WARNING: using the deprecated call style `%s %d`\n",
			argv[0], N);
		
	
	assert(N > 0);
	time_t unit_time_secs;
	int num_runs = 0;
	while (__afl_persistent_loop(N)) {
		ssize_t n_read = read(0, afl_input_buffer, kAFL_INBUF_MAX);
		if (n_read > 0) {
			// Copy AflInputBuf into a separate buffer to let asan find buffer
			// overflows. Don't use unique_ptr/etc to avoid extra dependencies.
			uint8_t *copy = new uint8_t[n_read];
			memcpy(copy, afl_input_buffer, n_read);
			
			struct timeval unit_start_time;
			CHECK_ERROR(gettimeofday(&unit_start_time, NULL) == 0,
				"Calling gettimeofday failed");
			
			num_runs++;
			AFLTestOneInput(copy, n_read);
			
			struct timeval unit_stop_time;
			CHECK_ERROR(gettimeofday(&unit_stop_time, NULL) == 0,
				"Calling gettimeofday failed");
			
			// Update slowest_unit_time_secs if we see a new max.
			unit_time_secs = unit_stop_time.tv_sec - unit_start_time.tv_sec;
			if (slowest_unit_time_secs < unit_time_secs)
				slowest_unit_time_secs = unit_time_secs;
			
			delete[] copy;
		}
	}
	fprintf(stderr, "%s: successfully executed %d input(s)\n", argv[0], num_runs);
}


//     int counter = 0;
//     time_t unit_time_secs;
//     int num_runs = 0;
//     while (__afl_persistent_loop(1000))
//     {
	//         ssize_t n_read = read(0, afl_input_buffer, kAFL_INBUF_MAX);
	//         if (n_read > 0) {
		//             /* if (*afl_input_buffer < 1)
		//             {
			//                 std::cerr << "Received no data from afl-fuzz!" << std::endl;
			//                 return -1;
			//             } else { 
				//             Copy AflInputBuf into a separate buffer to let asan find buffer
				//             overflows. Don't use unique_ptr/etc to avoid extra dependencies.
				//             */
				
				//             uint8_t *copy = new uint8_t[n_read];
				//             memcpy(copy, afl_input_buffer, n_read);
				
				//             struct timeval unit_start_time;
				//             gettimeofday(&unit_start_time, NULL);
				
				//             counter++;
				//             AFLTestOneInput(copy, n_read);
				
				//             struct timeval unit_stop_time;
				//             gettimeofday(&unit_stop_time, NULL);
				
				//             // Update slowest_unit_time_secs if we see a new max.
				//             unit_time_secs = unit_stop_time.tv_sec - unit_start_time.tv_sec;
				//             if (slowest_unit_time_secs < unit_time_secs)
				//                 slowest_unit_time_secs = unit_time_secs;
				
				
				//             delete[] copy;
				//         }
				// 	}
				// 	std::cout << argv[0] <<  ": successfully executed " << counter << " input(s)" << std::endl;
				// }