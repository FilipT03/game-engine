#pragma once

#include "Core/Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

namespace ft {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_EngineLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_EngineLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

#define FT_ENGINE_TRACE(...) ::ft::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define FT_ENGINE_INFO(...) ::ft::Log::GetCoreLogger()->info(__VA_ARGS__)
#define FT_ENGINE_WARN(...) ::ft::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define FT_ENGINE_ERROR(...) ::ft::Log::GetCoreLogger()->error(__VA_ARGS__)
#define FT_ENGINE_CRITICAL(...) ::ft::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define FT_TRACE(...) ::ft::Log::GetClientLogger()->trace(__VA_ARGS__)
#define FT_INFO(...) ::ft::Log::GetClientLogger()->info(__VA_ARGS__)
#define FT_WARN(...) ::ft::Log::GetClientLogger()->warn(__VA_ARGS__)
#define FT_ERROR(...) ::ft::Log::GetClientLogger()->error(__VA_ARGS__)
#define FT_CRITICAL(...) ::ft::Log::GetClientLogger()->critical(__VA_ARGS__)