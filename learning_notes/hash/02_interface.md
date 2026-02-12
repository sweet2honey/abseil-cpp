# absl::Hash 学习笔记（阶段二：接口与用法）

> 目标：会用、会扩展、会避坑。实现细节留到阶段三。

## 1) 头文件与命名空间
- 头文件：`#include "absl/hash/hash.h"`
- 命名空间：`absl`
- C++ 标准：通常 C++14+（Abseil 总体要求以项目配置为准）；示例以 C++17 编写。

## 2) 核心 API 清单（你最常用的）

### 类型
- `absl::Hash<T>`：通用哈希函子（可用于 `unordered_map` / `flat_hash_map` 等）。
- `absl::HashState`：hash state 的类型擦除包装（PImpl/虚函数等场景）。

### 函数
- `absl::HashOf(values...) -> size_t`：把多个值作为一个 tuple 的语义进行哈希。

### 扩展点（最重要）
- `template <class H> H AbslHashValue(H state, const T& value)`：为自定义类型提供可哈希支持（通过 ADL 发现）。

## 3) 交互方式：你能用它做什么

### A. 直接把 `absl::Hash<T>` 当成普通 hash functor
常见用法：
- 作为 `unordered_map` 的第三模板参数
- 直接调用 `absl::Hash<T>{}(value)` 得到 `size_t`

注意：
- 该 `size_t` 通常**不跨进程稳定**。
- 不要跨动态库边界传递 hash 值。

### B. 用 `AbslHashValue` 让你的类型“可哈希”

规则（最关键的契约）：
- `AbslHashValue` 只组合用于相等比较（`operator==`）的字段。
- 组合顺序必须与“你认为的结构语义”一致（有序 vs 无序）。
- 使用 `H::combine(...)` / `H::combine_contiguous(...)`；不要直接调用别人的 `AbslHashValue`。

典型写法（推荐 friend 方式，避免声明与定义分离导致 ADL 问题）：
```cpp
struct MyType {
  int a;
  std::string b;

  friend bool operator==(const MyType& x, const MyType& y) {
    return x.a == y.a && x.b == y.b;
  }

  template <typename H>
  friend H AbslHashValue(H h, const MyType& v) {
    return H::combine(std::move(h), v.a, v.b);
  }
};
```

### C. 用 `absl::HashState` 做类型擦除（PImpl/虚接口）
当你不能把哈希逻辑写成模板（例如虚函数 `virtual void HashValue(HashState)`），可用：
- `auto erased = absl::HashState::Create(&state);`
- 然后调用 `absl::HashState::combine(std::move(erased), ...)`

这会引入一定开销，除非必要不要用。

## 4) 语义说明（必须记住）
- **确定性（同一进程）**：同一进程内，对同一对象值，多次哈希应相同（实现依赖 seed/ASLR，但对单进程稳定）。
- **跨进程/跨 DSO 不保证**：不同进程或不同动态库实例的结果可能不同；不要把 hash 值当作稳定协议。
- **类型敏感**：`HashOf(2) != HashOf(2.0)`（类型不同即使值看起来“相等”）。
- **无序容器**：对 unordered 容器，框架会使用 order-independent 逻辑；同时注意“自定义 hash/equal 不兼容”的风险（见头文件注释）。

复杂度/性能：
- 基本是 $O(n)$（按字段/元素/字节数）。
- `combine_contiguous` 可能比逐元素更快（允许内部优化），但不保证与循环逐元素完全一致。

线程安全：
- `absl::Hash<T>` 本身是无状态可重入的（常见实现如此）；但你哈希的对象本身若并发修改，语义同样未定义。

异常安全：
- 通常不抛异常（取决于你在 `AbslHashValue` 中组合的成员操作是否会抛）。

## 5) 定制化/扩展点清单

### 扩展点：`AbslHashValue`（推荐）
你必须实现：
- `template <typename H> friend H AbslHashValue(H, const T&)`

你不应做：
- 不要特化 `absl::Hash<T>`（文档明确禁止）。
- 不要在别的命名空间/不相关文件里提供 `AbslHashValue`（ADL 要求）。

### 备用：`std::hash<T>`（不推荐，但可作为兼容）
当找不到 `AbslHashValue` 时，内部会尝试 fallback 到 `std::hash<T>`（主要为兼容）。

## 6) 常见陷阱（强烈建议过一遍）
- 只 `combine_contiguous(range)` 却忘了把长度也纳入哈希：容易出现“前缀/后缀”碰撞。
- `operator==` 与 `AbslHashValue` 用的字段不一致：会破坏哈希容器基本假设。
- 把 hash 值跨进程/跨动态库使用：违反框架假设。
- 对浮点做哈希：框架支持但通常不推荐（+0/-0、NaN 等语义复杂）。

## 7) 下一步建议
- 去阶段三：读 `HashSelect` 的选择顺序、seed、优化路径。
- 去阶段四：写 3 个小练习，做一次“字段一致性 + range 长度 + PImpl type-erasure”。
