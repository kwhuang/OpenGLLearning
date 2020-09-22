//
//  ViewController.m
//  OpenGLESDemo
//
//  Created by mac on 2020/9/22.
//

#import "ViewController.h"
#import <GLKit/GLKit.h>
#import "Demo1View.h"

@interface ViewController () <GLKViewDelegate>

@property (nonatomic, strong) EAGLContext *context;

@property (nonatomic, strong) GLKBaseEffect *baseEffect;

@property (nonatomic, strong) Demo1View *demo1View;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self demo1];
    [self uploadVertexArray];
    [self uploadTexture];
}

- (void)demo1
{
    _demo1View = [[Demo1View alloc]initWithFrame:self.view.bounds];
    _demo1View.delegate = self;
    [self.view addSubview:_demo1View];
    
    // 初始化对象并声明OpenGL ES版本
    _context = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES2];
    _demo1View.context = _context;
    // 设置颜色缓冲区格式
    _demo1View.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    // 设置当前上下文
    [EAGLContext setCurrentContext:_context];
    
}

- (void)uploadVertexArray
{
    //顶点数据，前三个是顶点坐标（x、y、z轴），后面两个是纹理坐标（x，y）
    GLfloat vertexData[] =
    {
        0.5, -0.5, 0.0f,    1.0f, 0.0f, //右下
        0.5, 0.5, -0.0f,    1.0f, 1.0f, //右上
        -0.5, 0.5, 0.0f,    0.0f, 1.0f, //左上
        
        0.5, -0.5, 0.0f,    1.0f, 0.0f, //右下
        -0.5, 0.5, 0.0f,    0.0f, 1.0f, //左上
        -0.5, -0.5, 0.0f,   0.0f, 0.0f, //左下
    };
    
    //顶点数据缓存
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(GLKVertexAttribPosition); //顶点数据缓存
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLfloat *)NULL + 0);
    glEnableVertexAttribArray(GLKVertexAttribTexCoord0); //纹理
    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLfloat *)NULL + 3);

}

- (void)uploadTexture
{
    //纹理贴图
    NSString* filePath = [[NSBundle mainBundle] pathForResource:@"demo1" ofType:@"jpg"];
    NSDictionary* options = [NSDictionary dictionaryWithObjectsAndKeys:@(1), GLKTextureLoaderOriginBottomLeft, nil];//GLKTextureLoaderOriginBottomLeft 纹理坐标系是相反的
    GLKTextureInfo* textureInfo = [GLKTextureLoader textureWithContentsOfFile:filePath options:options error:nil];
    //着色器
    _baseEffect = [[GLKBaseEffect alloc] init];
    _baseEffect.texture2d0.enabled = GL_TRUE;
    _baseEffect.texture2d0.name = textureInfo.name;
}

#pragma mark -GLKViewDelegate
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    glClearColor(0.3f, 0.6f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //启动着色器
    [_baseEffect prepareToDraw];
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


@end
