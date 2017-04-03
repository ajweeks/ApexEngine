
#include "Game\Debug\ImGuiStorage.h"

ImGuiID ImGuiStorage::GetID(const void* ptr)
{
	ImGuiID result = reinterpret_cast<ImGuiID>(ptr);
	return result;
}

ImGuiID ImGuiStorage::GetID(const char* str)
{
	ImGuiID result = reinterpret_cast<ImGuiID>(str);
	return result;
}

void ImGuiStorage::ClearAll()
{
	m_Data.clear();
}

bool ImGuiStorage::ClearValue(ImGuiID id)
{
	auto it = m_Data.find(id);
	if (it == m_Data.end() || it->first != id)
		return false;

	m_Data.erase(it);
	return true;
}

ImGuiValue ImGuiStorage::GetValue(ImGuiID id) const
{
	static ImGuiValue defaultValue;
	defaultValue.valInt = -1;

	auto it = m_Data.find(id);
	if (it == m_Data.end() || it->first != id)
		return defaultValue;
	return it->second;
}

bool ImGuiStorage::GetBool(ImGuiID id) const
{
	const ImGuiValue value = GetValue(id);
	return (value.valInt == 1);
}

int ImGuiStorage::GetInt(ImGuiID id) const
{
	const ImGuiValue value = GetValue(id);
	return value.valInt;
}

float ImGuiStorage::GetFloat(ImGuiID id) const
{
	ImGuiValue value = GetValue(id);
	if (value.valInt == -1) value.valFloat = -1.0f;
	return value.valFloat;
}

void* ImGuiStorage::GetVoid(ImGuiID id) const
{
	ImGuiValue value = GetValue(id);
	if (value.valInt == -1) value.valPtr = nullptr;
	return value.valPtr;
}

void ImGuiStorage::SetValue(ImGuiID id, ImGuiValue value)
{
	std::map<ImGuiID, ImGuiValue>::iterator it = m_Data.find(id);
	std::pair<ImGuiID, ImGuiValue> pair(id, value);

	if (it == m_Data.end() || it->first != id)
	{
		m_Data.insert(it, pair);
		return;
	}
	// Replace existing value
	m_Data.erase(it);
	m_Data.insert(pair);
}

void ImGuiStorage::SetBool(ImGuiID id, bool value)
{
	ImGuiValue v;
	v.valInt = value ? 1 : 0;
	SetValue(id, v);
}

void ImGuiStorage::SetInt(ImGuiID id, int value)
{
	ImGuiValue v;
	v.valInt = value;
	SetValue(id, v);
}

void ImGuiStorage::SetFloat(ImGuiID id, float value)
{
	ImGuiValue v;
	v.valFloat = value;
	SetValue(id, v);
}

void ImGuiStorage::SetVoid(ImGuiID id, void * value)
{
	ImGuiValue v;
	v.valPtr = value;
	SetValue(id, v);
}
