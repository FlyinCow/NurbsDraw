# NurbsDraw

交互式NURBS/B样条曲线编辑器，基于C++23.

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

## Concepts

Evaluator: takes a C and a u, gives a Vec3
Discretizor: takes a C and an Evaluator, gives an array of Vec3 to draw