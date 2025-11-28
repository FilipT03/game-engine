#pragma once

#include "Core/Core.h"
#include <unordered_map>

namespace ft {

	class FT_API ScriptComponent
	{
	public:
		ScriptComponent();
		virtual ~ScriptComponent();
		virtual void OnUpdate() {}
		virtual void OnRegister() {}
		virtual void OnClose() {}
		virtual void OnDestroy() {}
		uint16_t GetId() { return m_Id; }
	private:
		void SetId(uint16_t id) { m_Id = id; }
		uint16_t m_Id = 0;

		friend class Application;
	};
}

