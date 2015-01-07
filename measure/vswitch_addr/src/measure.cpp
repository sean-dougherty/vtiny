#include "util.h"
#include "measure-work.h"

//#define CHECKSUM true

template<typename checksum_t, typename operator_t>
struct TypeTest {
    const char *type_name;
    uint n_elements;
    uint n_iterations;
    uint n_samples;
    operator_t **ops;
    
    template<typename worker_t>
    worker_t **create_ops() {
        std::default_random_engine generator(1);
        std::uniform_int_distribution<int> distribution(0, __nops-1);
        worker_t **ops = new worker_t*[n_elements];
        for(uint i = 0; i < n_elements; i++) {
            OperatorType optype = (OperatorType)distribution(generator);
            ops[i] = worker_t::create(optype);
        }
        return ops;
    }

    template<typename worker_t>
    void dispose_ops(worker_t **ops) {
        for(uint i = 0; i < n_elements; i++) {
            worker_t::dispose(ops[i]);
        }
        delete [] ops;
    }

    template<typename worker_t>
    uint dowork(worker_t **ops) {
        uint accum = 0;
        
        for(uint it = 0; it < n_iterations; it++) {
            for(uint i = 0; i < n_elements; i++) {
                worker_t *op = ops[i];
                accum = op->op(accum);
            }
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
                result.accum = dowork<operator_t>(ops);
            });
        return result;
    }

public:
    void run() {
        print("#sizeof(" << type_name << ") = " << sizeof(operator_t));

#if CHECKSUM
        uint expected_accum;
        {
            checksum_t **checksum_ops = create_ops<checksum_t>();
            expected_accum = dowork<checksum_t>(checksum_ops);
            dispose_ops(checksum_ops);
        }
        print("#expected_accum=" << expected_accum);
#endif

        ops = create_ops<operator_t>();
        for(uint i = 0; i < n_samples; i++) {
            Sample sample = get_sample();
#if CHECKSUM
            if(expected_accum != sample.accum) {
                err("Bad accum for " << type_name << ". Expected " << expected_accum << ", found " << sample.accum);
            }
#endif

            print(type_name << "\t" << sample.work_time);
        }
        dispose_ops(ops);
    }
};

int main(int argc, const char **argv) {
    if(argc != 4) {
        err("usage: measure n_elements n_iterations n_samples");
    }
    uint argi = 1;
    uint n_elements = parse_uint(argv[argi++], "n_elements", 1);
    uint n_iterations = parse_uint(argv[argi++], "n_iterations", 1);
    uint n_samples = parse_uint(argv[argi++], "n_samples", 1);

#define __run(operator_t) {                         \
        TypeTest<VirtualOperator, operator_t> test{ \
            #operator_t,                            \
                n_elements,                         \
                n_iterations,                       \
                n_samples};                         \
        test.run();                                 \
    }

    __run(VswitchOperator);
    //__run(VirtualOperator);
#undef __run
}
