#pragma once

struct StaticCall {
    void work();
};

struct VirtualCall {
    virtual void work() = 0;
};

struct VirtualCall0 : public VirtualCall {
    virtual void work() override;
};

