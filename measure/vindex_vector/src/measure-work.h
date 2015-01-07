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
    vindex_t vindex;
    VindexOperator(OperatorType optype);

public:
    VindexOperator() {}
    typedef std::vector<VindexOperator> vec;

    uint op(uint accum);

    static VindexOperator create(OperatorType optype);

    VindexOperator *operator->() {
        return this;
    }
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
    return table_lookup.get(vindex)->op(accum);
}

//------------------------------------------------------------
//---
//--- Virtual
//---
//------------------------------------------------------------
struct VirtualOperator {
    typedef std::vector<std::unique_ptr<VirtualOperator> > vec;
    virtual ~VirtualOperator() {}
    virtual uint op(uint accum) = 0;

    static std::unique_ptr<VirtualOperator> create(OperatorType optype);
};
