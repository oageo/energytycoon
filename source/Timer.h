#ifndef TIMER_H
#define TIMER_H

/*-----------------------------------------------------------------------------------------------*/

#include "StdAfx.h"

/*-----------------------------------------------------------------------------------------------*/

//! A single timer
class TimerInstance {
public:

	//! Create the timer
	TimerInstance(std::string pName, float pSeconds) :
	  mName(pName), mSeconds(pSeconds), mSecondsDone(0.0)
	{}

	std::string mName;
	float mSeconds;
	float mSecondsDone;
};

/*-----------------------------------------------------------------------------------------------*/

//! Handles all timers
class GameTimer {
public:

	//! Start a timer
	static void startTimer(TimerInstance pTimerInstance)
	{
		mTimerInstances.push_back(pTimerInstance);
	}

	//! Check if timer is still running
	static float checkTimer(std::string pName)
	{
		for (size_t i = 0; i < mTimerInstances.size(); i++)
			if (mTimerInstances[i].mName == pName)
				return mTimerInstances[i].mSecondsDone;
		
		return false;
	}

	//! Update all timers
	static void updateTimers(float pElapsedTime)
	{
		std::vector<TimerInstance>::iterator it = mTimerInstances.begin();

		while(it != mTimerInstances.end()) {
			(*it).mSecondsDone += pElapsedTime;

			if((*it).mSecondsDone >= (*it).mSeconds)
				it = mTimerInstances.erase(it);
			else
				it++;
		}
	}

private:

	static std::vector<TimerInstance> mTimerInstances; //!< One and only instance
};

/*-----------------------------------------------------------------------------------------------*/

#endif // TIMER_H