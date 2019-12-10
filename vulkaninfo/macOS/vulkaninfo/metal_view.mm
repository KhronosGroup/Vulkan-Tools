/*
 * Copyright (c) 2018 The Khronos Group Inc.
 * Copyright (c) 2018 Valve Corporation
 * Copyright (c) 2018 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Jeremy Kniager <jeremyk@lunarg.com>
 */

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

@interface NativeMetalView : NSView
@end

@implementation NativeMetalView
- (id)initWithFrame:(NSRect)frame {
    if (self = [super initWithFrame:frame]) {
        self.wantsLayer = YES;
    }
    return self;
}

+ (Class)layerClass {
    return [CAMetalLayer class];
}

- (CALayer*)makeBackingLayer {
    CALayer* layer = [self.class.layerClass layer];
    CGSize viewScale = [self convertSizeToBacking:CGSizeMake(1.0, 1.0)];
    layer.contentsScale = MIN(viewScale.width, viewScale.height);
    return layer;
}
@end

void* CreateMetalView(uint32_t width, uint32_t height) {
    return [[NativeMetalView alloc] initWithFrame:NSMakeRect(0, 0, width, height)];
}

void DestroyMetalView(void* view) { [(NativeMetalView*)view dealloc]; }

void* GetCAMetalLayerFromMetalView(void* view) { return ((NativeMetalView*)view).layer; }
