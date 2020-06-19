#pragma once
#include "PCH.hpp"
#include "Queue.hpp"

namespace vdu {
class PhysicalDevice;

/*
        Wrapper for queue family
        */
class QueueFamily {
public:
  QueueFamily(PhysicalDevice *physicalDevice, VkQueueFamilyProperties props,
              uint32_t familyIndex);

  VkResult queryPresentCapability(VkSurfaceKHR surface);

  bool supportsGraphics() const { return m_supportsGraphics; }
  bool supportsCompute() const { return m_supportsCompute; }
  bool supportsTransfer() const { return m_supportsTransfer; }
  bool supportsPresent() const { return m_supportsPresent; }

  uint32_t getQueueCount() const { return m_familyProperties.queueCount; }
  uint32_t getTimestampValidBits() const {
    return m_familyProperties.timestampValidBits;
  }
  VkExtent3D getMinImageTransferGranularity() const {
    return m_familyProperties.minImageTransferGranularity;
  }

  uint32_t getIndex() const { return m_familyIndex; }

  Queue createQueue(float priority = 1.f);

  QueueFamily *getThis() const { return const_cast<QueueFamily *>(this); }

private:
  /*
          Fill in properties
          */
  void queryDetails();

  PhysicalDevice *m_physicalDevice;

  VkQueueFamilyProperties m_familyProperties;

  uint32_t m_familyIndex;

  bool m_supportsGraphics;
  bool m_supportsCompute;
  bool m_supportsTransfer;
  bool m_supportsPresent;
};
} // namespace vdu
