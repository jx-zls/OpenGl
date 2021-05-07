//
//  ViewController.m
//  加载顶点数据与纹理
//
//  Created by Tocce on 2021/4/22.
//

#import "ViewController.h"
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
@interface ViewController ()<GLKViewDelegate>
{
    EAGLContext *context;
    GLKBaseEffect *effect;
    
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self configureContext];
    [self confitureVertex];
    [self configureTexture];

    
}

/**
 GLuint                      name;
 GLenum                      target;
 GLuint                      width;
 GLuint                      height;
 GLuint                      depth;
 GLKTextureInfoAlphaState    alphaState;
 GLKTextureInfoOrigin        textureOrigin;
 BOOL                        containsMipmaps;
 GLuint                      mimapLevelCount;
 GLuint                      arrayLength;
}
 */

- (GLKTextureInfo *)getRecourse:(NSString *)path {
    NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:@(1), GLKTextureLoaderOriginBottomLeft,NULL];
    NSLog(@"%@", options);
    GLKTextureInfo *info = [GLKTextureLoader  textureWithContentsOfFile:path options:options error:NULL];
    return  info;
}

- (void)configureTexture {
//    NSString *path = [[NSBundle mainBundle] pathForResource:@"0001" ofType:@"jpg"];
//    NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:@(1), GLKTextureLoaderOriginBottomLeft,NULL];
//    NSLog(@"%@", options);
//    GLKTextureInfo *info = [GLKTextureLoader  textureWithContentsOfFile:path options:options error:NULL];
    GLKTextureInfo *info1 = [self getRecourse:[[NSBundle mainBundle] pathForResource:@"0001" ofType:@"jpg"]];
    GLKTextureInfo *info2 = [self getRecourse:[[NSBundle mainBundle] pathForResource:@"test" ofType:@"png"]];
    effect = [[GLKBaseEffect alloc] init];
    effect.texture2d0.enabled = GL_TRUE;
    effect.texture2d0.name = info2.name;
    
    
    
}

- (void)confitureVertex {
    
    GLfloat vertex[] = {
        0.5, -0.5, 0.0f,    1.0f, 0.0f,
        0.5, 0.5, -0.0f,    1.0f, 1.0f,
        -0.5, 0.5, 0.0f,    0.0f, 1.0f,
        
        0.5, -0.5, 0.0f,    1.0f, 0.0f,
        -0.5, 0.5, 0.0f,    0.0f, 1.0f,
        -0.5, -0.5, 0.0f,   0.0f, 0.0f,
    };
    
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*5, (GLfloat *)NULL + 0);
    
    glEnableVertexAttribArray(GLKVertexAttribTexCoord0);
    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, (GLfloat *)NULL + 3);
}


- (void)configureContext {
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    if (!context) {
        exit(0);
    }
    
    GLKView *view = self.view;
    view.context = context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    [EAGLContext setCurrentContext:context];

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
}

-(void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    [effect prepareToDraw];
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
}


@end
