# AGENTS

本文件定义本项目中 AI/自动化协作者的统一工作约定。

## 1. 基本约束

- 代码语言标准：`C++20`
- 代码格式规范：严格遵循仓库根目录 [`.clang-format`](./.clang-format)
- 命名规范：严格遵循仓库根目录 [`.clang-tidy`](./.clang-tidy)（当前主要启用 `readability-identifier-naming`）

## 2. 项目介绍

项目是一个基于 C++20 和 OpenGL 的绘图应用，包含以下核心模块：
1. base：基础工具层（日志、通用工具）
2. core：基础核心类型/框架
3. math：几何与数学
4. thirdparty：第三方库（如 clipper2, magic_enum, thread-pool）
5. gal：图形抽象层与 OpenGL 实现（含 shaders）
6. view：视图系统、图层、R-tree 查询与绘制调度
7. widget：UI 面板与交互控件
8. font：字体子系统
9. data：数据对象与 painter（line/circle/text 等）

项目的构建系统基于 CMake，使用 Ninja 作为构建工具，支持 Debug 和 Release 两种构建类型。项目的编译输出位于 `build-ninja` 目录下，包含 Debug 和 Release 两个子目录。
项目使用 vcpkg 管理第三方库，相关配置位于 `vcpkg.json` 文件中。

## 3. 编译与运行

项目编译/运行入口定义在 [`.codex/environments/environment.toml`](./.codex/environments/environment.toml)。

按该文件约定，执行流程为：

```powershell
./build.bat Debug
.\build-ninja\Debug\mini.exe
```

支持的构建方式：

```powershell
./build.bat             # 默认 Debug
./build.bat Debug
./build.bat Release
```

Release 运行路径：

```powershell
.\build-ninja\Release\mini.exe
```

除非用户明确要求，不要自行替换为其他构建链路。

## 4. 代码修改流程

1. 先最小化修改，避免无关重构。
2. 修改后确保代码符合 `.clang-format` 与 `.clang-tidy` 命名约束。
3. 优先在影响范围内自检编译通过，再给出变更说明。
4. 若发现现有代码与本规范冲突，优先保持用户当前功能正确，再在说明中标注冲突点。

## 5. 命名规则速记（来自 `.clang-tidy`）

- 类/结构体/函数/枚举：`CamelCase`
- 普通变量/成员变量：`camelBack`
- 私有成员前缀：`m_`
- 全局变量前缀：`g_`
- 常量前缀：`k`（`camelCase`）
- 宏：`UPPER_CASE`

## 6. 提交说明建议

- 说明应包含：改了什么、为什么改、如何验证。
- 如未完成本地验证，必须明确说明未验证项及原因。
