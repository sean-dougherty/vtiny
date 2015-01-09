#include "util.h"

using namespace std;

struct NonPOD0 {
    NonPOD0() {
        cout << "NonPOD0()" << endl;
    }
    ~NonPOD0() {
        cout << "~NonPOD0()" << endl;
    }
};

namespace __clunion {
    namespace clunion_A {
        enum class Ids {
            A, B, C, D
        };

        void dispatch_dtor(void *a_);
    }
}

/*
clunion A {
    int a_i;

    A(int a_i_) {
        a_i = a_i_;
    }

    virtual void vecho() {
        cout << "A, a_i=" << a_i << endl;
    }

    void secho() {
        cout << "A::secho()" << endl;
    }
};
*/
struct A {
    int a_i;
    NonPOD0 np;
    struct __clunion_members_t {
        int pad0;
        uchar pad[2];
        uchar id;

        static void ctor(A *thiz, int a_i_) {
            cout << "A::ctor()" << endl;
            thiz->a_i = a_i_;
            
        }
        static void dtor(A *thiz) {
            cout << "A::dtor()" << endl;
        }
    } __clunion_members;

    A(int a_i_) {
        cout << "A()" << endl;
        __clunion_members.id = (uchar)__clunion::clunion_A::Ids::A;
        __clunion_members_t::ctor(this, a_i_);
    }
    ~A() {
        cout << "~A()" << endl;
        __clunion::clunion_A::dispatch_dtor(this);
    }

    static void vecho(A *thiz) {
        cout << "A, a_i=" << thiz->a_i << endl;
    }

    void secho() {
        cout << "A::secho()" << endl;
    }
};

/*
clunion B : public A {
    int b_i;

    B(int a_i_, int b_i_) : A(a_i) {
        b_i = b_i_;
    }

    virtual void vecho() {
        cout << "B, a_i=" << a_i << ", b_i=" << b_i << endl;
    }

    void secho() {
        cout << "B::secho()" << endl;
    }
};
*/
struct B {
    int a_i;
    NonPOD0 np;
    int b_i;
    struct __clunion_members_t {
        uchar pad[2];
        uchar id;

        static void ctor(B *thiz, int a_i_, int b_i_) {
            cout << "B::ctor()" << endl;
            thiz->b_i = b_i_;
        }

        static void dtor(A *thiz) {
            cout << "B::dtor()" << endl;
            A::__clunion_members_t::dtor(thiz);
        }
    } __clunion_members;

    B(int a_i_, int b_i_) {
        cout << "B()" << endl;
        __clunion_members.id = (uchar)__clunion::clunion_A::Ids::B;
        A::__clunion_members_t::ctor((A*)this, a_i_);
        __clunion_members_t::ctor(this, a_i_, b_i_);
    }

    ~B() {
        cout << "~B()" << endl;
        __clunion::clunion_A::dispatch_dtor(this);
    }

    static void vecho(B *thiz) {
        cout << "B, a_i=" << thiz->a_i << ", b_i=" << thiz->b_i << endl;
    }

    void secho() {
        cout << "B::secho()" << endl;
    }
};

/*
clunion C : public A {
    int c_i;

    virtual void vecho() {
        cout << "C, a_i=" << a_i << ", c_i=" << c_i << endl;
    }
};
*/
struct C {
    int a_i;
    NonPOD0 np;
    int c_i;
    struct __clunion_members_t {
        uchar pad[2];
        uchar id;
    } __clunion_members;
};

/*
clunion D : public C {
    short d_i;

    virtual void vecho() {
        cout << "C, a_i=" << a_i << ", c_i=" << c_i << endl;
    }
};
*/
struct D {
    int a_i;
    NonPOD0 np;
    int c_i;
    short d_i;
    struct __clunion_members_t {
        uchar id;
    } __clunion_members;
};

namespace __clunion {
    namespace clunion_A {
        struct Vtable {
            void (*dtor)(void*);
            void (*vecho)(void*);
        };
        Vtable vtables[] = {
            {(void (*)(void*))&A::__clunion_members_t::dtor, (void (*)(void*))&A::vecho},
            {(void (*)(void*))&B::__clunion_members_t::dtor, (void (*)(void*))&B::vecho},
        };

        inline void dispatch_vecho(void *a_) {
            vtables[((A*)a_)->__clunion_members.id].vecho(a_);
        }

        inline void dispatch_dtor(void *a_) {
            vtables[((A*)a_)->__clunion_members.id].dtor(a_);
        }
    };
};

/*
int main(int argc, const char **argv) {
    B b(42, 2);
    A &a = b;

    a.vecho();
    a.secho();
    b.vecho();
    b.secho();

    return 0;
}
*/
int main(int argc, const char **argv) {
    B *b = new B(42, 2);
    A *a = reinterpret_cast<A *>(b);

    __clunion::clunion_A::dispatch_vecho(a);
    a->secho();
    __clunion::clunion_A::dispatch_vecho(b);
    b->secho();

    delete a;

    return 0;
}
