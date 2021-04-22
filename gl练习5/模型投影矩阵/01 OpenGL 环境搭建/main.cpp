
#include "GLTools.h"	// OpenGL toolkit
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


GLShaderManager		shaderManager;
//模型视图矩阵堆栈
GLMatrixStack		modelViewMatrix;

//投影视图矩阵堆栈
GLMatrixStack		projectionMatrix;

//观察者位置
GLFrame				cameraFrame;

//世界坐标位置
GLFrame             objectFrame;

//视景体，用来构造投影矩阵
GLFrustum			viewFrustum;

//三角形批次类
GLTriangleBatch     CC_Triangle;

//球
GLTriangleBatch     sphereBatch;
//环
GLTriangleBatch     torusBatch;
//圆柱
GLTriangleBatch     cylinderBatch;
//锥
GLTriangleBatch     coneBatch;
//磁盘
GLTriangleBatch     diskBatch;

//管道，用来管理投影视图矩阵堆栈和模型视图矩阵堆栈的
GLGeometryTransform	transformPipeline;

//颜色值，绿色、黑色
GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

//空格的标记
int nStep = 0;

// 将上下文中，进行必要的初始化
void SetupRC()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    shaderManager.InitializeStockShaders();

//    cameraFrame.MoveForward(-7.0f);
//    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    gltMakeSphere(sphereBatch, 0.4f, 10, 20);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}


void DrawWireFramedBatch(GLTriangleBatch* pBatch)
{

}


//召唤场景
void RenderScene(void)
{
    static CStopWatch rotTimer;
    
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;
    GLfloat vblack[] = {0.0f, 0.0f, 0.0f,1.0f};
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    M3DMatrix44f modelViewMatrix, translationMat, rotationMat, mModelViewProjection;
    m3dTranslationMatrix44(translationMat,0.0f, 0.0f, -2.5f);
    m3dRotationMatrix44(rotationMat, m3dDegToRad(yRot), 0.0f, 1.0f, 1.0f);
    m3dMatrixMultiply44(modelViewMatrix, translationMat, rotationMat);

    m3dMatrixMultiply44(mModelViewProjection, viewFrustum.GetProjectionMatrix(), modelViewMatrix);

    shaderManager.UseStockShader(GLT_SHADER_FLAT, mModelViewProjection, vBlack);
    
    
//    modelViewMatrix.PushMatrix();
//    M3DMatrix44f mCamera;
//    cameraFrame.GetCameraMatrix(mCamera);

//    modelViewMatrix.MultMatrix(mCamera);

//    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix() , vblack);
    
    sphereBatch.Draw();
    glutSwapBuffers();
    glutPostRedisplay();
    
}


void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    viewFrustum.SetPerspective(30.0f, float(w)/float(h), 1.0f, 500.0f);
//    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
//    modelViewMatrix.LoadIdentity();
    
}


//上下左右，移动图形
//void SpecialKeys(int key, int x, int y)
//{
//    if(key == GLUT_KEY_UP)
//        //移动世界坐标系，而不是去移动物体。
//        //将世界坐标系在X方向移动-5.0
//        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
//
//    if(key == GLUT_KEY_DOWN)
//        objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
//
//    if(key == GLUT_KEY_LEFT)
//        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
//
//    if(key == GLUT_KEY_RIGHT)
//        objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
//
//    glutPostRedisplay();
//}


//点击空格，切换渲染图形
//void KeyPressFunc(unsigned char key, int x, int y)
//{
//    if(key == 32)
//    {
//        nStep++;
//
//        if(nStep > 4)
//            nStep = 0;
//    }
//
//    switch(nStep)
//    {
//        case 0:
//            glutSetWindowTitle("Sphere");
//            break;
//        case 1:
//            glutSetWindowTitle("Torus");
//            break;
//        case 2:
//            glutSetWindowTitle("Cylinder");
//            break;
//        case 3:
//            glutSetWindowTitle("Cone");
//            break;
//        case 4:
//            glutSetWindowTitle("Disk");
//            break;
//    }
//
//    glutPostRedisplay();
//}


int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Sphere");
    glutReshapeFunc(ChangeSize);
//    glutKeyboardFunc(KeyPressFunc);
//    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    
    SetupRC();
    
    glutMainLoop();
    return 0;
}
