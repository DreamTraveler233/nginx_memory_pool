## 项目介绍：Nginx风格内存池（NgxMemPool）

### 特点
- **高效内存管理**  
  基于Nginx内存池设计，支持**小块内存池内连续分配**和**大块内存独立管理**，减少内存碎片，提升分配效率。
- **自动资源清理**  
  支持注册清理回调函数，内存池销毁时自动释放资源（如文件描述符、数据库连接等），避免内存泄漏。
- **内存对齐优化**  
  提供对齐（`palloc`）和非对齐（`pnalloc`）分配方式，满足不同场景的性能需求。
- **快速重置机制**  
  `resetPool()` 方法可重置内存池状态，保留已分配的内存块以供重用，适合高频分配场景（如HTTP请求处理）。
- **轻量级实现**  
  无依赖标准库外的组件，代码简洁，适用于嵌入式系统或高性能服务器。

---

### 适用场景
- **高并发网络服务**  
  如Web服务器、API网关，需频繁处理请求并分配临时内存。
- **长生命周期对象管理**  
  适合需要统一管理多个对象内存的场景（如数据库连接池）。
- **减少内存碎片**  
  对内存碎片敏感的应用（如实时系统、游戏服务器）。
- **资源自动回收**  
  需要确保资源（如文件句柄）随内存池释放的应用。

---

## 使用说明书

### 安装与集成
1. **克隆仓库**  
   ```bash
   git clone https://github.com/yourusername/ngx_mem_pool.git
   ```
2. **头文件引入**  
   将 `ngx_mem_pool.h` 和 `ngx_mem_pool.cpp` 添加到项目中，包含头文件即可使用：
   ```cpp
   #include "ngx_mem_pool.h"
   ```

---

### API 说明
| 方法                | 功能描述                                                                 |
|---------------------|--------------------------------------------------------------------------|
| `NgxMemPool(size_t)`| 构造函数，初始化内存池（默认大小 `NGX_MIN_POOL_SIZE=4096`）。           |
| `~NgxMemPool()`     | 析构函数，自动释放所有内存并执行清理回调。                              |
| `palloc(size_t)`    | 分配对齐内存，适合结构体等需要对齐的场景。                              |
| `pnalloc(size_t)`   | 分配非对齐内存，适合对性能要求更高的场景。                              |
| `pcalloc(size_t)`   | 分配内存并初始化为零。                                                  |
| `pfree(void*)`      | 释放大块内存（仅对大块内存有效）。                                      |
| `resetPool()`       | 重置内存池，保留内存块并重置指针，适合循环使用场景。                    |
| `cleanupAdd(size_t)`| 注册清理回调，返回的 `NgxPoolCleanup_t` 需手动设置 `handler` 和 `data`。|

---

### 示例代码
```cpp
// 创建内存池（默认大小4KB）
NgxMemPool pool;

// 分配对齐内存
int* arr = static_cast<int*>(pool.palloc(100 * sizeof(int)));

// 分配并清零内存
char* buffer = static_cast<char*>(pool.pcalloc(1024));

// 注册清理回调（例如关闭文件）
NgxPoolCleanup_t* cleanup = pool.cleanupAdd(sizeof(FILE*));
FILE* fp = fopen("test.txt", "r");
cleanup->data = fp;
cleanup->handler = [](void* data) { fclose(static_cast<FILE*>(data)); };

// 重置内存池（保留内存块，释放大内存）
pool.resetPool();

// 析构时自动释放所有资源
```

---

### 注意事项
1. **线程安全**  
   默认未加锁，多线程环境需自行封装互斥机制。
2. **小块内存无需手动释放**  
   仅大块内存需调用 `pfree()`，小块内存随池销毁自动释放。
3. **对齐分配的开销**  
   使用 `palloc` 时会有地址对齐的额外计算，对性能敏感场景建议测试后选择对齐方式。
4. **重置内存池**  
   `resetPool()` 不会缩小内存池物理大小，适合长期运行的服务。

---

### 许可证
MIT License. 自由用于开源和商业项目。
