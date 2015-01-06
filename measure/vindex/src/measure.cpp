#include "util.h"
#include "measure-work.h"

template<typename checksum_t, typename operator_t>
struct TypeTest {
    const char *type_name;
    uint n_iterations;
    uint n_samples;
    
    template<typename worker_t>
    uint dowork() {
        std::default_random_engine generator(1);
        std::uniform_int_distribution<int> distribution(0, __nops-1);
        uint accum = 0;
        
        for(uint it = 0; it < n_iterations; it++) {
            OperatorType optype = (OperatorType)distribution(generator);
            worker_t *op = worker_t::get(optype);
            accum = op->op(accum);
        }

        return accum;
    }

    struct Sample {
        ullong work_time;
        uint accum;
    };

    Sample get_sample() {
        Sample result;
        result.work_time = time_task([this, &result]() {
                result.accum = dowork<operator_t>();
            });
        return result;
    }

public:
    void run() {
        print("#sizeof(" << type_name << ") = " << sizeof(operator_t));

        uint expected_accum = dowork<checksum_t>();
        print("#expected_accum=" << expected_accum);

        for(uint i = 0; i < n_samples; i++) {
            Sample sample = get_sample();
            if(expected_accum != sample.accum) {
                err("Bad accum for " << type_name << ". Expected " << expected_accum << ", found " << sample.accum);
            }

            print(type_name << "\t" << sample.work_time);
        }
    }
};

int main(int argc, const char **argv) {
    //echo(sizeof(VindexOperator));
    if(argc != 3) {
        err("usage: measure n_iterations n_samples");
    }
    uint argi = 1;
    uint n_iterations = parse_uint(argv[argi++], "n_iterations", 1);
    uint n_samples = parse_uint(argv[argi++], "n_samples", 1);

#define __run(operator_t) {                         \
        TypeTest<VirtualOperator, operator_t> test{ \
            #operator_t,                            \
                n_iterations,                       \
                n_samples};                         \
        test.run();                                 \
    }

    __run(VindexOperator);
    __run(VirtualOperator);
#undef __run
}
