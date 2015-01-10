#include <typeinfo>
#include "util.h"

using namespace std;

#define DYNAMIC_TYPE_CHECKING

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
    namespace clunion_A {
        namespace target {
            struct A;
        }
        namespace dispatch {
            void vecho(struct target::A *thiz);
            void aecho(struct target::A *thiz);
        }

        typedef unsigned char vindex_t;

        enum class Ids {
            A, B, C
        };

        namespace target {
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
                void aecho() {
                    dispatch::aecho(this);
                }
                void __clunion_virtual_aecho() {
                    cout << "aecho()" << endl;
                }
                void vecho() {
                    dispatch::vecho(this);
                }
                void __clunion_virtual_vecho() {
                    cout << "A::vecho(), a_i=" << a_i << endl;
                }
                void secho() {
                    cout << "A::secho()" << endl;
                }

#ifdef DYNAMIC_TYPE_CHECKING
                vindex_t __clunion_vindex_decl;
#endif
                vindex_t __clunion_vindex_actual;
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
                void __clunion_virtual_vecho() {
                    cout << "B::vecho(), a_i=" << a_i << ", b_i=" << b_i << endl;
                }
                void secho() {
                    cout << "B::secho()" << endl;
                }
            };

            struct C : public A {
                int c_i0;
                int c_i1;
                NonPOD0 np1;

                C(int a_i_, int c_i0_, int c_i1_) : A(a_i_), np1("np1") {
                    cout << "C(" << a_i_ << ", " << c_i0_ << ", " << c_i1_ << ")" << endl;
                    c_i0 = c_i0_;
                    c_i1 = c_i1_;
                }
                ~C() {
                    cout << "~C()" << endl;
                }
                void __clunion_virtual_vecho() {
                    cout << "C::vecho()(" << a_i << ", " << c_i0 << ", " << c_i1 << ")" << endl;
                }
                void secho() {
                    cout << "C::secho()" << endl;
                }
            };

            Ids get_actual(const target::A *thiz) {
                return (Ids)thiz->__clunion_vindex_actual;
            }
            void set_actual(target::A *thiz, Ids id) {
                thiz->__clunion_vindex_actual = (vindex_t)id;
            }

#ifdef DYNAMIC_TYPE_CHECKING
            Ids get_decl(const target::A *thiz) {
                return (Ids)thiz->__clunion_vindex_decl;
            }
            void set_decl(target::A *thiz, Ids id) {
                thiz->__clunion_vindex_decl = (vindex_t)id;
            }
            void check_cast(target::A *thiz, Ids to) {
                switch(get_decl(thiz)) {
                case Ids::A:
                    break;
                case Ids::B:
                    if(to != Ids::B) {
                        throw std::bad_cast();
                    }
                    break;
                case Ids::C:
                    if(to != Ids::C) {
                        throw std::bad_cast();
                    }
                    break;
                }
            }
#endif

            template<typename T> constexpr
            const T &max(const T &a, const T &b) {
                return a > b ? a : b;
            }
            template<typename T, typename... U> constexpr
            const T &max(const T &a, const U &... b) {
                return max(a, max(b...));
            }
            constexpr size_t sizeof_members = max(sizeof(A), sizeof(B), sizeof(C));
        }

        namespace dispatch {
            namespace A {
                void copy(target::A *thiz, const target::A &other) {
                    new ((target::A *)thiz) target::A((const target::A&)other);
                }
                void assign(target::A *thiz, const target::A &other) {
                    *((target::A *)thiz) = (const target::A&)other;
                }
                void dtor(target::A *thiz) {
                    ((target::A *)thiz)->~A();
                }
                void aecho(target::A *thiz) {
                    ((target::A *)thiz)->__clunion_virtual_aecho();
                }
                void vecho(target::A *thiz) {
                    ((target::A *)thiz)->__clunion_virtual_vecho();
                }
            };

            namespace B {
                void copy(target::A *thiz, const target::A &other) {
                    new ((target::B *)thiz) target::B((const target::B&)other);
                }
                void assign(target::A *thiz, const target::A &other) {
                    *((target::B *)thiz) = (const target::B&)other;
                }
                void dtor(target::A *thiz) {
                    ((target::B *)thiz)->~B();
                }
                void vecho(target::A *thiz) {
                    ((target::B *)thiz)->__clunion_virtual_vecho();
                }
            };

            namespace C {
                void copy(target::A *thiz, const target::A &other) {
                    new ((target::C *)thiz) target::C((const target::C&)other);
                }
                void assign(target::A *thiz, const target::A &other) {
                    *((target::C *)thiz) = (const target::C&)other;
                }
                void dtor(target::A *thiz) {
                    ((target::C *)thiz)->~C();
                }
                void vecho(target::A *thiz) {
                    ((target::C *)thiz)->__clunion_virtual_vecho();
                }
            };

            struct Vtable {
                void (*copy)(target::A*,const target::A&);
                void (*assign)(target::A*,const target::A&);
                void (*dtor)(target::A*);
                void (*aecho)(target::A*);
                void (*vecho)(target::A*);
            };
            Vtable vtables[] = {
                {
                    &A::copy,
                    &A::assign,
                    &A::dtor,
                    &A::aecho,
                    &A::vecho
                },
                {
                    &B::copy,
                    &B::assign,
                    &B::dtor,
                    &A::aecho,
                    &B::vecho
                },
                {
                    &C::copy,
                    &C::assign,
                    &C::dtor,
                    &A::aecho,
                    &C::vecho
                }
            };

            void copy(target::A *thiz, const target::A &other) {
                target::set_actual(thiz, target::get_actual(&other));
                vtables[(vindex_t)target::get_actual(thiz)].copy(thiz, other);
            }
            void dtor(target::A *thiz) {
                vtables[(vindex_t)target::get_actual(thiz)].dtor(thiz);
            }
            void aecho(target::A *thiz) {
                vtables[(vindex_t)target::get_actual(thiz)].aecho(thiz);
            }
            void vecho(target::A *thiz) {
                vtables[(vindex_t)target::get_actual(thiz)].vecho(thiz);
            }
            void assign(target::A *thiz, const target::A &other) {
                if(target::get_actual(thiz) != target::get_actual(&other)) {
#ifdef DYNAMIC_TYPE_CHECKING
                    check_cast(thiz, target::get_actual(&other));
#endif
                    dtor(thiz);
                    copy(thiz, other);
                } else {
                    vtables[(vindex_t)target::get_actual(thiz)].assign(thiz, other);
                }
            }
        }

        namespace container {
            struct A {
            protected:
                A() {}
                char members[target::sizeof_members];
                target::A *thiz() const {
                    return ((target::A *)members);
                }

            public:
                A(const A &other) {
#ifdef DYNAMIC_TYPE_CHECKING
                    target::set_decl(thiz(), Ids::A);
#endif
                    dispatch::copy(thiz(), *other.thiz());
                }
                A(int a_i_) {
#ifdef DYNAMIC_TYPE_CHECKING
                    target::set_decl(thiz(), Ids::A);
#endif
                    target::set_actual(thiz(), Ids::A);
                    new (thiz()) target::A(a_i_);
                }
                ~A() {
                    dispatch::dtor(thiz());
                }
                target::A *operator->() const {
                    return thiz();
                }
                A &operator=(const A &other) {
                    if(this != &other) {
                        dispatch::assign(thiz(), *other.thiz());
                    }
                    return *this;
                }
            };

            struct B : public A {
            protected:
                target::B *thiz() const {
                    return (target::B *)A::thiz();
                }
            public:
                B(const B &other) {
#ifdef DYNAMIC_TYPE_CHECKING
                    target::set_decl(thiz(), Ids::B);
#endif
                    dispatch::copy(thiz(), *other.thiz());
                }
                B(int a_i_, int b_i_) {
#ifdef DYNAMIC_TYPE_CHECKING
                    target::set_decl(thiz(), Ids::B);
#endif
                    target::set_actual(thiz(), Ids::B);
                    new (thiz()) target::B(a_i_, b_i_);
                }
                target::B *operator->() const {
                    return thiz();
                }
                B &operator=(const B &other) {
                    if(this != &other) {
                        dispatch::assign(thiz(), *other.thiz());
                    }
                    return *this;
                }
            };

            struct C : public A {
            protected:
                target::C *thiz() const {
                    return (target::C *)A::thiz();
                }
            public:
                C(const C &other) {
#ifdef DYNAMIC_TYPE_CHECKING
                    target::set_decl(thiz(), Ids::C);
#endif
                    dispatch::copy(thiz(), *other.thiz());
                }
                C(int a_i_, int c_i0_, int c_i1_) {
#ifdef DYNAMIC_TYPE_CHECKING
                    target::set_decl(thiz(), Ids::C);
#endif
                    target::set_actual(thiz(), Ids::C);
                    new (thiz()) target::C(a_i_, c_i0_, c_i1_);
                }
                target::C *operator->() const {
                    return thiz();
                }
                C &operator=(const C &other) {
                    if(this != &other) {
                        dispatch::assign(thiz(), *other.thiz());
                    }
                    return *this;
                }
            };
        }
    }
}

typedef __clunion::clunion_A::container::A A;
typedef __clunion::clunion_A::container::B B;
typedef __clunion::clunion_A::container::C C;

int main(int argc, const char **argv) {
    B b{11,12};
    b->aecho();
    b->secho();
    b->vecho();

    A &ar{b};
    ar->aecho();
    ar->secho();
    ar->vecho();

    A a{42};
    a->aecho();
    a->secho();
    a->vecho();

    C c{101,102,103};
    c->aecho();
    c->secho();
    c->vecho();

    cout << "---" << endl;
    a = c;
    a->secho();
    a->vecho();
    cout << "---" << endl;
}

// todo: move constructor

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

    virtual void aecho() {
        cout << "aecho()" << endl;
    }

    virtual void vecho() {
        cout << "A::vecho(), a_i=" << a_i << endl;
    }

    void secho() {
        cout << "A::secho()" << endl;
    }
};

clunion B : public A {
    int b_i;
    NonPOD0 np1;

    B(int a_i_, int b_i_) : A(a_i_), np1("np1") {
        cout << "B(" << a_i_ << ", " << b_i_ << ")" << endl;
        b_i = b_i_;
    }
    ~B() {
        cout << "~B()" << endl;
    }

    virtual void vecho() {
        cout << "B::vecho(), a_i=" << a_i << ", b_i=" << b_i << endl;
    }

    void secho() {
        cout << "B::secho()" << endl;
    }
};

clunion C : public A {
    int c_i0;
    int c_i1;
    NonPOD0 np1;

    C(int a_i_, int c_i0_, int c_i1_) : A(a_i_), np1("np1") {
        cout << "C(" << a_i_ << ", " << c_i0_ << ", " << c_i1_ << ")" << endl;
        c_i0 = c_i0_;
        c_i1 = c_i1_;
    }
    ~C() {
        cout << "~C()" << endl;
    }

    virtual void vecho() {
        cout << "C::vecho()(" << a_i << ", " << c_i0 << ", " << c_i1 << ")" << endl;
    }

    void secho() {
        cout << "C::secho()" << endl;
    }
};
*/
