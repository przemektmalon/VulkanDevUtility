#pragma once
#include "LogicalDevice.hpp"
#include "PCH.hpp"


/*
Wrappers for command, descriptor, and query pools
*/

namespace vdu {
class CommandBuffer;
class QueueFamily;

class DescriptorPool {
public:
  DescriptorPool();

  void create(LogicalDevice *logicalDevice);

  void destroy();

  VkDescriptorPool getHandle() { return m_descriptorPool; }

  void addPoolCount(VkDescriptorType type, uint32_t count);
  void addSetCount(uint32_t count);

  void setFreeable(bool freeable) { m_freeable = freeable; }

private:
  VkDescriptorPool m_descriptorPool;
  bool m_freeable;

  std::map<VkDescriptorType, uint32_t> m_descriptorTypeCounts;
  uint32_t m_maxSets;

  LogicalDevice *m_logicalDevice;
};

class CommandPool {
public:
  CommandPool();

  void create(LogicalDevice *logicalDevice);

  void destroy();

  VkCommandPool getHandle() { return m_commandPool; }

  void setQueueFamily(const QueueFamily *family) { m_queueFamily = family; }
  void setFlags(VkCommandPoolCreateFlagBits flags) { m_flags = flags; }

private:
  VkCommandPool m_commandPool;
  const QueueFamily *m_queueFamily;
  LogicalDevice *m_logicalDevice;
  VkCommandPoolCreateFlagBits m_flags;
};

class QueryPool {
public:
  QueryPool();

  void create(LogicalDevice *logicalDevice);
  void destroy();

  void setQueryType(VkQueryType type);
  void setPipelineStats(VkQueryPipelineStatisticFlags flags);
  void setQueryCount(uint32_t count);

  uint64_t *query();
  uint64_t *query(uint32_t first, uint32_t count);

  void cmdReset(const vdu::CommandBuffer &cmd);
  void cmdReset(const vdu::CommandBuffer &cmd, uint32_t first, uint32_t count);
  void cmdReset(const VkCommandBuffer &cmd);
  void cmdReset(const VkCommandBuffer &cmd, uint32_t first, uint32_t count);

  VkQueryPool getHandle() { return m_queryPool; }

  void cmdTimestamp(const vdu::CommandBuffer &cmd,
                    VkPipelineStageFlagBits flags, uint32_t index);
  void cmdTimestamp(const VkCommandBuffer &cmd, VkPipelineStageFlagBits flags,
                    uint32_t index);

private:
  VkQueryPool m_queryPool;
  LogicalDevice *m_logicalDevice;
  VkQueryPipelineStatisticFlags m_pipelineStats;
  VkQueryType m_type;
  uint32_t m_count;

  uint64_t *m_queryData;
};

} // namespace vdu
