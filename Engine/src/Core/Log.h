#pragma once

#include <memory>

#include "Core.h"
#include "spdlog/spdlog.h"

namespace ft {

	class FT_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#define FT_CORE_TRACE(...)    ::ft::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define FT_CORE_INFO(...)     ::ft::Log::GetCoreLogger()->info(__VA_ARGS__)
#define FT_CORE_WARN(...)     ::ft::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define FT_CORE_ERROR(...)    ::ft::Log::GetCoreLogger()->error(__VA_ARGS__)
#define FT_CORE_CRITICAL(...) ::ft::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define FT_TRACE(...)         ::ft::Log::GetClientLogger()->trace(__VA_ARGS__)
#define FT_INFO(...)          ::ft::Log::GetClientLogger()->info(__VA_ARGS__)
#define FT_WARN(...)          ::ft::Log::GetClientLogger()->warn(__VA_ARGS__)
#define FT_ERROR(...)         ::ft::Log::GetClientLogger()->error(__VA_ARGS__)
#define FT_CRITICAL(...)      ::ft::Log::GetClientLogger()->critical(__VA_ARGS__)