#include "util.h"
#include "measure-work.h"

template<typename call_t>
struct TypeTest {
    const char *type_name;
    uint n_iterations;
    uint n_samples;
    call_t *call;
    
    void dowork() {
        for(uint it = 0; it < n_iterations; it++) {
            call->work();
        }
    }

    struct Sample {
        ullong work_time;
    };

    Sample get_sample() {
        Sample result;
        result.work_time = time_task([this]() {
                dowork();
            });
        return result;
    }

public:
    void run() {
        call = new call_t;
        print("#sizeof(" << type_name << ") = " << sizeof(call_t));

        for(uint i = 0; i < n_samples; i++) {
            Sample sample = get_sample();
            print(type_name << "\t" << sample.work_time);
        }

        delete call;
    }
};

int main(int argc, const char **argv) {
    if(argc != 3) {
        err("usage: measure n_iterations n_samples");
    }
    uint argi = 1;
    uint n_iterations = parse_uint(argv[argi++], "n_iterations", 1);
    uint n_samples = parse_uint(argv[argi++], "n_samples", 1);

#define __run(call_t) {           \
        TypeTest<call_t> test{  \
            #call_t,                         \
                n_iterations,                   \
                n_samples,                      \
                nullptr};                       \
        test.run();                             \
    }
    __run(StaticCall);
    __run(VirtualCall0);
#undef __run
}
