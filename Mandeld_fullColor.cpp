#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#define _OPENGL_4_1_
#if !defined (_OPENGL_4_1_)
    #define GLSL_VERSION "#version 450\n"
#else
    #define GLSL_VERSION "#version 410\n"
#endif

#define VERTEX_CODE GLSL_VERSION\
"layout (location = 2) in vec2 vPos;       \n"\
"                                          \n"\
"#ifdef GL_ES                              \n"\
"#else                                     \n"\
"out gl_PerVertex                          \n"\
"{                                         \n"\
"   vec4 gl_Position;                      \n"\
"};                                        \n"\
"#endif                                    \n"\
"                                          \n"\
"void main(void)                           \n"\
"{                                         \n"\
"    gl_Position = vec4(vPos.xy,.0f,1.f);  \n"\
"                                          \n"\
"}                                         \n"

#define FRAGMENT_CODE GLSL_VERSION\
"#ifdef GL_ES                                                             \n"\
"precision highp float;                                                   \n"\
"#endif                                                                   \n"\
"uniform vec2 wSize;                                                      \n"\
"                                                                         \n"\
"uniform dvec2 mScale;                                                    \n"\
"uniform dvec2 mTransp;                                                   \n"\
"                                                                         \n"\
"out vec4 color;                                                          \n"\
"                                                                         \n"\
"const int iterations = 512;                                              \n"\
"                                                                         \n"\
"#define M_PI 3.1415926535897932384626433832795                                         \n"\
"                                                                                       \n"\
"const float sqrBailoutRadius = 1.0E30; // (square of the bail-out radius)              \n"\
"                                                                                       \n"\
"                                                                                       \n"\
"float sh(float x)                                                                      \n"\
"{                                                                                      \n"\
"    return (exp(x)-exp(-x))/2.0;                                                       \n"\
"}                                                                                      \n"\
"float ch(float x)                                                                      \n"\
"{                                                                                      \n"\
"    return (exp(x)+exp(-x))/2.0;                                                       \n"\
"}                                                                                      \n"\
"                                                                                       \n"\
"vec3 colorFunc(float iter, float zx)                                                   \n"\
"{                                                                                      \n"\
"vec3 abcVec = vec3(105.0, 125.0, 5.0);	                                                \n"\
"float rmaj = 122.0;                                                                     \n"\
"float rmin = 317.0;                                                                     \n"\
"float g = 33.0; //+180.; invert r <-> b ... whit q                                     \n"\
"float h = -33.0;                                                                       \n"\
"float q = 28.79;//+270.;                                                               \n"\
"float uK = log(log(sqrBailoutRadius));                                                 \n"\
"float Iter = 256.0;                                                                    \n"\
"	                                                                                    \n"\
"float pi=M_PI;                                                                         \n"\
"	                                                                                    \n"\
"float vK = 1.0/log(2.0); // (2 is the degree of the rational function)                   \n"\
"float dens = 25.0;  // (density of the colours)                                          \n"\
"float disp = 315.0; // (displacement of the colour scale)                              \n"\
"                                                                                       \n"\
"vec3 uvwVec = vec3(cos(g * pi / 180.) * cos(h * pi / 180.),                            \n"\
"		           cos(g * pi / 180.) * sin(h * pi / 180.),                             \n"\
"		           sin(g * pi / 180.));                                                 \n"\
"	                                                                                    \n"\
"vec3 xyzVec = vec3(-abcVec.xy, (abcVec.x * uvwVec.x + abcVec.y * uvwVec.y) / uvwVec.z);\n"\
"	                                                                                    \n"\
"vec3 sqrV = xyzVec*xyzVec;                                                             \n"\
"	                                                                                    \n"\
"vec3 xyz1Vec = xyzVec/sqrt(sqrV.x + sqrV.y + sqrV.z);	                                \n"\
"	                                                                                    \n"\
"vec3 xyz2Vec = vec3(uvwVec.y*xyz1Vec.z - uvwVec.z*xyz1Vec.y,                           \n"\
"		            uvwVec.z*xyz1Vec.x - uvwVec.x*xyz1Vec.z,                            \n"\
"		            uvwVec.x*xyz1Vec.y - uvwVec.y*xyz1Vec.x);                           \n"\
"                                                                                       \n"\
"    xyz1Vec = xyz1Vec * cos(q * pi / 180.) + xyz2Vec * sin(q * pi / 180.);             \n"\
"                                                                                       \n"\
"    xyz2Vec = vec3(uvwVec.y*xyz1Vec.z - uvwVec.z*xyz1Vec.y,                            \n"\
"                   uvwVec.z*xyz1Vec.x - uvwVec.x*xyz1Vec.z,                            \n"\
"		           uvwVec.x*xyz1Vec.y - uvwVec.y*xyz1Vec.x);                            \n"\
"                                                                                       \n"\
"    //float s = (720./float(iterations)) * iter - vK * (log(log(zx)) + uK);            \n"\
"	float s = iter - vK * (log(log(zx)) + uK);                                          \n"\
"    float n = dens * s + disp; // mod 720                                              \n"\
"                                                                                       \n"\
"    float e = rmaj * cos(n * pi / 360.);                                               \n"\
"    float f = rmin * sin(n * pi / 360.);                                               \n"\
"                                                                                       \n"\
"    vec3 col = abcVec + e*xyz1Vec + f*xyz2Vec;                                         \n"\
"                                                                                       \n"\
"    return col/float(Iter);                                                            \n"\
"}                                                                                      \n"\
"                                                                         \n"\
"void main ()                                                             \n"\
"{                                                                        \n"\
"    dvec2 c = (mTransp - mScale) + gl_FragCoord.xy/wSize * (mScale*2.0); \n"\
"    dvec2 z = vec2(0.0);                                                 \n"\
"    float clr=0.0;                                                       \n"\
"                                                                         \n"\
"    for(int i=0; i < iterations ; i++) {                                 \n"\
"        z = dvec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;                   \n"\
"        if (dot(z, z) > 64.0) { clr=float(i); break; }                   \n"\
"    }	                                                                  \n"\
"   vec2 fZ = vec2(z);                                                          \n"\
"   color = vec4(colorFunc(clr, dot(fZ,fZ)),1.0);                         \n"\
"}                                                                        \n"\

// ogl vertex buffers
float vtx[] = {-1.0f,-1.0f,
                1.0f,-1.0f,
                1.0f, 1.0f,
                -1.0f, 1.0f };
GLuint vao, vbo;

// Shaders attributes
GLuint locMScale, locMTransp, locWSize;
GLuint program;

int width = 1024, height = 1024;
double mScaleX = 1.5, mScaleY = 1.5, mTranspX = -.75, mTranspY = 0.0;
const float zoomFactor = .025;

GLFWwindow *glfwWindow;

const float black[] = { 0.0, 0.0, 0.0, 1.0 };
void drawMandel()
{
    glClearBufferfv(GL_COLOR, 0, black);

    glUseProgram(program);

    // pass uniform variables
    glUniform2d(locMScale, mScaleX, mScaleY); 
    glUniform2d(locMTransp, mTranspX, mTranspY); 
    glUniform2f(locWSize, width, height); 

    // Draw
    glBindVertexArray(vao);        
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glUseProgram(0);

    glfwSwapBuffers(glfwWindow);
}

void buildBuffers() 
{
enum Attrib_IDs { vPosition = 2 };

#if !defined (_OPENGL_4_1_) // altrimenti OpenGL 4.5
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vbo);
        glNamedBufferStorage(vbo, sizeof(vtx), vtx, 0); 

        glVertexArrayAttribBinding(vao,vPosition, 0);
        glVertexArrayAttribFormat(vao, vPosition, 2, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(vao, vPosition);        

        glVertexArrayVertexBuffer(vao, 0, vbo, 0, 8);

#else                      // GL 4.1 / GL_ES 3.0 / WGL2
        glGenVertexArrays(1, &vao); 
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER,vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vtx), vtx, GL_STATIC_READ);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0L);
        glEnableVertexAttribArray(vPosition);
#endif
}


void initGL()
{

    const char *vCode = VERTEX_CODE;
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, NULL);
    glCompileShader(vertex);

    const char *fCode = FRAGMENT_CODE;
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, NULL);
    glCompileShader(fragment);

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    locMScale  = glGetUniformLocation(program,"mScale");
    locMTransp = glGetUniformLocation(program,"mTransp");
    locWSize   = glGetUniformLocation(program,"wSize");

    buildBuffers();

    glViewport(0, 0, width, height);
    drawMandel();
    
}

void glfwWindowSizeCallback(GLFWwindow* window, int w, int h) { 
    mScaleX+=(w-width) * mScaleX/(double)width ;   mScaleY+=(h-height) * mScaleY/(double)height ;
    width = w; height = h; 
    glViewport(0, 0, width, height);

    drawMandel();
}

void doZoom(float scale)
{
    double x,y;
    glfwGetCursorPos(glfwWindow, &x, &y);

    mScaleX *= 1.0+scale;
    mScaleY *= 1.0+scale;

    const double ptX =  double((width >>1) - x)/double(width >>1) * scale;
    const double ptY = -double((height>>1) - y)/double(height>>1) * scale;

    mTranspX += ptX * mScaleX;    
    mTranspY += ptY * mScaleY;

    drawMandel();
}

void initGLFW()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#if !defined (_OPENGL_4_1_) 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

    glfwWindow = glfwCreateWindow(width, height, "glMandel", NULL, NULL);
    if (!glfwWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(glfwWindow);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);  //get OpenGL extensions

    glfwSetWindowSizeCallback(glfwWindow, glfwWindowSizeCallback);

    glfwSwapInterval(0); // 0 vSync off - 1 vSync on
}

int main()
{
    initGLFW();

    initGL();

    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();

        if(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT)  == GLFW_PRESS) 
            doZoom(-zoomFactor);        
        else if(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT)  == GLFW_PRESS)  
            doZoom(zoomFactor);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}