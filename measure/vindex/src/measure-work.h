#pragma once

#include "util.h"

enum OperatorType {
    Add,
    Sub,
    __nops
};

//------------------------------------------------------------
//---
//--- Vindex
//---
//------------------------------------------------------------
struct VindexOperator {
private:
    uchar vindex;
    VindexOperator(OperatorType optype);

public:
    uint op(uint accum);

    static VindexOperator *get(OperatorType optype);
};

//------------------------------------------------------------
//---
//--- Virtual
//---
//------------------------------------------------------------
struct VirtualOperator {
    virtual uint op(uint accum) = 0;

    static VirtualOperator *get(OperatorType optype);
};
