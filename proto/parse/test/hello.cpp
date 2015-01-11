#define clunion struct __attribute__((annotate("clunion")))

clunion A {
public:
    virtual void foo(int a, float *b) {
        //return a + 1.0;
    }
 };

clunion B : A {
};

clunion C : public B {
};
