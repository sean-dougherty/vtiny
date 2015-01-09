#include "util.h"

using namespace std;

struct NonPOD0 {
    const char *id;
    NonPOD0(const char *id_) : id(id_) {
        cout << "NonPOD0(" << id << ")" << endl;
    }
    ~NonPOD0() {
        cout << "~NonPOD0(" << id << ")" << endl;
    }
};

namespace __clunion {
    typedef unsigned int vindex_t;
    template<typename T> constexpr
    T const& max(T const& a, T const& b) {
        return a > b ? a : b;
    }

    namespace clunion_A {

        enum class Ids {
            A, B
        };

        void dispatch_vecho(void *thiz);

        struct A {
            int a_i;
            NonPOD0 np0;

            A(int a_i_) : np0("np0") {
                cout << "A(" << a_i_ << ")" << endl;
                a_i = a_i_;
            }
            ~A() {
                cout << "~A()" << endl;
            }
            void vecho() {
                dispatch_vecho(this);
            }
            void __clunion_virtual_vecho() {
                cout << "A::vecho(), a_i=" << a_i << endl;
            }
            void secho() {
                cout << "A::secho()" << endl;
            }
            static void __clunion_call_dtor(void *thiz) {
                ((A *)thiz)->~A();
            }
            static void __clunion_call_vecho(void *thiz) {
                ((A *)thiz)->__clunion_virtual_vecho();
            }
        };

        struct B : public A {
            int b_i;
            NonPOD0 np1;

            B(int a_i_, int b_i_) : A(a_i_), np1("np1") {
                cout << "B(" << a_i_ << ", " << b_i_ << ")" << endl;
                b_i = b_i_;
            }
            ~B() {
                cout << "~B()" << endl;
            }
            void vecho() {
                dispatch_vecho(this);
            }
            void __clunion_virtual_vecho() {
                cout << "B::vecho(), a_i=" << a_i << ", b_i=" << b_i << endl;
            }
            void secho() {
                cout << "B::secho()" << endl;
            }
            static void __clunion_call_dtor(void *thiz) {
                ((B *)thiz)->~B();
            }
            static void __clunion_call_vecho(void *thiz) {
                ((B *)thiz)->__clunion_virtual_vecho();
            }
        };

        constexpr size_t sizeof_A_members = max(sizeof(A), sizeof(B));

        struct Vtable {
            void (*dtor)(void*);
            void (*vecho)(void*);
        };
        Vtable vtables[] = {
            {&A::__clunion_call_dtor, &A::__clunion_call_vecho},
            {&B::__clunion_call_dtor, &B::__clunion_call_vecho}
        };

        vindex_t get_vindex(void *thiz) {
            return *(vindex_t *)((char*)thiz + sizeof_A_members);
        }

        void dispatch_dtor(void *thiz) {
            vtables[get_vindex(thiz)].dtor(thiz);
        }
        void dispatch_vecho(void *thiz) {
            vtables[get_vindex(thiz)].vecho(thiz);
        }
    }
}

/*
clunion A {
    int a_i;
    NonPOD0 np0;

    A(int a_i_) : np0("np0") {
        cout << "A(" << a_i_ << ")" << endl;
        a_i = a_i_;
    }
    ~A() {
        cout << "~A()" << endl;
    }

    virtual void vecho() {
        cout << "A::vecho(), a_i=" << a_i << endl;
    }

    void secho() {
        cout << "A::secho()" << endl;
    }
};
*/
struct A {
protected:
    A() {}
    struct __clunion_members_t {
        char buffer[__clunion::clunion_A::sizeof_A_members];
        __clunion::vindex_t vindex;
    } __clunion_members;
    __clunion::clunion_A::A *thiz() {
        return ((__clunion::clunion_A::A *)__clunion_members.buffer);
    }

public:
    A(int a_i_) {
        __clunion_members.vindex = (__clunion::vindex_t)__clunion::clunion_A::Ids::A;
        new (thiz()) __clunion::clunion_A::A(a_i_);
    }
    ~A() {
        __clunion::clunion_A::dispatch_dtor(thiz());
    }

    __clunion::clunion_A::A *operator->() {
        return thiz();
    }
};

/*
clunion B : public A {
    int b_i;
    NonPOD0 np1;

    B(int a_i_, int b_i_) : A(a_i_), np1("np1") {
        cout << "B(" << a_i_ << ", " << b_i_ << ")" << endl;
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
struct B : public A {
protected:
    __clunion::clunion_A::B *thiz() {
        return (__clunion::clunion_A::B *)A::thiz();
    }
public:
    B(int a_i_, int b_i_) {
        __clunion_members.vindex = (__clunion::vindex_t)__clunion::clunion_A::Ids::B;
        new (thiz()) __clunion::clunion_A::B(a_i_, b_i_);
    }
    __clunion::clunion_A::B *operator->() {
        return thiz();
    }
};

int main(int argc, const char **argv) {
/*
    shared_ptr<A> ap = make_shared<A>(11);
    (*ap)->secho();
    (*ap)->vecho();
*/

    B b{42, 52};
    A &a = b;
    a->secho();
    a->vecho();

    echo(sizeof(A));
    echo(sizeof(B));
}
