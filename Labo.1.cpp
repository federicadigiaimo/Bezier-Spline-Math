/*
 * Lab-01.c
 *
 *     This program draws straight lines connecting dots placed with mouse clicks.
 *
 * Usage:
 *   Left click to place a control point.
 *		Maximum number of control points allowed is currently set at 300.
 *	 Press "f" to remove the first control point
 *	 Press "l" to remove the last control point.
 *	 Press escape to exit.
 */
#include "lib.h"
#include "ShaderMaker.h"
#include "gestione_callback.h"
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


int		width = 800;
int		height = 800;

///////////////////////////////////////////////////////////
static unsigned int programId;

float r = 0.0, g = 0.5, b = 1.0;

unsigned int    vao, vposition_CP_ID;
unsigned int    vao_2, vposition_Curve_ID;

#define MaxNumPts 400
float   vPositions_CP[MaxNumPts][2];
float   vPositions_C[MaxNumPts][2];

int     mouseOverIndex = -1;
int     isMovingPoint = false;
int     NumPts = 0;
bool    isDragging = false;
int     selectedPoint = -1;

//----------------------------------------------------------------------------------------
void removeFirstPoint() {
    int i;
    if (NumPts > 0) {
        // Remove the first point, slide the rest down
        NumPts--;
        for (i = 0; i < NumPts; i++) {
            vPositions_CP[i][0] = vPositions_CP[i + 1][0];
            vPositions_CP[i][1] = vPositions_CP[i + 1][1];
        }
    }
}
// Add a new point to the end of the list.  
// Remove the first point in the list if too many points.
void removeLastPoint() {
    if (NumPts > 0) {
        NumPts--;
    }
}
// Add a new point to the end of the list.  
// Remove the first point in the list if too many points.
void addNewPoint(float x, float y) {
    if (NumPts >= MaxNumPts) {
        removeFirstPoint();
    }
    vPositions_CP[NumPts][0] = x;
    vPositions_CP[NumPts][1] = y;
    printf("CP %i coords (%f, %f) \n", NumPts, vPositions_CP[NumPts][0], vPositions_CP[NumPts][1]);
    NumPts++;
}

void init(void)
{
    // Create a vertex array object for CONTROL POLYGON
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // attribute VBO : positions 
    glGenBuffers(1, &vposition_CP_ID);
    glBindBuffer(GL_ARRAY_BUFFER, vposition_CP_ID);


    // Create a vertex array object  for CURVE
    glGenVertexArrays(1, &vao_2);
    glBindVertexArray(vao_2);
    // attribute VBO : positions 
    glGenBuffers(1, &vposition_Curve_ID);
    glBindBuffer(GL_ARRAY_BUFFER, vposition_Curve_ID);

    // Background color
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glViewport(0, 0, width, height);
}

void initShader(void)
{
    GLenum ErrorCheckValue = glGetError();

    char* vertexShader = (char*)"vertexshaderC.glsl";
    char* fragmentShader = (char*)"fragmentshaderC.glsl";

    programId = ShaderMaker::createProgram(vertexShader, fragmentShader);

    glUseProgram(programId);
}


void findPoint(float xPos, float yPos) {
    float dist;
    for (int i = 0; i < NumPts; i++) {
        // Pitagora
        dist = sqrt(pow(vPositions_CP[i][0] - xPos, 2) + pow(vPositions_CP[i][1] - yPos, 2));
        if (dist < 0.03) {
            mouseOverIndex = i;
            return;
        }
        else mouseOverIndex = - 1;
    }
}

void modifyPoint(float xPos, float yPos) {
    vPositions_CP[mouseOverIndex][0] = xPos;
    vPositions_CP[mouseOverIndex][1] = yPos;
}

void de_casteljau_alghoritm(float t, float* result, int num_points, float points[][2]) {

    float cord_X[MaxNumPts];
    float cord_Y[MaxNumPts];

    for (int i = 0; i < num_points; i++) {
        cord_X[i] = points[i][0];
        cord_Y[i] = points[i][1];
    }
    for (int i = 1; i < num_points; i++) {
        for (int j = 0; j < num_points - i; j++) {
            cord_X[j] = (1 - t) * cord_X[j] + t * cord_X[j + 1];
            cord_Y[j] = (1 - t) * cord_Y[j] + t * cord_Y[j + 1];
        }
    }
    result[0] = cord_X[0];
    result[1] = cord_Y[0];
}


void drawScene_deCasteljau(void)
{
    // Background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    if (NumPts > 1) {
        float result_dc[2];
        int index_cat = 0;

        int steps_per_segment = 200;

        //de casteljau
        for (int j = 0; j <= steps_per_segment; j++) {
            de_casteljau_alghoritm((GLfloat)j / steps_per_segment, result_dc, NumPts, vPositions_CP);

            if (index_cat < MaxNumPts) {
                vPositions_C[index_cat][0] = result_dc[0];
                vPositions_C[index_cat][1] = result_dc[1];
                index_cat++;
            }
        }
        // Draw curve
        glBindVertexArray(vao_2);
        glBindBuffer(GL_ARRAY_BUFFER, vposition_Curve_ID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vPositions_C), &vPositions_C[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Draw the line segments between C
        glLineWidth(2.0);
        glDrawArrays(GL_LINE_STRIP, 0, index_cat);
        glBindVertexArray(0);

    }
    // Draw control polygon
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vposition_CP_ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vPositions_CP), &vPositions_CP[0], GL_STREAM_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Draw the control points CP
    glPointSize(6.0);
    glDrawArrays(GL_POINTS, 0, NumPts);
    // Draw the line segments between CP
    glLineWidth(2.0);
    glDrawArrays(GL_LINE_STRIP, 0, NumPts);
    glBindVertexArray(0);
}


void drawScene_catmullRom(void)
{
    // Background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    if (NumPts > 1) {
        float result_dc[2];
        int index_cat = 0;

        //catmull-rom spline
        for (int i = 0; i < NumPts - 1; i++) {

            float* pi_minus_1;
            float* pi = vPositions_CP[i];
            float* pi_plus_1 = vPositions_CP[i + 1];
            float* pi_plus_2;

            if (i == 0) {
                pi_minus_1 = vPositions_CP[i];
            }
            else
                pi_minus_1 = vPositions_CP[i - 1];

            if (i >= NumPts - 2) {
                pi_plus_2 = vPositions_CP[i + 1];
            }
            else
                pi_plus_2 = vPositions_CP[i + 2];

            float mix = (pi_plus_1[0] - pi_minus_1[0]) / 2.0f;
            float miy = (pi_plus_1[1] - pi_minus_1[1]) / 2.0f;
            float mi_plus_1x = (pi_plus_2[0] - pi[0]) / 2.0f;
            float mi_plus_1y = (pi_plus_2[1] - pi[1]) / 2.0f;


            float temp_bezier[4][2];

            temp_bezier[0][0] = pi[0];
            temp_bezier[0][1] = pi[1];

            temp_bezier[1][0] = pi[0] + mix / 3.0f;
            temp_bezier[1][1] = pi[1] + miy / 3.0f;

            temp_bezier[2][0] = pi_plus_1[0] - mi_plus_1x / 3.0f;
            temp_bezier[2][1] = pi_plus_1[1] - mi_plus_1y / 3.0f;

            temp_bezier[3][0] = pi_plus_1[0];
            temp_bezier[3][1] = pi_plus_1[1];

            int steps_per_segment = 100 / (NumPts - 1);
            if (steps_per_segment == 0) steps_per_segment = 1;

            //de casteljau
            for (int j = 0; j <= steps_per_segment; j++) {
                de_casteljau_alghoritm((GLfloat)j / steps_per_segment, result_dc, 4, temp_bezier);

                if (index_cat < MaxNumPts) {
                    vPositions_C[index_cat][0] = result_dc[0];
                    vPositions_C[index_cat][1] = result_dc[1];
                    index_cat++;
                }
            }
        }

        // Draw curve
        glBindVertexArray(vao_2);
        glBindBuffer(GL_ARRAY_BUFFER, vposition_Curve_ID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vPositions_C), &vPositions_C[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Draw the line segments between C
        glLineWidth(2.0);
        glDrawArrays(GL_LINE_STRIP, 0, index_cat);
        glBindVertexArray(0);

    }


    // Draw control polygon
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vposition_CP_ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vPositions_CP), &vPositions_CP[0], GL_STREAM_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Draw the control points CP
    glPointSize(6.0);
    glDrawArrays(GL_POINTS, 0, NumPts);
    // Draw the line segments between CP
    glLineWidth(0.5);
    glDrawArrays(GL_LINE_STRIP, 0, NumPts);
    glBindVertexArray(0);

}


// Function to initialize OpenGL and create a window.
GLFWwindow* initOpenGL() {

    // Initialize GLFW.
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW." << std::endl;
        glfwTerminate();
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    /******* Create a window and its OpenGL context *****/
    GLFWwindow* window = glfwCreateWindow(width, height, "LAB_01", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window !" << std::endl;
        glfwTerminate();
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to load opengl function pointers !" << std::endl;
        glfwTerminate();
    }
    return window;
}


int main(void)
{
    // Initialize OpenGL and create a window.
    GLFWwindow* window = initOpenGL();

    initShader();
    init();

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Render 
        // Uncomment one of the following drawScene to use de Casteljau or Catmull Rom Splines
        //drawScene_deCasteljau();
        drawScene_catmullRom();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(programId);
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &vao_2);

    glfwTerminate();
    return 0;
}