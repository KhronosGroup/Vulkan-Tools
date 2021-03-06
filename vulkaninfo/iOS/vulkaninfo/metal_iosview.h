/*
 * Copyright (c) 2018-2020 The Khronos Group Inc.
 * Copyright (c) 2018-2020 Valve Corporation
 * Copyright (c) 2018-2020 LunarG, Inc.
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
 * Author: Richard Wright <richard@lunarg.com>
 */

#ifndef metal_iosview_h
#define metal_iosview_h

void* CreateMetalView(uint32_t width, uint32_t height);

void DestroyMetalView(void* view);

void* GetCAMetalLayerFromMetalView(void* view);

#endif /* metal_view_h */
