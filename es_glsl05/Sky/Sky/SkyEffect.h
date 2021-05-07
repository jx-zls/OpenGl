//
//  SkyEffect.h
//  Sky
//
//  Created by Tocce on 2021/5/7.
//

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface SkyEffect : NSObject<GLKNamedEffect>

@property(nonatomic, assign) GLKVector3 center;
@property(nonatomic,assign)GLfloat xSize;
@property(nonatomic,assign)GLfloat ySize;
@property(nonatomic,assign)GLfloat zSize;

@property(nonatomic, strong) GLKEffectPropertyTransform *transform;
@property(nonatomic, strong) GLKEffectPropertyTexture *textureCubeMap;


- (void)prepareToDraw;
- (void)draw;


@end

NS_ASSUME_NONNULL_END
