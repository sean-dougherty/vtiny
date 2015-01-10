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
    typedef unsigned char vindex_t;

    template<typename T> constexpr
    T const& max(T const& a, T const& b) {
        return a > b ? a : b;
    }

    namespace clunion_A {
        namespace target {
            struct A;
        }
        namespace dispatch {
            void vecho(struct target::A *thiz);
        }

        enum class Ids {
            A, B
        };

        namespace target {
            struct A {
                int a_i;
                NonPOD0 np0;
#ifdef DYNAMIC_TYPE_CHECKING
                vindex_t __clunion_vindex_decl;
#endif
                vindex_t __clunion_vindex_actual;

                A(int a_i_) : np0("np0") {
                    cout << "A(" << a_i_ << ")" << endl;
                    a_i = a_i_;
                }
                ~A() {
                    cout << "~A()" << endl;
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
                    dispatch::vecho(this);
                }
                void __clunion_virtual_vecho() {
                    cout << "B::vecho(), a_i=" << a_i << ", b_i=" << b_i << endl;
                }
                void secho() {
                    cout << "B::secho()" << endl;
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
                }
            }
#endif

            constexpr size_t sizeof_members = max(sizeof(A), sizeof(B));
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

            struct Vtable {
                void (*copy)(target::A*,const target::A&);
                void (*assign)(target::A*,const target::A&);
                void (*dtor)(target::A*);
                void (*vecho)(target::A*);
            };
            Vtable vtables[] = {
                {
                    &A::copy,
                    &A::assign,
                    &A::dtor,
                    &A::vecho
                },
                {
                    &B::copy,
                    &B::assign,
                    &B::dtor,
                    &B::vecho
                }
            };

            void copy(target::A *thiz, const target::A &other) {
                target::set_actual(thiz, target::get_actual(&other));
                vtables[(vindex_t)target::get_actual(thiz)].copy(thiz, other);
            }
            void dtor(target::A *thiz) {
                vtables[(vindex_t)target::get_actual(thiz)].dtor(thiz);
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
        }
    }
}

typedef __clunion::clunion_A::container::A A;
typedef __clunion::clunion_A::container::B B;

int main(int argc, const char **argv) {
    B b{11,12};
    b->secho();
    b->vecho();

    A &ar{b};
    ar->secho();
    ar->vecho();

    A a{42};
    a->secho();
    a->vecho();
}

//todo: clunion C : public A

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
