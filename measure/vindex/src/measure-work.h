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

struct VindexTable {
    VindexTable(uint (*op_)(uint accum)) : op(op_) {
    }

    uint (*op)(uint accum);
};

struct VindexTableLookup {
    static VindexTable *tables[256];

    VindexTableLookup();

    inline VindexTable *get(uchar index) {
        return tables[index];
    }
};
extern VindexTableLookup table_lookup;

inline uint VindexOperator::op(uint accum) {
    return table_lookup.get(vindex)->op(accum);
}

//------------------------------------------------------------
//---
//--- Virtual
//---
//------------------------------------------------------------
struct VirtualOperator {
    virtual uint op(uint accum) = 0;

    static VirtualOperator *get(OperatorType optype);
};
