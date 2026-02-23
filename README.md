# Termi - 终端游戏框架

一个轻量级的 C++ 终端游戏引擎，内置贪吃蛇和俄罗斯方块两款经典游戏。

## 特性

- **跨终端图形引擎** - 自定义控制台渲染，支持 ANSI 彩色输出
- **两款经典游戏**
  - 贪吃蛇（支持 AI 自动驾驶）
  - 俄罗斯方块（内置 AI 控制器）
- **零外部依赖** - 纯 C++ 标准库实现
- **CMake 构建系统**，支持预设配置

## 项目结构

```
termi/
├── TermiCoreBase/          # 核心游戏引擎库
│   ├── include/            # 公共头文件
│   └── src/                # 实现源码
├── games/
│   ├── snake/              # 贪吃蛇游戏
│   │   ├── src/            # 游戏源码
│   │   └── tests/          # 单元测试
│   └── tetris/             # 俄罗斯方块
│       ├── src/            # 游戏源码
│       └── tests/          # 单元测试
├── doc/                    # 英文文档
├── CMakePresets.json       # 构建预设（团队共享）
└── CMakeUserPresets.json   # 本地构建预设
```

## 环境要求

- C++17 兼容编译器
- CMake 3.21+
- Linux 终端（支持 ANSI 转义码）

## 构建

```bash
# 配置
cmake --preset=debug      # 或: cmake --preset=release

# 构建
cmake --build --preset=debug

# 运行测试
ctest --preset=test-all
```

## 游戏操作

### 贪吃蛇
移动: `W/A/S/D` 或方向键  
暂停: `P`  
AI 模式: `M`  
加速模式: `F`  
重置: `R`

### 俄罗斯方块
移动: `A/D`  
旋转: `W` 或 `↑`  
软降: `S` 或 `↓`  
硬降: `空格`  
AI 模式: `M`  
暂停: `P`  
重置: `R`

## 许可证

版权所有 © 2024 [Theqiqi]

- **个人使用**：免费，但需保留作者署名
- **商业授权**：请联系作者获取授权
- **传染性**：基于本软件修改的作品必须采用相同许可证发布
