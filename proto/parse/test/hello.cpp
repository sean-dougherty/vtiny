#define clunion struct __attribute__((annotate("clunion")))

clunion A {
    int x, y;
    char z;
public:
    A() {
    }
    ~A() {
    }

    virtual void foo(int a, float *b) {
        //return a + 1.0;
    }
    virtual void foo() = 0;
};

clunion B : A {
    virtual void foo() {
    }
    virtual void another() {
    } 
};

clunion C : public B {
    void hi();
};

clunion D {
    virtual void bar() = 0;
};
