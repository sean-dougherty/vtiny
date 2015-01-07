#include "measure-work.h"
#include "util.h"

//------------------------------------------------------------
//---
//--- Vindex
//---
//------------------------------------------------------------
//VindexTable VindexTableLookup::tables[256];

VindexTableLookup table_lookup;

struct VindexAdd : VindexOperator {
    static uint op(uint accum) {
        return accum + 2;
    }
};

struct VindexSub : VindexOperator {
    static uint op(uint accum) {
        return accum - 1;
    }
};

VindexTableLookup::VindexTableLookup() {
    tables = new VindexTable[256];
    tables[Add].init(&VindexAdd::op);
    tables[Sub].init(&VindexSub::op);
}

VindexOperator::VindexOperator(OperatorType optype)
    : vindex(optype) {
}

VindexOperator VindexOperator::create(OperatorType optype) {
    switch(optype) {
    case Add:
        return VindexOperator(Add);
    case Sub:
        return VindexOperator(Sub);
    default:
        abort();
    }
}

//------------------------------------------------------------
//---
//--- Virtual
//---
//------------------------------------------------------------
struct VirtualAdd : public VirtualOperator {
    virtual uint op(uint accum) override  {
        return accum + 2;
    }
};

struct VirtualSub : public VirtualOperator {
    virtual uint op(uint accum) override  {
        return accum - 1;
    }
};

std::unique_ptr<VirtualOperator> VirtualOperator::create(OperatorType optype) {
    switch(optype) {
    case Add:
        return std::unique_ptr<VirtualOperator>(new VirtualAdd());
    case Sub:
        return std::unique_ptr<VirtualOperator>(new VirtualSub());
    default:
        abort();
    }
}
