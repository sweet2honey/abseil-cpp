# absl::Hash 学习笔记（阶段四：练习与验证）

> 目标：用可编译练习固化理解。每题都留有 TODO，让你补全关键部分并验证。

练习代码已放到：`learning_notes/examples/`
- `hash_ex01.cc`
- `hash_ex02.cc`
- `hash_ex03.cc`
- `hash_ex04.cc`

## 编译方式（单文件快速验证）
在仓库根目录执行（不需要链接库时通常可行）：
- `c++ -std=c++17 -I. -O2 learning_notes/examples/hash_ex01.cc -o /tmp/h1 && /tmp/h1`

如果你希望更贴近项目构建，可改用 CMake 在 `build/` 下构建/运行（本仓库已有 build 目录）。

## 题 1（基础）：最小用法 + HashOf
目标：理解 `absl::Hash<T>` 与 `absl::HashOf` 的基本语义。
- 你需要补全：对字符串/tuple 的 HashOf 验证
- 预期：输出 `OK`，并且 `assert` 全部通过

文件：`learning_notes/examples/hash_ex01.cc`

## 题 2（基础）：自定义类型 + AbslHashValue
目标：为 `Point` 提供 `AbslHashValue`，并保证与 `operator==` 一致。
- 你需要补全：`AbslHashValue` 的 friend 实现
- 预期：相等对象哈希相等；不同对象大概率哈希不同

文件：`learning_notes/examples/hash_ex02.cc`

## 题 3（中级）：range/contiguous 的坑（别忘了长度）
目标：体验“只 hash 字节序列但不混入长度”会导致的等价性问题。
- 你需要补全：对 vector/string 的 hash 组合，确保把 size 也纳入
- 预期：避免前缀/后缀造成的等价冲突

文件：`learning_notes/examples/hash_ex03.cc`

## 题 4（进阶）：HashState 类型擦除（PImpl/虚接口）
目标：在不能模板化的场景，把 hash 贡献延迟到虚函数实现里。
- 你需要补全：`Interface::HashValue` 的实现、派生类组合字段
- 预期：两个派生类同值对象 hash 相等；不同值对象 hash 不同

文件：`learning_notes/examples/hash_ex04.cc`

## 完成定义（你可以打勾）
- [ ] 我能解释 `AbslHashValue` 的契约：与 `operator==` 字段一致
- [ ] 我能解释 `HashOf` 的“类型敏感”语义
- [ ] 我能解释 `combine_contiguous` 的用途和风险点（长度/一致性）
- [ ] 我能在 PImpl/虚接口里用 `absl::HashState` 正确贡献 hash
