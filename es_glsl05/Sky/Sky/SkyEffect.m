//
//  SkyEffect.m
//  Sky
//
//  Created by Tocce on 2021/5/7.
//

#import "SkyEffect.h"
#import <OpenGLES/ES2/glext.h>

const static int SkyboxNumVertexIndices = 14;
const static int SkyboxNumCoords = 24;
enum
{
    MVPMatrix,
    SamplersCube,
    NumUniforms
    
};

@interface SkyEffect ()
{
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint program;
    GLuint vertexArrayID;
    GLuint uniforms[NumUniforms];
}


- (BOOL)loadShaders;

- (BOOL)compileShader:(GLuint *)shader
                 type:(GLenum)type
                 file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;


@end

@implementation SkyEffect


- (id) init {
    self = [super init];
    if(self != nil){
        
        _textureCubeMap = [[GLKEffectPropertyTexture alloc] init];
        _textureCubeMap.enabled = true;
        _textureCubeMap.name = 0;
        _textureCubeMap.target = GLKTextureTargetCubeMap;
        
        _transform = [[GLKEffectPropertyTransform alloc] init];
        self.center = GLKVector3Make(0, 0, 0);
        self.xSize = 1.0f;
        self.ySize = 1.0f;
        self.zSize = 1.0f;
        
        
        const float vertices[SkyboxNumCoords] = {
            -0.5, -0.5,  0.5, // 左下 0
            0.5, -0.5,  0.5, // 右下 1
            -0.5,  0.5,  0.5, // 左上 2
            0.5,  0.5,  0.5, // 右上 3
            -0.5, -0.5, -0.5, // 内左下 4
            0.5, -0.5, -0.5, // 内右上 5
            -0.5,  0.5, -0.5, // 内左上 6
            0.5,  0.5, -0.5, // 内右上 7
        };
        
    
        glGenBuffers(1, &vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        const GLubyte indices[SkyboxNumVertexIndices] = {
            1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1, 2
        };
        
        glGenBuffers(1, &indexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
    }
    return self;
}


- (void)draw {
    glDrawElements(GL_TRIANGLE_STRIP,
                   SkyboxNumVertexIndices,
                   GL_UNSIGNED_BYTE,
                   NULL);
}

- (void) prepareToDraw {
    if(program == 0){
        [self loadShaders];
    }
    
    if(program != 0){

        glUseProgram(program);

        GLKMatrix4 skyModelMat = GLKMatrix4Translate(self.transform.modelviewMatrix, self.center.x, self.center.y, self.center.z);
        skyModelMat = GLKMatrix4Scale(skyModelMat, self.xSize, self.ySize, self.zSize);

        GLKMatrix4 projectionMat = GLKMatrix4Multiply(self.transform.projectionMatrix, skyModelMat);

        glUniformMatrix4fv(uniforms[MVPMatrix], 1, GL_FALSE, projectionMat.m);
        glUniform1f(uniforms[SamplersCube], 0);

        if(vertexArrayID == 0){
            glGenVertexArraysOES(1, &vertexArrayID);
            glBindVertexArrayOES(vertexArrayID);

            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

            glEnableVertexAttribArray(GLKVertexAttribPosition);
            glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*3, 0);
        }else{
            glBindVertexArrayOES(vertexArrayID);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

        if(self.textureCubeMap.enabled){
            glBindTexture(GL_TEXTURE_CUBE_MAP, self.textureCubeMap.name);
        }else{
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
    }
    
   
}


- (void)dealloc
{
    if(0 != vertexArrayID)
    {
        glDeleteVertexArraysOES(1, &vertexArrayID);
        vertexArrayID = 0;
    }
    if(0 != indexBufferID)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &vertexBufferID);
    }
    if(0 != indexBufferID)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &indexBufferID);
    }
    if(0 != program)
    {
        glUseProgram(0);
        glDeleteProgram(program);
    }
}

- (BOOL)loadShaders {
    
    program = glCreateProgram();
    GLuint vertexShader, fragShader;
    
    NSString *vertexShaderPath, *fragShaderPath;
    
    vertexShaderPath = [[NSBundle mainBundle] pathForResource:@"SkyboxShader" ofType:@"vsh"];
    fragShaderPath =  [[NSBundle mainBundle] pathForResource:@"SkyboxShader" ofType:@"fsh"];
    
    if(![self compileShader:&vertexShader type:GL_VERTEX_SHADER file:vertexShaderPath]){
        NSLog(@"vertexShader 编译失败 ===");
        return false;
    }
    if(![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPath]){
        NSLog(@"fragShader 编译失败 ===");
        return false;
    }
    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragShader);
    
    glBindAttribLocation(program, GLKVertexAttribPosition, "a_position");
    
    if(![self linkProgram:program]){
        
        NSLog(@"program 链接失败 ===");
        if (vertexShader)
        {
            glDeleteShader(vertexShader);
            vertexShader = 0;
        }
        if (fragShader)
        {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (program)
        {
            glDeleteProgram(program);
            program = 0;
        }
        
        return false;
    }
    
    uniforms[MVPMatrix] = glGetUniformLocation(program, "u_mvpMatrix");
    uniforms[SamplersCube] = glGetUniformLocation(program, "u_samplersCube");
    
    if (vertexShader)
    {
        glDetachShader(program, vertexShader);
        glDeleteShader(vertexShader);
    }
    if (fragShader)
    {
        glDetachShader(program, fragShader);
        glDeleteShader(fragShader);
    }
    
    return true;
}

- (BOOL)compileShader:(GLuint *)shader
                 type:(GLenum)type
                 file:(NSString *)file {
    
    NSString *source = [NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil];
    GLchar *bytes = (GLchar *)[source UTF8String];
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &bytes, NULL);
    glCompileShader(*shader);
    
    GLint status;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &status);
    if(status > 0){
        GLchar *log = (GLchar *)malloc(status);
        glGetShaderInfoLog(*shader, status, &status, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
        return false;
    }

    return true;
}

- (BOOL)linkProgram:(GLuint)prog {
    
    glLinkProgram(prog);
    
    GLint loglength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &loglength);
    if(loglength > 0){
        
        GLchar *log = (GLchar*) malloc(loglength);
        glGetProgramInfoLog(prog, loglength, &loglength, log);
        NSLog(@"program link log:\n%s", log);
        free(log);
        
        return false;
    }
    
    return true;
}

- (BOOL)validateProgram:(GLuint)prog {
    
    return true;
}



@end
