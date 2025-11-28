#include "SimpleScript.h"

void SimpleScript::OnRegister()
{
	FT_TRACE("Registered SimpleScript");
}

void SimpleScript::OnUpdate()
{
	//FT_INFO("Update loop, timeDelta is {}", ft::Time::DeltaTime());
	FT_INFO("Update loop, total time is {0:.3f}s", ft::Time::TotalTime());
}