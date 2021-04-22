#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];


GLShaderManager		shaderManager;			// 着色器管理器
GLMatrixStack		modelViewMatrix;		// 模型视图矩阵
GLMatrixStack		projectionMatrix;		// 投影矩阵
GLFrustum			viewFrustum;			// 视景体
GLGeometryTransform	transformPipeline;		// 几何图形变换管道

GLTriangleBatch		torusBatch;             // 花托批处理
GLBatch				floorBatch;             // 地板批处理

//**定义公转球的批处理（公转自转）**
GLTriangleBatch     sphereBatch;            //球批处理

//角色帧 照相机角色帧
GLFrame             cameraFrame;


void SetupRC()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    shaderManager.InitializeStockShaders();
    cameraFrame.MoveForward(-10);
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    
    
    floorBatch.Begin(GL_LINES, 324);
    for(GLfloat x = -20.0; x <= 20.0f; x+= 0.5) {
        floorBatch.Vertex3f(x, -0.55f, 20.0f);
        floorBatch.Vertex3f(x, -0.55f, -20.0f);
        
        floorBatch.Vertex3f(20.0f, -0.55f, x);
        floorBatch.Vertex3f(-20.0f, -0.55f, x);
    }
    floorBatch.End();
    
    gltMakeTorus(torusBatch, 0.4f, 0.13f, 30, 30);
    gltMakeSphere(sphereBatch, 0.1f, 26, 13);
    for (int i = 0; i < NUM_SPHERES; i++) {
        
        //y轴不变，X,Z产生随机值
        GLfloat x = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        GLfloat z = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        
        //在y方向，将球体设置为0.0的位置，这使得它们看起来是飘浮在眼睛的高度
        //对spheres数组中的每一个顶点，设置顶点数据
        spheres[i].SetOrigin(x, 0.0f, z);
    }
    
    
}


// 屏幕更改大小或已初始化
void ChangeSize(int nWidth, int nHeight)
{
    glViewport(0, 0, nWidth, nHeight);
    viewFrustum.SetPerspective(30.0f, float(nWidth)/float(nHeight), 1.0f, 1000.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    modelViewMatrix.LoadIdentity();
    
}


void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLfloat vRed[] = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat vGreen[] = {0.0f, 1.0f, 1.0f, 1.0f};
    GLfloat vBlue[] = {0.0f, 0.0f, 1.0f, 1.0f};
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.PushMatrix(mCamera);
    
    static CStopWatch rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;
    shaderManager.UseStockShader(GLT_SHADER_FLAT, viewFrustum.GetProjectionMatrix(), vGreen);
    floorBatch.Draw();
    
    M3DVector4f vLightPos = {0.0f, 10.0f, 5.0f, 1.0f};
    M3DVector4f vLightEysMat;
    m3dTransformVector4(vLightEysMat, vLightPos, mCamera);
    
    for (int i = 0; i < NUM_SPHERES; i++) {
        modelViewMatrix.PushMatrix();
        modelViewMatrix.MultMatrix(spheres[i]);
        shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vBlue);
        shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(),vLightEysMat,vBlue);
        sphereBatch.Draw();
        modelViewMatrix.PopMatrix();
    }
    
    modelViewMatrix.Translate(0.0f, 0.0f, -15.f);
    modelViewMatrix.PushMatrix();
    modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vLightEysMat, vRed);
    torusBatch.Draw();
    modelViewMatrix.PopMatrix();
    
    
    modelViewMatrix.Rotate(yRot * -2.0f, 0.0f, 1.0f, 0.0f);
    modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vBlue);
    sphereBatch.Draw();
    
    modelViewMatrix.PopMatrix();
    
    
    
    
    
    glutSwapBuffers();
    glutPostRedisplay();
    

    
}

//进行调用以绘制场景
//void RenderScene(void)
//{
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    GLfloat vGreen[] = {0.0f, 1.0f, 0.0f, 1.0f};
//    GLfloat vRed[] = {1.0f, 0.0f, 0.0f, 1.0f};
//    GLfloat vBlue[] = {0.0f, 0.0f, 1.0f, 1.0f};
//
//    static CStopWatch    rotTimer;
//    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;
//
//    M3DMatrix44f mCamera;
//    cameraFrame.GetCameraMatrix(mCamera);
//    modelViewMatrix.PushMatrix(mCamera);
//
//    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vGreen);
//    floorBatch.Draw();
//
////    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
////    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vRed);
////    torusBatch.Draw();
//
//
//    M3DVector4f vLightPos = {0.0f,10.0f,5.0f,1.0f};
//    M3DVector4f vLightEyePos;
//    //将照相机矩阵mCamera 与 光源矩阵vLightPos 相乘获得vLightEyePos 矩阵
//
//    m3dTransformVector4(vLightEyePos, vLightPos, mCamera);
//
//
//    for (int i = 0; i < NUM_SPHERES; i++) {
//        modelViewMatrix.PushMatrix();
//        modelViewMatrix.MultMatrix(spheres[i]);
//
////        shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vBlue);
//
//        shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(),
//                                     transformPipeline.GetProjectionMatrix(), vLightEyePos, vBlue);
//
//        sphereBatch.Draw();
//
//        modelViewMatrix.PopMatrix();
//
//    }
//
//    //modelViewMatrix 顶部矩阵沿着z轴移动2.5单位
//    modelViewMatrix.Translate(0.0f, 0.0f, -2.5f);
//
//    //**保存平移（公转自转）**
//    modelViewMatrix.PushMatrix();
//
//    //modelViewMatrix 顶部矩阵旋转yRot度
//    modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
//
////    使用平面着色器 变换管道中的投影矩阵 和 变换矩阵 相乘的矩阵，指定甜甜圈颜色
//    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(),vRed);
////    **4、绘制光源，修改着色器管理器
//    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(),
//                                 transformPipeline.GetProjectionMatrix(), vLightEyePos, vRed);
//    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vRed);
////    开始绘制
//    torusBatch.Draw();
//
////     恢复modelViewMatrix矩阵，移除矩阵堆栈
////    使用PopMatrix推出刚刚变换的矩阵，然后恢复到单位矩阵
//    modelViewMatrix.PopMatrix();
//
//    modelViewMatrix.Rotate(yRot * -2.0f, 0.0f, 1.0f, 0.0f);
//    modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
//    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vBlue);
//    sphereBatch.Draw();
//
//    modelViewMatrix.PopMatrix();
//    glutSwapBuffers();
//    glutPostRedisplay();
//
//
//}


int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    
    glutCreateWindow("OpenGL SphereWorld");
    
    glutReshapeFunc(ChangeSize);
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
