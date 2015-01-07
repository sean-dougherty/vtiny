#pragma once

#include "util.h"

enum OperatorType {
    Add,
    Sub,
    Mul,
    Div,
    Lsh,
    Rsh,
    __nops
};

//------------------------------------------------------------
//---
//--- Vswitch
//---
//------------------------------------------------------------
typedef uchar vswitch_t;

struct VswitchOperator {
private:
    vswitch_t vswitch;
    VswitchOperator(OperatorType optype);

public:
    uint op(uint accum);

    static VswitchOperator *create(OperatorType optype);
    static void dispose(VswitchOperator *op);
};

inline uint op_add(uint accum) {
    return accum + 2;
};

inline uint op_sub(uint accum) {
    return accum - 1;
};

inline uint op_mul(uint accum) {
    return accum * 4;
};

inline uint op_div(uint accum) {
    return accum / 2;
};

inline uint op_lsh(uint accum) {
    return accum << 1;
};

inline uint op_rsh(uint accum) {
    return accum >> 1;
};

inline uint VswitchOperator::op(uint accum) {
    switch(vswitch) {
    case Add:
        return op_add(accum);
    case Sub:
        return op_sub(accum);
    case Mul:
        return op_mul(accum);
    case Div:
        return op_div(accum);
    case Lsh:
        return op_lsh(accum);
    case Rsh:
        return op_rsh(accum);
    default:
        abort();
    }
}

//------------------------------------------------------------
//---
//--- Virtual
//---
//------------------------------------------------------------
struct VirtualOperator {
    virtual ~VirtualOperator() {}
    virtual uint op(uint accum) = 0;

    static VirtualOperator *create(OperatorType optype);
    static void dispose(VirtualOperator *op);
};
