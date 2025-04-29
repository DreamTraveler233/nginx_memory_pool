//
// Created by shuzeyong on 2025/4/29.
//

#ifndef NGINX_MEMORY_POOL_NGX_MEM_POOL_H
#define NGINX_MEMORY_POOL_NGX_MEM_POOL_H

#include <cstdint>
#include <cstdlib>
#include <cstring>

// 定义清理操作函数指针类型
typedef void (*NgxPoolCleanupPt)(void *data);

/*清理操作结构体*/
struct NgxPoolCleanup_t
{
    NgxPoolCleanupPt handler;// 指向清理操作函数的指针，用于在销毁内存池时执行特定的清理操作
    void *data;              // 传递给清理操作函数的数据
    NgxPoolCleanup_t *next;  // 指向下一个清理操作节点的指针，用于链接多个清理操作节点
};

/*大块内存分配结构体*/
struct NgxPoolLarge_t
{
    NgxPoolLarge_t *next;// 指向下一个大块内存分配节点的指针，用于链接多个大块内存分配节点
    void *alloc;         // 指向实际分配的大块内存的指针
};

struct NgxPool_t;
/*小块内存数据结构体*/
struct NgxPoolData_t
{
    u_char *last;   // 指向当前内存池块中剩余可用内存的起始位置
    u_char *end;    // 指向当前内存池块中内存的末尾位置
    NgxPool_t *next;// 指向下一个内存块的指针，用于链接多个内存块
    u_int failed;   // 记录该内存块分配失败的次数
};

struct NgxPool_t
{
    NgxPoolData_t d;          // 小块内存头部信息
    size_t max;               // 允许从内存池中分配的小块内存的最大大小
    NgxPool_t *current;       // 指向当前用于分配小块内存的内存池块
    NgxPoolLarge_t *large;    // 指向大块内存分配链表的头节点，用于管理大块内存的分配
    NgxPoolCleanup_t *cleanup;// 指向清理操作链表的头节点，用于在销毁内存池时执行清理操作
};

/*将整数 d 向上对齐到 a 的倍数*/
#define ngx_align(d, a) (((d) + (a - 1)) & ~(a - 1))
/*将指针 p 向上对齐到 a 的倍数*/
#define ngx_align_ptr(p, a) (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

/*一个物理页面的大小 4KB*/
const int ngx_pagesize = 4096;
/*小块内存池内存大小最大值*/
const int NGX_MAX_ALLOC_FROM_POOL = ngx_pagesize - 1;
/*内存池大小按照16字节对齐*/
const int NGX_POOL_ALIGNMENT = 16;
/*内存池最小内存大小*/
const int NGX_MIN_POOL_SIZE = ngx_align((sizeof(NgxPool_t) + 2 * sizeof(NgxPoolLarge_t)), NGX_POOL_ALIGNMENT);
const int NGX_ALIGNMENT = sizeof(unsigned long);

class NgxMemPool
{
public:
    /*构造函数，创建一个新的内存池，分配指定大小的内存块，并初始化内存池的各个成员变量*/
    explicit NgxMemPool(size_t size = NGX_MIN_POOL_SIZE);
    /*销毁内存池，释放所有分配的内存，并执行所有清理操作*/
    ~NgxMemPool();
    /*重置内存池，释放所有大块内存，并将小块内存的分配位置重置为初始状态*/
    void resetPool();
    /*从内存池中分配对齐的内存（小块或大块）*/
    void *palloc(size_t size);
    /*分配非对齐的内存*/
    void *pnalloc(size_t size);
    /*分配并清零内存*/
    void *pcalloc(size_t size);
    /*释放从内存池中分配的大块内存*/
    void pfree(void *p);
    /*注册清理回调*/
    NgxPoolCleanup_t *cleanupAdd(size_t size);

private:
    /*处理小块内存分配*/
    inline void *pallocSmall(size_t size, bool align);
    /*分配新的内存池块*/
    inline void *pallocBlock(size_t size);
    /*分配大内存并加入链表*/
    inline void *pallocLarge(size_t size);

    NgxPool_t *_pool;
};

#endif//NGINX_MEMORY_POOL_NGX_MEM_POOL_H
