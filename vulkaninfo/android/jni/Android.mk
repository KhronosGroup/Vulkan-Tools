# Copyright 2015 The Android Open Source Project
# Copyright (C) 2015 Valve Corporation

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#      http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(abspath $(call my-dir))
SRC_DIR := $(LOCAL_PATH)/../../..
VULKANINFO_DIR := $(SRC_DIR)/vulkaninfo

include $(CLEAR_VARS)
LOCAL_MODULE := vulkaninfo
LOCAL_SRC_FILES += $(VULKANINFO_DIR)/vulkaninfo.cpp
LOCAL_C_INCLUDES += $(SRC_DIR)/build-android/third_party/Vulkan-Headers/include \
                    $(VULKANINFO_DIR) \
                    $(VULKANINFO_DIR)/generated
LOCAL_CFLAGS += -DVK_USE_PLATFORM_ANDROID_KHR -DVK_ENABLE_BETA_EXTENSIONS
LOCAL_LDLIBS := -llog -landroid
include $(BUILD_EXECUTABLE)
