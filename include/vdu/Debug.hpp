#include "PCH.hpp"

namespace vdu
{
class ObjectNamer
{
public:
	ObjectNamer() {}

	void addName(VkBuffer buffer, const std::string &name);

	void removeName(VkBuffer buffer);

	std::string getName(VkBuffer buffer);

private:
	std::unordered_map<VkBuffer, std::string> m_bufferNames;
};
} // namespace vdu