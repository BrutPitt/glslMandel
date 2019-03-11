////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018-2019 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  https://BrutPitt.com
//
//  This software is distributed under the terms of the BSD 2-Clause license:
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//   
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////
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
"    vec4 gl_Position;                     \n"\
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
"uniform vec2 mScale;                                                     \n"\
"uniform vec2 mTransp;                                                    \n"\
"                                                                         \n"\
"out vec4 color;                                                          \n"\
"                                                                         \n"\
"const int iterations = 256;                                              \n"\
"                                                                         \n"\
"/////////////////////////////////////////////////                        \n"\
"vec3 hsl2rgb(vec3 hsl)                                                   \n"\
"{                                                                        \n"\
"    float H = fract(hsl.x);                                              \n"\
"    vec3 rgb = clamp(vec3(      abs(H * 6.0 - 3.0) - 1.0,                \n"\
"                      2.0 - abs(H * 6.0 - 2.0),                          \n"\
"                      2.0 - abs(H * 6.0 - 4.0)                           \n"\
"                      ), 0.0, 1.0);                                      \n"\
"    float C = (1.0 - abs(2.0 * hsl.z - 1.0)) * hsl.y;                    \n"\
"    return (rgb - 0.5) * C + hsl.z;                                      \n"\
"}                                                                        \n"\
"                                                                         \n"\
"                                                                         \n"\
"void main ()                                                             \n"\
"{                                                                        \n"\
"    vec2 c = (mTransp - mScale) + gl_FragCoord.xy/wSize * (mScale*2.0);  \n"\
"    vec2 z = vec2(0.0);                                                  \n"\
"    float clr=0.0;                                                       \n"\
"                                                                         \n"\
"    for(int i=0; i < iterations ; i++) {                                 \n"\
"        z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;                    \n"\
"        if (dot(z, z) > 4.0) { clr=float(i)/float(iterations); break; }  \n"\
"    }	                                                                  \n"\
"   vec3 tone = hsl2rgb(vec3(clr,1.0, 0.5));                              \n"\
"   color = clr>0.0 ? vec4(tone,1.0) : vec4(0.0);                         \n"\
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
float mScaleX = 1.5, mScaleY = 1.5, mTranspX = -.75, mTranspY = 0.0;
const float zoomFactor = .01;

GLFWwindow *glfwWindow;

const float black[] = { 0.0, 0.0, 0.0, 1.0 };
void drawMandel()
{
    glClearBufferfv(GL_COLOR, 0, black);

    glUseProgram(program);

    // pass uniform variables
    glUniform2f(locMScale, mScaleX, mScaleY); 
    glUniform2f(locMTransp, mTranspX, mTranspY); 
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
    mScaleX+=(w-width) * mScaleX/(float)width ;   mScaleY+=(h-height) * mScaleY/(float)height ;
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

    const float ptX =  float((width >>1) - x)/float(width >>1) * scale;
    const float ptY = -float((height>>1) - y)/float(height>>1) * scale;

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