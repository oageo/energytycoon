#ifndef GAMETIME_H
#define GAMETIME_H

/*-----------------------------------------------------------------------------------------------*/

#include "Event.h"

/*-----------------------------------------------------------------------------------------------*/

//! Possible speed values
enum eSpeed { ePause, eSlow, eNormal, eFast };

/*-----------------------------------------------------------------------------------------------*/

const eSpeed cDefaultSpeed = eNormal; //!< Start speed

/*-----------------------------------------------------------------------------------------------*/

//! Handles the game time, raises game time events (e.g. "eWeekPassed", "eDayPassed")
class GameTime {
public:

	//! Start
	GameTime(void);
	
	//! Get time as numbers
	void getTime(int &oYear, int &oMonth, int &oDay);

	//! Get preformated and localized time/date string
	Ogre::UTFString getTime(void);

	int getSpeedInt(void);

	//! Get current speed
	eSpeed getSpeed(void);

  //! set speed
	void setGameSpeed(eSpeed pSpeed);

	//! Update
	void update(float pElapsedTime);

	//! Serialize
	void serializeIntoXMLElement(TiXmlElement* pParentElement);

	//! Load
	void deserializeFromXMLElement(TiXmlNode* pParentNode);

private:

	//! time event handler
	void slower(EventData* pData);

	//! time event handler
	void faster(EventData* pData);

	//! time event handler
	void pause(EventData* pData);


	double mTimeHours; //!< Time passed in hours used to advance time
	int mYear; //!< current date
	int mMonth;
	int mDay;
	float mWeekCounter; //!< Used to raise "eWeekPassed" event
	eSpeed mSpeed; //!< Current speed
};

/*-----------------------------------------------------------------------------------------------*/

#endif // GAMETIME_H
