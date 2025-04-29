#include "ngx_mem_pool.h"

/**
 * @brief NgxMemPool 类的构造函数，用于初始化内存池。
 *
 * 该构造函数根据传入的 size 参数分配内存池，并初始化内存池的相关属性。
 * 如果分配的内存大小小于 NGX_MIN_POOL_SIZE，则使用 NGX_MIN_POOL_SIZE 作为分配大小。
 *
 * @param size 请求的内存池大小。如果该值小于 NGX_MIN_POOL_SIZE，则使用 NGX_MIN_POOL_SIZE。
 */
NgxMemPool::NgxMemPool(size_t size)
{
    // 分配内存池，确保分配的大小不小于 NGX_MIN_POOL_SIZE
    _pool = static_cast<NgxPool_t *>(malloc(size > NGX_MIN_POOL_SIZE ? size : NGX_MIN_POOL_SIZE));
    if (_pool == nullptr)
    {
        /*日志*/
        return;// 如果内存分配失败，直接返回
    }

    // 初始化内存池的 d 结构体，设置 last 和 end 指针
    _pool->d.last = (u_char *) _pool + sizeof(NgxPool_t);
    _pool->d.end = (u_char *) _pool + size;
    _pool->d.next = nullptr;
    _pool->d.failed = 0;

    // 计算并设置内存池的最大可分配大小
    size = size - sizeof(NgxPool_t);
    _pool->max = (size < NGX_MAX_ALLOC_FROM_POOL) ? size : NGX_MAX_ALLOC_FROM_POOL;

    // 初始化内存池的其他属性
    _pool->current = _pool;
    _pool->large = nullptr;
    _pool->cleanup = nullptr;
}
/**
 * @brief NgxMemPool类的析构函数，用于释放内存池中的所有资源。
 *
 * 该函数负责清理内存池中的各种资源，包括清理回调函数、大内存块以及内存池本身。
 * 通过遍历内存池中的各个链表，依次释放相关资源，确保内存池的完全释放。
 */
NgxMemPool::~NgxMemPool()
{
    NgxPool_t *p, *n;          // 用于遍历内存池链表的指针
    NgxPoolLarge_t *l;         // 用于遍历大内存块链表的指针
    NgxPoolCleanup_t *c;       // 用于遍历清理回调函数链表的指针

    // 遍历并执行所有清理回调函数
    for (c = _pool->cleanup; c; c = c->next)
    {
        if (c->handler)
        {
            c->handler(c->data); // 调用清理回调函数，释放相关资源
        }
    }

    // 遍历并释放所有大内存块
    for (l = _pool->large; l; l = l->next)
    {
        if (l->alloc)
        {
            free(l->alloc); // 释放大内存块
        }
    }

    // 遍历并释放内存池中的所有内存块
    for (p = _pool, n = _pool->d.next; /* void */; p = n, n = n->d.next)
    {
        free(p); // 释放当前内存块

        if (n == nullptr)
        {
            break; // 如果下一个内存块为空，结束循环
        }
    }
}

/**
 * @brief 重置内存池，释放所有大块内存并重置内存池状态。
 *
 * 该函数用于重置内存池，释放所有通过大块内存分配的内存，并将内存池的状态重置为初始状态。
 * 具体操作包括：
 * 1. 遍历并释放所有大块内存。
 * 2. 重置内存池的指针和状态，使其恢复到初始状态。
 */
void NgxMemPool::resetPool()
{
    NgxPool_t *p;
    NgxPoolLarge_t *l;

    // 遍历并释放所有大块内存
    for (l = _pool->large; l; l = l->next)
    {
        if (l->alloc)
        {
            free(l->alloc);
        }
    }

    // 重置第一块内存池的状态
    p = _pool;
    p->d.last = (u_char *) p + sizeof(NgxPool_t);
    p->d.failed = 0;

    // 重置后续内存池的状态
    for (p = p->d.next; p; p = p->d.next)
    {
        p->d.last = (u_char *) p + sizeof(NgxPoolData_t);
        p->d.failed = 0;
    }

    // 将当前内存池指针重置为第一块内存池，并清空大块内存链表
    _pool->current = _pool;
    _pool->large = nullptr;
}

/**
 * @brief 从内存池中分配指定大小的内存块，并进行内存对齐。
 *
 * 该函数根据请求的内存大小决定是从小块内存池还是大块内存池中分配内存。
 * 如果请求的大小小于或等于内存池中小块内存的最大大小，则从小块内存池中分配；
 * 否则，从大块内存池中分配。
 *
 * @param size 请求分配的内存大小。
 * @return void* 返回指向分配的内存块的指针。如果分配失败，返回NULL。
 */
void *NgxMemPool::palloc(size_t size)
{
    // 判断请求的内存大小是否小于或等于小块内存的最大大小
    if (size <= _pool->max)
    {
        // 从小块内存池中分配内存
        return pallocSmall(size, true);
    }
    // 从大块内存池中分配内存
    return pallocLarge(size);
}
/**
 * @brief 从内存池中分配指定大小的内存块，但不进行内存对齐。
 *
 * 该函数根据请求的内存大小决定是从小块内存池还是大块内存池中分配内存。
 * 如果请求的内存大小小于或等于内存池的最大小块大小，则从小块内存池中分配；
 * 否则，从大块内存池中分配。与 `palloc` 函数不同，`pnalloc` 不会对分配的内存进行对齐操作。
 *
 * @param size 请求分配的内存大小。
 * @return void* 返回指向分配内存的指针。如果分配失败，返回 NULL。
 */
void *NgxMemPool::pnalloc(size_t size)
{
    // 检查请求的内存大小是否小于或等于内存池的最大小块大小
    if (size <= _pool->max)
    {
        // 从小块内存池中分配内存，不进行内存对齐
        return pallocSmall(size, false);
    }
    // 从大块内存池中分配内存
    return pallocLarge(size);
}
/**
 * @brief 从内存池中分配指定大小的内存，并将其初始化为零。
 *
 * 该函数首先调用 `palloc` 从内存池中分配指定大小的内存块。如果分配成功，
 * 则使用 `memset` 将分配的内存块初始化为零。
 *
 * @param size 要分配的内存块的大小（以字节为单位）。
 * @return 返回指向分配的内存块的指针。如果分配失败，则返回 NULL。
 */
void *NgxMemPool::pcalloc(size_t size)
{
    void *p;

    // 从内存池中分配指定大小的内存块
    p = palloc(size);

    // 如果分配成功，将内存块初始化为零
    if (p)
    {
        memset(p, 0, size);
    }

    return p;
}
/**
 * @brief 从内存池中分配小块内存
 *
 * 该函数用于从内存池中分配指定大小的内存块。如果内存池的当前节点无法满足需求，
 * 则会尝试从下一个节点分配。如果所有节点都无法满足需求，则调用 pallocBlock 函数
 * 分配一个新的内存块。
 *
 * @param size 需要分配的内存大小
 * @param align 是否需要对内存地址进行对齐
 * @return void* 返回分配的内存地址，如果分配失败则返回 NULL
 */
void *NgxMemPool::pallocSmall(size_t size, bool align)
{
    u_char *m;
    NgxPool_t *p;

    p = _pool->current;

    do {
        m = p->d.last;

        // 如果需要对齐，则调整内存地址
        if (align)
        {
            m = ngx_align_ptr(m, NGX_ALIGNMENT);
        }

        // 检查当前节点是否有足够的空间
        if ((size_t) (p->d.end - m) >= size)
        {
            p->d.last = m + size;

            return m;
        }

        // 尝试从下一个节点分配
        p = p->d.next;

    } while (p);

    // 如果所有节点都无法满足需求，则分配一个新的内存块
    return pallocBlock(size);
}
/**
 * @brief 从内存池中分配大块内存
 *
 * 该函数用于从内存池中分配一块较大的内存。首先尝试使用标准库的 `malloc` 函数分配内存，
 * 如果分配成功，则将该内存块添加到内存池的大内存块链表中。如果链表中已有空闲的节点，
 * 则直接使用该节点。如果链表中的节点已满，则分配一个新的节点并将其插入到链表头部。
 *
 * @param size 需要分配的内存大小
 * @return void* 返回分配的内存指针，如果分配失败则返回 `nullptr`
 */
void *NgxMemPool::pallocLarge(size_t size)
{
    void *p;
    u_int n;
    NgxPoolLarge_t *large;

    // 使用标准库的 malloc 函数分配内存
    p = malloc(size);
    if (p == nullptr)
    {
        return nullptr;
    }

    n = 0;

    // 遍历大内存块链表，查找是否有空闲的节点
    for (large = _pool->large; large; large = large->next)
    {
        if (large->alloc == nullptr)
        {
            // 找到空闲节点，将分配的内存块挂载到该节点
            large->alloc = p;
            return p;
        }

        // 如果遍历的节点数超过 3 个，则跳出循环
        if (n++ > 3)
        {
            break;
        }
    }

    // 分配一个新的节点并将其插入到链表头部
    large = (NgxPoolLarge_t *) pallocSmall(sizeof(NgxPoolLarge_t), true);
    if (large == nullptr)
    {
        // 如果节点分配失败，则释放之前分配的内存块
        free(p);
        return nullptr;
    }

    // 将新节点插入到链表头部，并挂载分配的内存块
    large->alloc = p;
    large->next = _pool->large;
    _pool->large = large;

    return p;
}
/**
 * @brief 从内存池中分配一个内存块。
 *
 * 该函数用于在内存池中分配一个指定大小的内存块。如果当前内存池无法满足需求，
 * 则会创建一个新的内存池，并将其链接到现有内存池的链表中。
 *
 * @param size 需要分配的内存块大小。
 * @return 返回指向分配内存块的指针，如果分配失败则返回 nullptr。
 */
void *NgxMemPool::pallocBlock(size_t size)
{
    u_char *m;
    size_t psize;
    NgxPool_t *p, *newPool;

    // 计算当前内存池的剩余空间大小
    psize = (size_t) (_pool->d.end - (u_char *) _pool);

    // 分配一个新的内存池
    m = (u_char *) malloc(psize);
    if (m == nullptr)
    {
        return nullptr;
    }

    // 初始化新内存池的结构
    newPool = (NgxPool_t *) m;
    newPool->d.end = m + psize;
    newPool->d.next = nullptr;
    newPool->d.failed = 0;

    // 调整指针位置，确保内存对齐，并设置新内存池的 last 指针
    m += sizeof(NgxPoolData_t);
    m = ngx_align_ptr(m, NGX_ALIGNMENT);
    newPool->d.last = m + size;

    // 遍历内存池链表，更新 current 指针
    for (p = _pool->current; p->d.next; p = p->d.next)
    {
        if (p->d.failed++ > 4)
        {
            _pool->current = p->d.next;
        }
    }

    // 将新内存池链接到链表中
    p->d.next = newPool;

    return m;
}
NgxPoolCleanup_t *NgxMemPool::cleanupAdd(size_t size)
{
    NgxPoolCleanup_t *c;

    c = (NgxPoolCleanup_t *) palloc(sizeof(NgxPoolCleanup_t));
    if (c == nullptr)
    {
        return nullptr;
    }

    if (size)
    {
        c->data = palloc(size);
        if (c->data == nullptr)
        {
            return nullptr;
        }
    }
    else
    {
        c->data = nullptr;
    }

    c->handler = nullptr;
    c->next = _pool->cleanup;

    _pool->cleanup = c;

    return c;
}

/**
 * @brief 释放内存池中的指定内存块
 *
 * 该函数用于释放内存池中由指针 `p` 指向的内存块。如果该内存块存在于大内存块链表中，
 * 则将其释放，并将对应的链表节点移动到链表头部，以便后续快速访问。
 *
 * @param p 指向需要释放的内存块的指针
 */
void NgxMemPool::pfree(void *p)
{
    NgxPoolLarge_t *prev = nullptr;  // 用于记录当前节点的前一个节点
    NgxPoolLarge_t *l = _pool->large;// 获取大内存块链表的头节点

    // 遍历大内存块链表，查找需要释放的内存块
    while (l)
    {
        if (p == l->alloc)
        {
            free(l->alloc);    // 释放内存块
            l->alloc = nullptr;// 将指针置为空，防止悬空指针

            // 如果当前节点不是链表头节点，则将其移动到链表头部
            if (prev)
            {
                prev->next = l->next;  // 将前一个节点的 next 指向当前节点的下一个节点
                l->next = _pool->large;// 将当前节点的 next 指向链表头节点
                _pool->large = l;      // 将链表头节点更新为当前节点
            }
            return;// 内存块已释放，函数返回
        }
        prev = l;   // 更新前一个节点为当前节点
        l = l->next;// 移动到下一个节点
    }
}