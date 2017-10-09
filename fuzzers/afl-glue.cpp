/*
   To make this work, the library and this shim need to be compiled in LLVM
   mode using afl-clang-fast (other compiler wrappers will *not* work).
 */

#include <stdint.h>
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

// AFL test case harness external
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

static time_t unit_time_secs;
static time_t slowest_unit_time_secs = 0;

static int good_runs = 0;
static int bad_runs = 0;
static int num_runs = 0;
static int result = 0;

int main(int argc, char **argv)
{
	if (argc > 1) {
		std::streampos begin, end;

		long length;

		for (int i = 1; i < argc; i++) {
			std::ifstream file(argv[i]);

			begin = file.tellg();
			file.seekg (0, std::ios_base::end);

			end = file.tellg();
			file.seekg (0, file.beg);

			length = (end-begin);
			if (length == 0) {
				fprintf(stderr, "Skipping read of invalid file: %s\n", argv[i]);
				continue;
			} else if (length > kAFL_INBUF_MAX) {
				fprintf(stderr, "File %s is far to large to be processed, skipping\n", argv[i]);
				continue;
			}

			printf("Reading %ld bytes from %s\n", length, argv[i]);

			// Allocate exactly length bytes so that we reliably catch buffer overflows.
			std::vector<char> bytes(length);
			file.read(bytes.data(), bytes.size());

			result = AFLTestOneInput(reinterpret_cast<const uint8_t *>(bytes.data()),
			bytes.size());

			if (result != 0) {
				fprintf(stderr, "Execution of file: %s failed!\n", argv[i]);
				continue;
			}
			printf("Execution sucessfull\n");
		}
		return 0;
	}

	__afl_manual_init();

	printf("[afl-glue] afl deferred forkserver started\n");
	printf("[afl-glue] you may input your testcase directly into this buffer\n");
	printf("[afl-glue] or you may even supply a list of files to run:\n");
	printf("\t>>> %s ./testcase1 ../testcase2\n>>>", argv[0]);

	while (__afl_persistent_loop(1000))
	{
		ssize_t n_read = read(0, afl_input_buffer, kAFL_INBUF_MAX);
		if (n_read > 0 ) {

			//Copy AflInputBuf into a separate buffer to let asan find buffer
			//overflows. Don't use unique_ptr/etc to avoid extra dependencies.
			uint8_t *copy = new uint8_t[n_read];
			memcpy(copy, afl_input_buffer, n_read);

			struct timeval unit_start_time;
			gettimeofday(&unit_start_time, NULL);

			result = AFLTestOneInput(copy, n_read);

			struct timeval unit_stop_time;
			gettimeofday(&unit_stop_time, NULL);

			if (result == 0) {
				good_runs++;
			} else {
				bad_runs++;
			}

			num_runs++;

			// Update slowest_unit_time_secs if we see a new max.
			unit_time_secs = unit_stop_time.tv_sec - unit_start_time.tv_sec;
			if (slowest_unit_time_secs < unit_time_secs)
				slowest_unit_time_secs = unit_time_secs;

			delete[] copy;
		}
	}
	if (num_runs > 1) {
		printf("Executed %d inputs\n\tSucessfull: %d  Failed %d\n", num_runs, good_runs, bad_runs);
	} else {
		printf("Execution of buffer ");
		if (good_runs > 0) {
			printf("succeeded\n");
		} else {
			printf("failed\n");
		}
	}
	return 0;
}