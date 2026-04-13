# Interactive curves and Geometric continuity
## Project Objectives
This project focuses on the mathematical implementation and real-time visualization of parametric curves. The following core features were implemented as part of the computational geometry study:
* **Bézier Curve Evaluation**: Implementation of the **de Casteljau** algorithm to generate curves from a set of user-defined control points (CP).
* **Dynamic Control Point Manipulation**: Interactive modification of the curve's shape via real-time mouse-drag callbacks.
* **Catmull-Rom**: Generation of piecewise interpolating Bézier curves for smooth path construction.

*Developed as part of the Fundamentals of Computer Graphics course in Computer Engineering in Alma Mater Studiorum, Bologna, Italy.*

## Technical Implementation: De Casteljau’s Algorithm

The core of the Bézier evaluation is based on De Casteljau's algorithm, a numerically stable method to evaluate parametric curves. While theoretically recursive, the algorithm is implemented iteratively to optimize performance and stack usage, which is a standard practice in real-time graphics.

### **Technical Implementation: De Casteljau’s Algorithm**

The core of the Bézier evaluation system is based on **De Casteljau's algorithm**, a numerically stable method used to obtain the points of a parametric curve $f(t)$ through geometric construction. While the algorithm is theoretically recursive, it has been implemented **iteratively** in this project to optimize performance and stack usage, which is a critical requirement for real-time graphics applications.

#### **1. Mathematical Evaluation & Discretization**
Since a Bézier curve is a continuous function, it must be discretized into a **polyline** for visualization.The implementation samples the curve at **$N=201$ equidistant points** along the parameter $t \in [0, 1]$. 

For each point, the algorithm performs successive linear interpolations (**LERP**) between control points until a single point on the curve is reached. The recursive LERP process follows this logic:
$$P_i^k = (1 - t)P_i^{k-1} + tP_{i+1}^{k-1}$$

#### **2. Core Algorithm Implementation (C++)**
The following function evaluates the curve's coordinates for a specific parameter $t$. The outer loop manages the levels of interpolation, while the inner loop computes the LERP between adjacent points.

```cpp
// Evaluates a single point on the Bézier curve at parameter t
void de_casteljau_alghoritm(float t, float* result, int num_points, float points[][2]) {
    float cord_X[MaxNumPts];
    float cord_Y[MaxNumPts];

    // Initialize temporary coordinate arrays with control points
    for (int i = 0; i < num_points; i++) {
        cord_X[i] = points[i][0];
        cord_Y[i] = points[i][1];
    }

    // Iterative LERP process for numerical stability
    for (int i = 1; i < num_points; i++) {
        for (int j = 0; j < num_points - i; j++) {
            cord_X[j] = (1 - t) * cord_X[j] + t * cord_X[j + 1];
            cord_Y[j] = (1 - t) * cord_Y[j] + t * cord_Y[j + 1];
        }
    }

    // Resulting point on the curve
    result[0] = cord_X[0];
    result[1] = cord_Y[0];
}
```

### **3. Integration in the Rendering Loop**

The drawScene function manages the discretization process. It iterates through the defined steps (200 segments), invoking the algorithm for each t to update the vertex arrays used for rendering.
```cpp
void drawScene_deCasteljau(void) {
    if (NumPts > 1) {
        float result_dc[2];
        int steps_per_segment = 200; // Number of steps for discretization

        for (int j = 0; j <= steps_per_segment; j++) {
            // Calculate parameter t and evaluate position
            de_casteljau_alghoritm((GLfloat)j / steps_per_segment, result_dc, NumPts, vPositions_CP);
            
            // Store results in the vertex buffer for OpenGL rendering
            if (index_cat < MaxNumPts) {
                vPositions_C[index_cat][0] = result_dc[0];
                vPositions_C[index_cat][1] = result_dc[1];
                index_cat++;
            }
        }
    }
}
```
