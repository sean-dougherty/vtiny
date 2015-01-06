#include "util.h"

template<typename element_t, typename checksum_t>
struct TypeTest {
    const char *type_name;
    uint n_elements;
    uint n_iterations;
    uint n_samples;
    element_t *buffer;
    
    void fill() {
        for(uint it = 0; it < n_iterations; it++) {
            for(uint i = 0; i < n_elements; i++) {
                buffer[i] = i;
            }
        }
    }

    checksum_t checksum() {
        checksum_t result = 0;
        for(uint it = 0; it < n_iterations; it++) {
            for(uint i = 0; i < n_elements; i++) {
                result += buffer[i];
            }
        }
        return result;
    }

    struct Sample {
        ullong fill_time;
        ullong checksum_time;
        checksum_t checksum;
    };

    Sample get_sample() {
        Sample result;
        result.fill_time = time_task([this]() {
                fill();
            });
        result.checksum_time = time_task([this, &result]() {
                result.checksum = checksum();
            });
        return result;
    }

public:
    void run() {
        buffer = new element_t[n_elements + 1];

        checksum_t expected_checksum;
        {
            expected_checksum = get_sample().checksum;
            print("# expected_checksum = " << expected_checksum);
        }

        std::vector<uint> offsets;
        {
            for(uint offset = 0; offset <= sizeof(element_t); offset++) {
                for(uint i = 0; i < n_samples; i++) {
                    offsets.push_back(offset);
                }
            }
            std::random_shuffle(offsets.begin(), offsets.end());
        }

        for(uint offset: offsets) {
            element_t *offset_buffer = (element_t *)((uchar *)buffer + offset);
            Sample sample = get_sample();
            assert(sample.checksum == expected_checksum);
            print(type_name << "\t" << offset << "\t" << sample.fill_time << "\t" << sample.checksum_time);
        }
        delete [] buffer;
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

#define __run(element_t,checksum_t) {           \
        TypeTest<element_t, checksum_t> test{  \
            #element_t,                         \
                n_elements,                     \
                n_iterations,                   \
                n_samples,                      \
                nullptr};                       \
        test.run();                             \
    }
/*
    __run(uchar, uint);
    __run(ushort, uint);
*/
    __run(uint, uint);
/*
    __run(ullong, ullong);
    __run(float, double);
    __run(double, double);
*/
#undef __run
}
