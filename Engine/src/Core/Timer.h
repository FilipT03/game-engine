#pragma once

#include "Core/Core.h"

namespace ft {

	class Timer
	{
	public:
		Timer() { Reset(); }
		
		void Reset() 
		{
			m_Start = std::chrono::high_resolution_clock::now();
		}

		/// <summary>
		///		Get time since initiliazation in seconds.
		/// </summary>
		double Elapsed() const
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001 * 0.001;
		}

		double ElapsedMiliseconds() const
		{
			return Elapsed() * 1000.0;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	};
}

