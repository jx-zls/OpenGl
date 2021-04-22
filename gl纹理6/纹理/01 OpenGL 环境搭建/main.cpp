#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLFrame.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager		shaderManager;
GLMatrixStack		modelViewMatrix;
GLMatrixStack		projectionMatrix;
GLFrame				cameraFrame;
GLFrame             objectFrame;
GLFrustum			viewFrustum;

GLBatch             pyramidBatch;
//GLTriangleBatch      pyramidBatch;


//纹理变量，一般使用无符号整型
GLuint              textureID;

GLGeometryTransform	transformPipeline;
M3DMatrix44f		shadowMatrix;

//绘制金字塔
void MakePyramid(GLBatch& pyramidBatch)
{
    /*1、通过pyramidBatch组建三角形批次
      参数1：类型
      参数2：顶点数
      参数3：这个批次中将会应用1个纹理
      注意：如果不写这个参数，默认为0。
     */
    pyramidBatch.Begin(GL_TRIANGLES, 18, 1);
    
    //金字塔底部
    //底部的四边形 = 三角形X + 三角形Y
    //三角形X
    
    /*设置法线
     Normal3f：添加一个表面法线（法线坐标 与 Vertex顶点坐标中的Y轴一致）
     表面法线是有方向的向量，代表表面或者顶点面对的方向（相反的方向）。在多数的关照模式下是必须使用。后面的课程会详细来讲法线的应用
     */
    pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
    /**设置纹理坐标
     MultiTexCoord2f(GLuint texture,GLclampf s,GLclamp t);
     参数1：texture，纹理层次，对于使用存储着色器来进行渲染，设置为0
     参数2：(s,t,r,q对应顶点坐标的x,y,z,w)s：对应顶点坐标中的x坐标
     参数3：t:对应顶点坐标中的y
     */
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    //vBlackLeft点
    pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
   
    
    pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    //vBlackRight点
    pyramidBatch.Vertex3f(1.0f, -1.0f, -1.0f);
    
    pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    //vFrontRight点
    pyramidBatch.Vertex3f(1.0f, -1.0f, 1.0f);
    
    
    //三角形B
    pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
    pyramidBatch.Vertex3f(-1.0f, -1.0f, 1.0f);
    
    pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3f(-1.0f, -1.0f, -1.0f);
    
    pyramidBatch.Normal3f(0.0f, -1.0f, 0.0f);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3f(1.0f, -1.0f, 1.0f);
    
    //塔顶
    M3DVector3f vApex = { 0.0f, 1.0f, 0.0f };
    M3DVector3f vFrontLeft = { -1.0f, -1.0f, 1.0f };
    M3DVector3f vFrontRight = { 1.0f, -1.0f, 1.0f };
    M3DVector3f vBackLeft = { -1.0f, -1.0f, -1.0f };
    M3DVector3f vBackRight = { 1.0f, -1.0f, -1.0f };
    M3DVector3f n;
    
    
    // 金字塔前面
    //三角形：（Apex，vFrontLeft，vFrontRight）
    //纹理坐标设置，参考PPT图6-4图
    /** 获取从三点找到一个法线坐标(三点确定一个面)
     void m3dFindNormal(result,point1, point2,point3);
     参数1：结果
     参数2-4：3个顶点数据
     */
    m3dFindNormal(n, vApex, vFrontLeft, vFrontRight);
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);

    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    //金字塔左边
    //三角形：（vApex, vBackLeft, vFrontLeft）
    m3dFindNormal(n, vApex, vBackLeft, vFrontLeft);
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    //金字塔右边
    //三角形：（vApex, vFrontRight, vBackRight）
    m3dFindNormal(n, vApex, vFrontRight, vBackRight);
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    //金字塔后边
    //三角形：（vApex, vBackRight, vBackLeft）
    m3dFindNormal(n, vApex, vBackRight, vBackLeft);
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    //结束批次设置
    pyramidBatch.End();
}


// 将TGA文件加载为2D纹理。
bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
    GLbyte *bytes;
    int width, height, component;
    GLenum format;
    
    bytes = gltReadTGABits(szFileName, &width, &height, &component, &format);
    if(bytes == NULL) return false;
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTexImage2D(GL_TEXTURE_2D, 0, component, width, height, 0, format, GL_UNSIGNED_BYTE, bytes);
    
    free(bytes);
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}



void SetupRC()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    shaderManager.InitializeStockShaders();
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    LoadTGATexture("stone.tga", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_CLAMP_TO_EDGE);
    MakePyramid(pyramidBatch);
    cameraFrame.MoveForward(-15.0f);
   
    
}



//void setUpRC(){
//    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//    shaderManager.InitializeStockShaders();
//    cameraFrame.MoveForward(-15);
//
//    glGenTextures(1, &textureID);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//
//    GLbyte *cBytes;
//    int cwidth,cheight,ccomponent;
//    GLenum cformat;
//
//    cBytes = gltReadTGABits("stone.tga", &cwidth, &cheight, &ccomponent, &cformat);
//    if(cBytes == NULL) return;
//
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//    glTexImage2D(GL_TEXTURE_2D, 0, ccomponent, cwidth, cheight, 0, cformat, GL_UNSIGNED_BYTE, cBytes);
//
//    free(cBytes);
//
//
////    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), lightPos, white,0);
//}
//
//

// 清理…例如删除纹理对象
void ShutdownRC(void)
{
    glDeleteTextures(1, &textureID);

}



void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLfloat vRed[] = {1.0f, 1.0f,1.0f,1.0f};
    GLfloat vLightPos [] = { 1.0f, 1.0f, 0.0f };
//    GLfloat vBlue[] = {0.0f, 1.0f,0.0f,1.0f};
    modelViewMatrix.PushMatrix();
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.MultMatrix(mCamera);
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    modelViewMatrix.MultMatrix(mObjectFrame);
    
//    glBindTexture(GL_TEXTURE_2D, textureID);

    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(),vLightPos,vRed, 0);
    
//    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vRed);
    pyramidBatch.Draw();
    modelViewMatrix.PopMatrix();

    glutSwapBuffers();
    
}



void SpecialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    
    glutPostRedisplay();
}


void ChangeSize(int w, int h)
{
    glViewport(0, 0, w, h);
    viewFrustum.SetPerspective(30.0f, float(w)/float(h), 1.0f, 1000.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    modelViewMatrix.LoadIdentity();
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}


int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Pyramid");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    
    SetupRC();
    
    glutMainLoop();
    
    ShutdownRC();
    
    return 0;
}
