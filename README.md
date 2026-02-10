# NurbsDraw

## Project structure

## Geometry

### BezeirCurve

Bezeir curve is parameterised as:

$$
C(t)=\sum_{j=1}^{n} B_{j,n}(t)\vec P_j
$$

where
$$
B_{j,n}(t)=(1-t)B_{j,n-1}(t)+tB_{j-1,n-1}(t)
$$