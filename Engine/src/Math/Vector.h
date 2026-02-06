#pragma once

#include "Core/Core.h"

#include<glm/glm.hpp>

namespace ft {

	class Vector
	{
	public:
		~Vector() = default;

		static const glm::vec3 Up, Down, Left, Right, Forward, Back;

	private:
		Vector() = default;
	};
}

