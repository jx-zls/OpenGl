//
//  ViewController.m
//  Sky
//
//  Created by Tocce on 2021/5/7.
//

#import "ViewController.h"
#import "starship.h"
#import "SkyEffect.h"
#import <OpenGLES/ES2/glext.h>

@interface ViewController ()

@property(nonatomic, strong) EAGLContext *cContext;

@property(nonatomic, strong) SkyEffect *skyEffect;
//@property(nonatomic, strong) CCSkyBoxEffect *skyEffect;


@property(nonatomic, strong) GLKBaseEffect *baseEffect;

@property(nonatomic, assign) GLKVector3 eyePosition;
@property(nonatomic, assign) GLKVector3 lookPosition;
@property(nonatomic, assign) GLKVector3 upPosition;

@property(nonatomic, assign) float angle;


@property (assign, nonatomic) GLuint cPositionBuffer;
@property (assign, nonatomic) GLuint cNormalBuffer;

@property(nonatomic, strong) UISwitch *cPauseSwitch;

@end

@implementation ViewController


- (void)bindBuffer {
    GLuint buffer;
//    glGenVertexArraysOES(1, &_cPositionBuffer);
//    glBindVertexArrayOES(_cPositionBuffer);
    
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(starshipPositions), starshipPositions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*3, 0);
    
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(starshipNormals), starshipNormals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(GLKVertexAttribNormal);
    glVertexAttribPointer(GLKVertexAttribNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.cContext = [[EAGLContext alloc] initWithAPI:(kEAGLRenderingAPIOpenGLES2)];
    
    GLKView *view = (GLKView *)self.view;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.context = self.cContext;
    
    [EAGLContext setCurrentContext:self.cContext];
    
    self.eyePosition = GLKVector3Make(0.0f, 10.0f, 10.0f);
    self.lookPosition = GLKVector3Make(0.0f, 0.0f, 0.0f);
    self.upPosition = GLKVector3Make(0.0f, 1.0f, 0.0f);
    
    self.baseEffect = [[GLKBaseEffect alloc] init];
    self.baseEffect.light0.enabled = GL_TRUE;
    self.baseEffect.light0.diffuseColor = GLKVector4Make(0.75f, 0.75f, 0.75f, 1.0f);
    self.baseEffect.light0.position = GLKVector4Make(0.0f, 0.0f, 2.0f, 1.0f);
    self.baseEffect.light0.specularColor = GLKVector4Make(0.25f, 0.25f, 0.25f, 1.0f);
    self.baseEffect.lightingType = GLKLightingTypePerPixel;
    self.angle = 0.5;
    
    [self setMatrix];
    
    [self bindBuffer];
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    NSString *path = [[NSBundle bundleForClass:[self class]]
                      pathForResource:@"skybox3" ofType:@"png"];
   
    NSError *error = nil;
    //获取纹理信息
    GLKTextureInfo* textureInfo = [GLKTextureLoader
                                   cubeMapWithContentsOfFile:path
                                   options:nil
                                   error:&error];
    if (error) {
        NSLog(@"error %@", error);
    }
    
    self.skyEffect = [[SkyEffect alloc] init];
    self.skyEffect.textureCubeMap.name = textureInfo.name;
    self.skyEffect.textureCubeMap.target = textureInfo.target;
    
    self.skyEffect.xSize = 6.0f;
    self.skyEffect.ySize = 6.0f;
    self.skyEffect.zSize = 6.0f;
    
    self.cPauseSwitch = [[UISwitch alloc] initWithFrame:CGRectMake(20, 50, 44, 44)];
    [self.view addSubview:self.cPauseSwitch];
    
    
    
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    
    glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (!self.cPauseSwitch.on) {
        //更新变换矩阵
        [self setMatrix];
    }
    
    self.skyEffect.center = self.eyePosition;
    self.skyEffect.transform.projectionMatrix = self.baseEffect.transform.projectionMatrix;
    self.skyEffect.transform.modelviewMatrix = self.baseEffect.transform.modelviewMatrix;
    [self.skyEffect prepareToDraw];
    
    glDepthMask(GL_FALSE);
    [self.skyEffect draw];
    
    glDepthMask(GL_TRUE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

//    glBindVertexArrayOES(self.cPositionBuffer);
    glBindVertexArrayOES(0);

    
    
    for(int i=0; i<starshipMaterials; i++)
    {
        //设置材质的漫反射颜色
        self.baseEffect.material.diffuseColor = GLKVector4Make(starshipDiffuses[i][0], starshipDiffuses[i][1], starshipDiffuses[i][2], 1.0f);

        //设置反射光颜色
        self.baseEffect.material.specularColor = GLKVector4Make(starshipSpeculars[i][0], starshipSpeculars[i][1], starshipSpeculars[i][2], 1.0f);

        //飞船准备绘制
        [self.baseEffect prepareToDraw];

        glDrawArrays(GL_TRIANGLES, starshipFirsts[i], starshipCounts[i]);
    }
    
}

- (void) setMatrix {
    
    GLfloat aspectRatio = self.view.frame.size.width / self.view.frame.size.height;
    
    self.baseEffect.transform.projectionMatrix = GLKMatrix4MakePerspective(GLKMathDegreesToRadians(85.0f), aspectRatio, 1.0f, 20.0f);
    self.baseEffect.transform.modelviewMatrix = GLKMatrix4MakeLookAt(self.eyePosition.x, self.eyePosition.y, self.eyePosition.z, self.lookPosition.x, self.lookPosition.y, self.lookPosition.z, self.upPosition.x, self.upPosition.y, self.upPosition.z);
    
    self.angle += 0.003;
    
    self.eyePosition = GLKVector3Make(-5.0f * sinf(self.angle),
                                      -5.0f,
                                      -5.0f * cosf(self.angle));
    
    // 调整观察的位置
    self.lookPosition = GLKVector3Make(0.0,
                                         1.5 + -5.0f * sinf(0.3 * self.angle),
                                         0.0);}


@end
