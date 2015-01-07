#pragma once

#include "util.h"

enum OperatorType {
    Add,
    Sub,
    Mul,
    Div,
    __nops
};

//------------------------------------------------------------
//---
//--- Vswitch
//---
//------------------------------------------------------------
struct VswitchOperator {
private:
    uint pad;

public:
    uint op(uint accum);

    static VswitchOperator *create(OperatorType optype);
    static void dispose(VswitchOperator *op);
};

inline VswitchOperator *encode(VswitchOperator *op, OperatorType optype) {
    return (VswitchOperator *)(size_t(op) | optype);
}

inline OperatorType decode_type(VswitchOperator *op) {
    return OperatorType(size_t(op) & 0x3);
} 

inline VswitchOperator *decode_addr(VswitchOperator *op) {
    return (VswitchOperator *)(size_t(op) & ~0x3);
} 


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

inline uint VswitchOperator::op(uint accum) {
    switch(decode_type(this)) {
    case Add:
        return op_add(accum);
    case Sub:
        return op_sub(accum);
    case Mul:
        return op_mul(accum);
    case Div:
        return op_div(accum);
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
    uint pad;
    virtual ~VirtualOperator() {}
    virtual uint op(uint accum) = 0;

    static VirtualOperator *create(OperatorType optype);
    static void dispose(VirtualOperator *op);
};
