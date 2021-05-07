//
//  ViewController.m
//  LightAndNormal
//
//  Created by Tocce on 2021/4/27.
//

#import "ViewController.h"
#import "AGLKVertexAttribArrayBuffer.h"
#import "sceneUtil.h"

@interface ViewController ()

@property(nonatomic, strong) EAGLContext *context;
@property(nonatomic, strong) GLKBaseEffect *baseEffect;
@property(nonatomic, strong) GLKBaseEffect *extraEffect;
@property(nonatomic, strong) AGLKVertexAttribArrayBuffer *vertexBuffer;
@property(nonatomic, strong) AGLKVertexAttribArrayBuffer *normalBuffer;

@property(nonatomic, assign) BOOL shouleFaceNormals;

@property(nonatomic, assign) BOOL shouldDrawNormals;

@property(nonatomic, assign) GLfloat centexVertexHeight;
@property(nonatomic, assign) BOOL isOpen;

@end

@implementation ViewController
{
    //三角形-8面
    SceneTriangle triangles[NUM_FACES];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setup];
    
}

- (void)setup {
    
    self.context = [[EAGLContext alloc] initWithAPI:(kEAGLRenderingAPIOpenGLES2)];
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    [EAGLContext setCurrentContext:self.context];
    
    self.baseEffect = [[GLKBaseEffect alloc] init];
    self.baseEffect.light0.enabled = true;
    self.baseEffect.light0.diffuseColor = GLKVector4Make(0.7, 0.7, 0.7, 1.0f);
    self.baseEffect.light0.position = GLKVector4Make(1.0f, 1.0f, 0.5f, 0.0f);
    
    
    
    self.extraEffect = [[GLKBaseEffect alloc] init];
    self.extraEffect.useConstantColor = true;
    
    
 
    
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    
    
    triangles[0] = SceneTriangleMake(vertexA, vertexB, vertexD);
    triangles[1] = SceneTriangleMake(vertexB, vertexC, vertexF);
    triangles[2] = SceneTriangleMake(vertexD, vertexB, vertexE);
    triangles[3] = SceneTriangleMake(vertexE, vertexB, vertexF);
    triangles[4] = SceneTriangleMake(vertexD, vertexE, vertexH);
    triangles[5] = SceneTriangleMake(vertexE, vertexF, vertexH);
    triangles[6] = SceneTriangleMake(vertexG, vertexD, vertexH);
    triangles[7] = SceneTriangleMake(vertexH, vertexF, vertexI);
    
    self.vertexBuffer = [[AGLKVertexAttribArrayBuffer alloc] initWithAttribStride:sizeof(SceneVertex) numberOfVertices:sizeof(triangles)/sizeof(SceneVertex) bytes:triangles usage:GL_DYNAMIC_DRAW];
    
    self.normalBuffer = [[AGLKVertexAttribArrayBuffer alloc]initWithAttribStride:sizeof(SceneVertex) numberOfVertices:0 bytes:NULL usage:GL_DYNAMIC_DRAW];


    self.centexVertexHeight = 0.0f;
    self.shouleFaceNormals = true;
    
    
}


-(void) glkView:(GLKView *)view drawInRect:(CGRect)rect {
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (self.isOpen) {
        
        GLKMatrix4 modelViewMatrix = GLKMatrix4MakeRotation(GLKMathDegreesToRadians(-60.0f), 1.0f, 0.0f, 0.0f);
        
        modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix,GLKMathDegreesToRadians(-30.0f), 0.0f, 0.0f, 1.0f);
        
        modelViewMatrix = GLKMatrix4Translate(modelViewMatrix, 0.0f, 0.0f, 0.25f);
        
        //设置baseEffect,extraEffect 模型矩阵
        self.baseEffect.transform.modelviewMatrix = modelViewMatrix;
        self.extraEffect.transform.modelviewMatrix = modelViewMatrix;
        
    }
    
    [self.vertexBuffer prepareToDrawWithAttrib:GLKVertexAttribPosition numberOfCoordinates:3 attribOffset:offsetof(SceneVertex, position) shouldEnable:true];
    [self.baseEffect prepareToDraw];
    
    [self.vertexBuffer prepareToDrawWithAttrib:GLKVertexAttribNormal numberOfCoordinates:3 attribOffset:offsetof(SceneVertex, normal) shouldEnable:YES];
    
    [self.vertexBuffer drawArrayWithMode:GL_TRIANGLES startVertexIndex:0 numberOfVertices:sizeof(triangles)/sizeof(SceneVertex)];
    
    if(self.shouleFaceNormals){
        [self drawNormals];
        
    }
}

- (void)drawNormals {
    
    GLKVector3 normalLineVertices[NUM_LINE_VERTS];
    SceneTrianglesNormalLinesUpdate(triangles, GLKVector3MakeWithArray(self.baseEffect.light0.position.v), normalLineVertices);
    [self.normalBuffer reinitWithAttribStride:sizeof(GLKVector3) numberOfVertices:NUM_LINE_VERTS bytes:normalLineVertices];
    [self.normalBuffer prepareToDrawWithAttrib:GLKVertexAttribPosition numberOfCoordinates:3 attribOffset:0 shouldEnable:YES];

    self.extraEffect.useConstantColor = GL_TRUE;
    //设置光源颜色为绿色，画顶点法线
    self.extraEffect.constantColor = GLKVector4Make(0.0f, 1.0f, 0.0f, 1.0f);
    [self.extraEffect prepareToDraw];
    [self.normalBuffer drawArrayWithMode:GL_LINES startVertexIndex:0 numberOfVertices:NUM_NORMAL_LINE_VERTS];

    self.extraEffect.constantColor = GLKVector4Make(1.0f, 1.0f, 0.0f, 1.0f);
    
    
    //准备绘制-黄色的光源方向线
    [self.extraEffect prepareToDraw];
    //(NUM_LINE_VERTS - NUM_NORMAL_LINE_VERTS) = 2 .2点确定一条线
    [self.normalBuffer drawArrayWithMode:GL_LINES startVertexIndex:NUM_NORMAL_LINE_VERTS numberOfVertices:(NUM_LINE_VERTS - NUM_NORMAL_LINE_VERTS)];
    
}

-(void)updateNormals
{
    if (self.shouleFaceNormals) {
        //更新每个点的平面法向量
        SceneTrianglesUpdateFaceNormals(triangles);
    }else
    {
        //通过平均值求出每个点的法向量
        SceneTrianglesUpdateVertexNormals(triangles);
    }
    [self.vertexBuffer reinitWithAttribStride:sizeof(SceneVertex) numberOfVertices:sizeof(triangles)/sizeof(SceneVertex) bytes:triangles];
    
}

- (void)setCentexVertexHeight:(GLfloat)centexVertexHeight {
    
    _centexVertexHeight = centexVertexHeight;
    
    SceneVertex newVertexE = vertexE;
    newVertexE.position.z = _centexVertexHeight;
    triangles[2] = SceneTriangleMake(vertexD, vertexB, newVertexE);
    triangles[3] = SceneTriangleMake(newVertexE, vertexB, vertexF);
    triangles[4] = SceneTriangleMake(vertexD, newVertexE, vertexH);
    triangles[5] = SceneTriangleMake(newVertexE, vertexF, vertexH);
    
    //更新法线
    [self updateNormals];
}

- (void)setShouleFaceNormals:(BOOL)shouleFaceNormals {
    if (shouleFaceNormals != _shouleFaceNormals) {
        
        _shouleFaceNormals = shouleFaceNormals;
        
        [self updateNormals];
    }

}

- (IBAction)drawNormals:(UISwitch *)sender {
    self.shouldDrawNormals = sender.isOn;
}

- (IBAction)drawFaceNormal:(UISwitch *)sender {
    self.shouleFaceNormals = sender.isOn;
}
- (IBAction)vertexHeight:(UISlider *)sender {
    self.centexVertexHeight = sender.value;
}

- (IBAction)openRotation:(UISwitch *)sender {
    self.isOpen = sender.isOn;
}

@end
