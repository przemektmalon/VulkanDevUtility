#pragma once
#include "DeviceMemory.hpp"
#include "LogicalDevice.hpp"
#include "PCH.hpp"
#include "RenderPass.hpp"

namespace vdu {
class Framebuffer {
public:
  Framebuffer() {}

  void addAttachment(vdu::Texture *att, std::string name);

  void create(vdu::LogicalDevice *device, vdu::RenderPass *renderPass);

  void destroy();

  const VkFramebuffer &getHandle() { return m_framebuffer; }

private:
  VkFramebuffer m_framebuffer;

  vdu::RenderPass *m_renderPass;

  vdu::LogicalDevice *m_logicalDevice;

  std::vector<vdu::Texture *> m_attachments;
  std::vector<std::string> m_attachmentNames;
};
} // namespace vdu
