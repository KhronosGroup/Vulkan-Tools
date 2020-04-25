/*
** Copyright (c) 2015-2020 The Khronos Group Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

namespace mock_icd {

static VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(VkInstance instance, const char* pName) {
    return GetFptr(name_to_funcptr_map, pName);
}

static VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceVersion(uint32_t* pApiVersion) {
    assert(loader_interface_version >= 5);  // we should not get here if loader is not Vulkan 1.1
    *pApiVersion = kInstanceVersion;
    return VK_SUCCESS;
}

static VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount,
                                                                           VkExtensionProperties* pProperties) {
    return EnumerateExtensions(kInstanceExtensions, pLayerName, pPropertyCount, pProperties);
}

static VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
    assert(false);  // Loader should never call ICD via vkEnumerateInstanceLayerProperties
    *pPropertyCount = 0;
    return VK_SUCCESS;
}

static VKAPI_ATTR VkResult VKAPI_CALL CreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                                                     const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    if (loader_interface_version < kMinRequiredInterfaceVersion) return VK_ERROR_INCOMPATIBLE_DRIVER;

    // loader interface 4 requires us to fail if not Vulkan 1.0
    if (loader_interface_version <= 4 && pCreateInfo->pApplicationInfo &&
        pCreateInfo->pApplicationInfo->apiVersion >= VK_API_VERSION_1_1) {
        return VK_ERROR_INCOMPATIBLE_DRIVER;
    }

    *pInstance = (VkInstance)CreateDispObjHandle();
    {
        lock_guard_t lock(global_lock);
        for (auto& physical_device : physical_device_map[*pInstance]) physical_device = (VkPhysicalDevice)CreateDispObjHandle();
    }

    return VK_SUCCESS;
}

static VKAPI_ATTR void VKAPI_CALL DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
    if (instance) {
        {
            lock_guard_t lock(global_lock);
            for (const auto physical_device : physical_device_map.at(instance)) DestroyDispObjHandle((void*)physical_device);
            physical_device_map.erase(instance);
        }
        DestroyDispObjHandle((void*)instance);
    }
}

static VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount,
                                                               VkPhysicalDevice* pPhysicalDevices) {
    std::array<VkPhysicalDevice, kPhysicalDeviceCount> physical_devices;
    {
        lock_guard_t lock(global_lock);
        physical_devices = physical_device_map.at(instance);
    }

    return Enumerate(physical_devices, pPhysicalDeviceCount, pPhysicalDevices);
}

static VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName,
                                                                         uint32_t* pPropertyCount,
                                                                         VkExtensionProperties* pProperties) {
    return EnumerateExtensions(kDeviceExtensions, pLayerName, pPropertyCount, pProperties);
}

static VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount,
                                                                     VkLayerProperties* pProperties) {
    assert(false);  // Loader should never call ICD via vkEnumerateDeviceLayerProperties
    *pPropertyCount = 0;
    return VK_SUCCESS;
}

static VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice,
                                                                         uint32_t* pQueueFamilyPropertyCount,
                                                                         VkQueueFamilyProperties* pQueueFamilyProperties) {
    if (pQueueFamilyProperties && *pQueueFamilyPropertyCount > 0)
        *pQueueFamilyProperties = kQueueFamilyProps;
    else if (!pQueueFamilyProperties || *pQueueFamilyPropertyCount > 0)
        *pQueueFamilyPropertyCount = 1;
}

static VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice,
                                                                          uint32_t* pQueueFamilyPropertyCount,
                                                                          VkQueueFamilyProperties2* pQueueFamilyProperties) {
    const VkQueueFamilyProperties2 props = {VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2, nullptr, kQueueFamilyProps};
    if (pQueueFamilyProperties && *pQueueFamilyPropertyCount > 0)
        *pQueueFamilyProperties = props;
    else if (!pQueueFamilyProperties || *pQueueFamilyPropertyCount > 0)
        *pQueueFamilyPropertyCount = 1;
}

static VKAPI_ATTR VkResult VKAPI_CALL CreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo,
                                                   const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    *pDevice = (VkDevice)CreateDispObjHandle();
    {
        lock_guard_t lock(global_lock);
        queue_map[*pDevice] = (VkQueue)CreateDispObjHandle();
    }

    return VK_SUCCESS;
}

static VKAPI_ATTR void VKAPI_CALL DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    lock_guard_t lock(global_lock);
    DestroyDispObjHandle((void*)queue_map.at(device));
    queue_map.erase(device);
    buffer_map.erase(device);
    image_memory_size_map.erase(device);

    DestroyDispObjHandle((void*)device);
}

static VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetDeviceProcAddr(VkDevice device, const char* pName) {
    // TODO(krOoze): this function should return nullptr for instance-level functions
    return GetInstanceProcAddr(nullptr, pName);
}

static VKAPI_ATTR void VKAPI_CALL GetDeviceQueue2(VkDevice device, const VkDeviceQueueInfo2* pQueueInfo, VkQueue* pQueue) {
    lock_guard_t lock(global_lock);
    *pQueue = queue_map.at(device);
}

static VKAPI_ATTR void VKAPI_CALL GetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue) {
    const VkDeviceQueueInfo2 dqi2 = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2, nullptr, 0, queueFamilyIndex, queueIndex};
    GetDeviceQueue2(device, &dqi2, pQueue);
}

}  // namespace mock_icd