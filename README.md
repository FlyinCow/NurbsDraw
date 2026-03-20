# NurbsDraw

交互式NURBS/B样条曲线编辑器，基于C++23、ImGui (docking分支) 和 SDL3 构建。

## 功能特性

- **曲线类型**：Bezier曲线、B样条曲线、NURBS曲线
- **曲面支持**：B样条曲面、NURBS曲面（规划中）
- **交互功能**：
  - 鼠标点击添加控制点
  - 拖拽移动控制点
  - 视图平移（鼠标中键拖拽）
  - 视图缩放（滚轮）
- **曲线操作**：求值、采样、绘制
- **2D/3D视图**：支持二维和三维几何编辑

## 项目结构

```
src/
├── app/              # 应用层（占位）
├── gui/
│   ├── adaptor/      # 适配器层（占位）
│   ├── widget/       # 原子级ImGui小部件
│   │   ├── Canvas.h          # 交互式画布
│   │   ├── ControlButton.h   # 控制按钮
│   │   └── CoordList.h      # 坐标列表
│   ├── widgets/      # 复合小部件（占位）
│   └── window/       # 窗口级函数
│       ├── DrawBezier.h     # 贝塞尔曲线窗口
│       └── Manager.h        # 窗口管理器
└── math/             # 几何原语
    ├── Vector.h              # 3D向量
    ├── BezierCurve.h         # 贝塞尔曲线
    ├── BernsteinBase.h       # 伯恩斯坦基
    └── BCurve.h              # B样条/NURBS曲线
```

## 构建

```bash
# 配置（Debug, Ninja, MSVC工具链）
cmake --preset windows-msvc

# 构建 → build/windows-msvc/NurbsDraw.exe
cmake --build --preset windows
```


## 关键模式

- Canvas小部件使用世界坐标系统，支持 `world_to_screen`/`screen_to_world` 转换
- 需要修改调用者状态时，通过**引用**传递（如 `bool &drawing`）
- `BezierCurve::eval(t)` 使用伯恩斯坦基函数

## 数学背景

### Bezier曲线

参数化形式为：

$$
C(t)=\sum_{j=0}^{n} B_{j,n}(t)\vec P_j
$$

其中 $B_{j,n}(t)$ 是伯恩斯坦基函数：

$$
B_{j,n}(t)=\binom{n}{j}(1-t)^{n-j}t^j
$$

### B样条曲线

B样条曲线由控制顶点、节点向量和度数定义：

$$
C(t)=\sum_{i=0}^{n} N_{i,p}(t) \vec P_i
$$

其中 $N_{i,p}(t)$ 是p度的B样条基函数。

### NURBS曲线

NURBS（非均匀有理B样条）是B样条的推广，引入权重：

$$
C(t)=\frac{\sum_{i=0}^{n} N_{i,p}(t) w_i \vec P_i}{\sum_{i=0}^{n} N_{i,p}(t) w_i}
$$

## 依赖

- **ImGui** — `docking` 分支，SDL3后端
- **SDL3** — `main` 分支，静态链接

## 许可证

MIT License
