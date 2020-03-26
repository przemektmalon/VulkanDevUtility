#include "PCH.hpp"
#include "Debug.hpp"

void vdu::ObjectNamer::addName(VkBuffer buffer, const std::string &name)
{
	m_bufferNames[buffer] = name;
}

void vdu::ObjectNamer::removeName(VkBuffer buffer)
{
	m_bufferNames.erase(buffer);
}

std::string vdu::ObjectNamer::getName(VkBuffer buffer)
{
	auto find = m_bufferNames.find(buffer);
	if (find != m_bufferNames.end())
		return find->second;
	else
		return "unnamed buffer";
}
