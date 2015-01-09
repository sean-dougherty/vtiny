#include "util.h"

using namespace std;

struct NonPOD0 {
    string id;
    NonPOD0(const char *id_) : id(id_) {
        cout << "NonPOD0(" << id << ")" << endl;
    }
    ~NonPOD0() {
        cout << "~NonPOD0(" << id << ")" << endl;
    }
};

namespace __clunion {
    typedef unsigned char vindex_t;

    template<typename T> constexpr
    T const& max(T const& a, T const& b) {
        return a > b ? a : b;
    }

    namespace clunion_A {

        enum class Ids {
            A, B
        };

        void dispatch_vecho(struct A *thiz);

        struct A {
            int a_i;
            NonPOD0 np0;
            vindex_t __clunion_vindex;

            void __clunion_set_vindex(Ids id) {
                __clunion_vindex = (vindex_t)id;
            }

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
            static void __clunion_call_copy(A *thiz, const A &other) {
                new ((A *)thiz)A((const A&)other);
            }
            static void __clunion_call_assign(A *thiz, const A &other) {
                *((A *)thiz) = (const A&)other;
            }
            static void __clunion_call_dtor(A *thiz) {
                thiz->~A();
            }
            static void __clunion_call_vecho(A *thiz) {
                thiz->__clunion_virtual_vecho();
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
            static void __clunion_call_copy(A *thiz, const A &other) {
                new ((B *)thiz)B((const B&)other);
            }
            static void __clunion_call_assign(A *thiz, const A &other) {
                *((B *)thiz) = (const B&)other;
            }
            static void __clunion_call_dtor(A *thiz) {
                ((B *)thiz)->~B();
            }
            static void __clunion_call_vecho(A *thiz) {
                ((B *)thiz)->__clunion_virtual_vecho();
            }
        };

        constexpr size_t sizeof_members = max(sizeof(A), sizeof(B));

        struct Vtable {
            void (*copy)(A*,const A&);
            void (*assign)(A*,const A&);
            void (*dtor)(A*);
            void (*vecho)(A*);
        };
        Vtable vtables[] = {
            {&A::__clunion_call_copy, &A::__clunion_call_assign, &A::__clunion_call_dtor, &A::__clunion_call_vecho},
            {&B::__clunion_call_copy, &B::__clunion_call_assign, &B::__clunion_call_dtor, &B::__clunion_call_vecho}
        };

        vindex_t get_vindex(const A *thiz) {
            return thiz->__clunion_vindex;
        }
        void set_vindex(A *thiz, vindex_t vindex) {
            thiz->__clunion_vindex = vindex;
        }

        void dispatch_copy(A *thiz, const A &other) {
            vtables[get_vindex(thiz)].copy(thiz, other);
        }
        void dispatch_dtor(A *thiz) {
            vtables[get_vindex(thiz)].dtor(thiz);
        }
        void dispatch_vecho(A *thiz) {
            vtables[get_vindex(thiz)].vecho(thiz);
        }
        void dispatch_assign(A *thiz, const A &other) {
            if(get_vindex(thiz) != get_vindex(&other)) {
                dispatch_dtor(thiz);
                set_vindex(thiz, get_vindex(&other));
                dispatch_copy(thiz, other);
            } else {
                vtables[get_vindex(thiz)].assign(thiz, other);
            }
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
    char __clunion_members[__clunion::clunion_A::sizeof_members];
    __clunion::clunion_A::A *__clunion_this() const {
        return ((__clunion::clunion_A::A *)__clunion_members);
    }

public:
    A(const A &other) {
        __clunion_this()->__clunion_set_vindex(__clunion::clunion_A::Ids::A);
        __clunion::clunion_A::dispatch_copy(__clunion_this(), *other.__clunion_this());
    }
    A(int a_i_) {
        __clunion_this()->__clunion_set_vindex(__clunion::clunion_A::Ids::A);
        new (__clunion_this()) __clunion::clunion_A::A(a_i_);
    }
    ~A() {
        __clunion::clunion_A::dispatch_dtor(__clunion_this());
    }
    __clunion::clunion_A::A *operator->() const {
        return __clunion_this();
    }
    A &operator=(const A &other) {
        if(this != &other) {
            __clunion::clunion_A::dispatch_assign(__clunion_this(), *other.__clunion_this());
        }
        return *this;
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
    __clunion::clunion_A::B *__clunion_this() const {
        return (__clunion::clunion_A::B *)A::__clunion_this();
    }
public:
    B(int a_i_, int b_i_) {
        __clunion_this()->__clunion_set_vindex(__clunion::clunion_A::Ids::B);
        new (__clunion_this()) __clunion::clunion_A::B(a_i_, b_i_);
    }
    __clunion::clunion_A::B *operator->() const {
        return __clunion_this();
    }
    B &operator=(const B &other) {
        if(this != &other) {
            __clunion::clunion_A::dispatch_assign(__clunion_this(), *other.__clunion_this());
        }
        return *this;
    }
};

int main(int argc, const char **argv) {
/*
    B b0{42, 52};
    A &ab = b0;
    ab->secho();
    ab->vecho();
    B b1{11, 12};
    b1->secho();
    b1->vecho();
    b1 = b0;
    b1->secho();
    b1->vecho();
*/
    A aa{100};
    A a1{aa};
    a1->vecho();
}
