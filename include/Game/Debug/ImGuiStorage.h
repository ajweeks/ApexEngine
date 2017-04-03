#pragma once

#include <map>
#include <vector>

typedef unsigned int ImGuiID;

struct ImGuiValue
{
	union
	{
		int valInt;
		float valFloat;
		void* valPtr;
	};
};

struct ImGuiStorage
{
	static ImGuiID GetID(const void* ptr);
	static ImGuiID GetID(const char* str);

	void ClearAll();
	bool ClearValue(ImGuiID id);

	ImGuiValue GetValue(ImGuiID id) const;
	bool GetBool(ImGuiID id) const;
	int GetInt(ImGuiID id) const;
	float GetFloat(ImGuiID id) const;
	void* GetVoid(ImGuiID id) const;

	void SetValue(ImGuiID id, ImGuiValue value);
	void SetBool(ImGuiID id, bool value);
	void SetInt(ImGuiID id, int value);
	void SetFloat(ImGuiID id, float value);
	void SetVoid(ImGuiID id, void* value);

	std::map<ImGuiID, ImGuiValue> m_Data;

};
