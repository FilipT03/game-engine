#pragma once

#include "Core/Core.h"

namespace ft {

	class Time
	{
	public:
		/// In seconds
		static double DeltaTime() { return s_DeltaTime; }
		static float DeltaTimeF() { return (float)s_DeltaTime; }
		static void SetDeltaTime(double time) { s_DeltaTime = time; }
		
		/// In seconds
		static double TotalTime() { return s_Time; }
		static void Reset() { s_Time = 0; s_DeltaTime = 0; }
		static void UpdateTime(double newTime) 
		{
			s_DeltaTime = newTime - s_Time;
			s_Time = newTime;
		}

	private:
		inline static double s_DeltaTime = 0;
		inline static double s_Time = 0;
	};

}

