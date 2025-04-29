#include "ngx_mem_pool.h"
#include <cassert>
#include <iostream>

// 清理回调示例
void cleanup_handler(void* data) {
    std::cout << "Cleanup handler called with data: " << *(int*)data << std::endl;
}

void test_ngx_mem_pool() {
    // 1. 测试初始化和小块内存分配
    NgxMemPool pool(1024);
    void* p1 = pool.palloc(64);
    assert(p1 != nullptr);
    std::cout << "Small block allocated at: " << p1 << std::endl;

    // 2. 测试对齐
    void* p2 = pool.palloc(128);
    assert(p2 != nullptr);
    assert((uintptr_t)p2 % NGX_ALIGNMENT == 0);
    std::cout << "Aligned block allocated at: " << p2 << std::endl;

    // 3. 触发大块内存分配
    void* p3 = pool.palloc(NGX_MAX_ALLOC_FROM_POOL + 1);
    assert(p3 != nullptr);
    std::cout << "Large block allocated at: " << p3 << std::endl;

    // 4. 测试pcalloc清零
    int* p4 = static_cast<int*>(pool.pcalloc(10 * sizeof(int)));
    for (int i = 0; i < 10; ++i) {
        assert(p4[i] == 0);
    }
    std::cout << "pcalloc zero-initialized memory verified." << std::endl;

    // 5. 测试清理回调
    NgxPoolCleanup_t* cleanup = pool.cleanupAdd(sizeof(int));
    assert(cleanup != nullptr);
    cleanup->handler = cleanup_handler;
    int* data = static_cast<int*>(cleanup->data);
    *data = 42;

    // 6. 测试pfree
    pool.pfree(p3);
    std::cout << "Large block freed." << std::endl;

    // 7. 测试resetPool
    pool.resetPool();
    void* p5 = pool.palloc(64);
    assert(p5 != nullptr);
    std::cout << "After reset, small block allocated at: " << p5 << std::endl;

    // 析构时自动调用清理回调
}

int main() {
    test_ngx_mem_pool();
    std::cout << "All tests passed." << std::endl;
    return 0;
}