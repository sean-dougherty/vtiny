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
typedef uchar vindex_t;

struct VindexTable {
    void init(uint (*op_)(uint accum)) {
        op = op_;
    }

    uint (*op)(uint accum);
};

struct VindexOperator {
private:
    //vindex_t vindex;
    VindexTable *table;
    VindexOperator(OperatorType optype);

public:
    uint op(uint accum);

    static VindexOperator *create(OperatorType optype);
    static void dispose(VindexOperator *op);
};

struct VindexTableLookup {
    VindexTable *tables;

    VindexTableLookup();

    inline VindexTable *get(vindex_t vindex) {
        return tables + vindex;
    }
};
extern VindexTableLookup table_lookup;

inline uint VindexOperator::op(uint accum) {
    //return table_lookup.get(vindex)->op(accum);
    return table->op(accum);
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
