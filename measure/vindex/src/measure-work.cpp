#include "measure-work.h"
#include "util.h"

//------------------------------------------------------------
//---
//--- Vindex
//---
//------------------------------------------------------------
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

struct VindexTable {
    VindexTable(uint (*op_)(uint accum)) : op(op_) {
    }

    uint (*op)(uint accum);
};

struct VindexTableLookup {
    VindexTable *tables[256];

    VindexTableLookup() {
        memset(tables, 0, sizeof(tables));
        tables[Add] = new VindexTable(&VindexAdd::op);
        tables[Sub] = new VindexTable(&VindexSub::op);
    }

    VindexTable *get(uchar index) {
        return tables[index];
    }
} table_lookup;

VindexOperator::VindexOperator(OperatorType optype)
    : vindex(optype) {
}

uint VindexOperator::op(uint accum) {
    return table_lookup.get(vindex)->op(accum);
}

VindexOperator *VindexOperator::get(OperatorType optype) {
    static VindexOperator *ops[] = {new VindexOperator(Add), new VindexOperator(Sub)};
    return ops[optype];
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

VirtualOperator *VirtualOperator::get(OperatorType optype) {
    static VirtualOperator *ops[] = {new VirtualAdd(), new VirtualSub()};
    return ops[optype];
}
