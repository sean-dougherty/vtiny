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

        enum class Ids {
            A, B
        };

        namespace target {
            struct A;
        }

        namespace dispatch {
            void vecho(struct target::A *thiz);
        }

        namespace target {
            struct A {
                int a_i;
                NonPOD0 np0;
#ifdef DYNAMIC_TYPE_CHECKING
                vindex_t __clunion_vindex_decl;
#endif
                vindex_t __clunion_vindex_actual;

#ifdef DYNAMIC_TYPE_CHECKING
                void __clunion_set_vindex_decl(Ids id) {
                    __clunion_vindex_decl = (vindex_t)id;
                }
#endif
                void __clunion_set_vindex_actual(Ids id) {
                    __clunion_vindex_actual = (vindex_t)id;
                }

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

#ifdef DYNAMIC_TYPE_CHECKING
            vindex_t get_vindex_decl(const target::A *thiz) {
                return thiz->__clunion_vindex_decl;
            }
#endif
            vindex_t get_vindex_actual(const target::A *thiz) {
                return thiz->__clunion_vindex_actual;
            }
            void set_vindex_actual(target::A *thiz, vindex_t vindex) {
                thiz->__clunion_vindex_actual = vindex;
            }
#ifdef DYNAMIC_TYPE_CHECKING
            void check_cast(target::A *thiz, vindex_t to) {
                switch(Ids(get_vindex_decl(thiz))) {
                case Ids::A:
                    break;
                case Ids::B:
                    if(to != vindex_t(Ids::B)) {
                        throw std::bad_cast();
                    }
                    break;
                }
            }
#endif

            void copy(target::A *thiz, const target::A &other) {
                set_vindex_actual(thiz, get_vindex_actual(&other));
                vtables[get_vindex_actual(thiz)].copy(thiz, other);
            }
            void dtor(target::A *thiz) {
                vtables[get_vindex_actual(thiz)].dtor(thiz);
            }
            void vecho(target::A *thiz) {
                vtables[get_vindex_actual(thiz)].vecho(thiz);
            }
            void assign(target::A *thiz, const target::A &other) {
                if(get_vindex_actual(thiz) != get_vindex_actual(&other)) {
#ifdef DYNAMIC_TYPE_CHECKING
                    check_cast(thiz, get_vindex_actual(&other));
#endif
                    dtor(thiz);
                    copy(thiz, other);
                } else {
                    vtables[get_vindex_actual(thiz)].assign(thiz, other);
                }
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
    char __clunion_members[__clunion::clunion_A::target::sizeof_members];
    __clunion::clunion_A::target::A *__clunion_this() const {
        return ((__clunion::clunion_A::target::A *)__clunion_members);
    }

public:
    A(const A &other) {
#ifdef DYNAMIC_TYPE_CHECKING
        __clunion_this()->__clunion_set_vindex_decl(__clunion::clunion_A::Ids::A);
#endif
        __clunion::clunion_A::dispatch::copy(__clunion_this(), *other.__clunion_this());
    }
    A(int a_i_) {
#ifdef DYNAMIC_TYPE_CHECKING
        __clunion_this()->__clunion_set_vindex_decl(__clunion::clunion_A::Ids::A);
#endif
        __clunion_this()->__clunion_set_vindex_actual(__clunion::clunion_A::Ids::A);
        new (__clunion_this()) __clunion::clunion_A::target::A(a_i_);
    }
    ~A() {
        __clunion::clunion_A::dispatch::dtor(__clunion_this());
    }
    __clunion::clunion_A::target::A *operator->() const {
        return __clunion_this();
    }
    A &operator=(const A &other) {
        if(this != &other) {
            __clunion::clunion_A::dispatch::assign(__clunion_this(), *other.__clunion_this());
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
    __clunion::clunion_A::target::B *__clunion_this() const {
        return (__clunion::clunion_A::target::B *)A::__clunion_this();
    }
public:
    B(const B &other) {
#ifdef DYNAMIC_TYPE_CHECKING
        __clunion_this()->__clunion_set_vindex_decl(__clunion::clunion_A::Ids::B);
#endif
        __clunion::clunion_A::dispatch::copy(__clunion_this(), *other.__clunion_this());
    }
    B(int a_i_, int b_i_) {
#ifdef DYNAMIC_TYPE_CHECKING
        __clunion_this()->__clunion_set_vindex_decl(__clunion::clunion_A::Ids::B);
#endif
        __clunion_this()->__clunion_set_vindex_actual(__clunion::clunion_A::Ids::B);
        new (__clunion_this()) __clunion::clunion_A::target::B(a_i_, b_i_);
    }
    __clunion::clunion_A::target::B *operator->() const {
        return __clunion_this();
    }
    B &operator=(const B &other) {
        if(this != &other) {
            __clunion::clunion_A::dispatch::assign(__clunion_this(), *other.__clunion_this());
        }
        return *this;
    }
};

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
