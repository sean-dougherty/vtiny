#include "measure-work.h"
#include "util.h"

//------------------------------------------------------------
//---
//--- Vswitch
//---
//------------------------------------------------------------

VswitchOperator *VswitchOperator::create(OperatorType optype) {
    return encode(new VswitchOperator(), optype);
}

void VswitchOperator::dispose(VswitchOperator *op) {
    delete decode_addr(op);
}

//------------------------------------------------------------
//---
//--- Virtual
//---
//------------------------------------------------------------
struct VirtualAdd : public VirtualOperator {
    virtual uint op(uint accum) override  {
        return op_add(accum);
    }
};

struct VirtualSub : public VirtualOperator {
    virtual uint op(uint accum) override  {
        return op_sub(accum);
    }
};

struct VirtualMul : public VirtualOperator {
    virtual uint op(uint accum) override  {
        return op_mul(accum);
    }
};

struct VirtualDiv : public VirtualOperator {
    virtual uint op(uint accum) override  {
        return op_div(accum);
    }
};

VirtualOperator *VirtualOperator::create(OperatorType optype) {
    switch(optype) {
    case Add:
        return new VirtualAdd();
    case Sub:
        return new VirtualSub();
    case Mul:
        return new VirtualMul();
    case Div:
        return new VirtualDiv();
    default:
        abort();
    }
}

void VirtualOperator::dispose(VirtualOperator *op) {
    delete op;
}
