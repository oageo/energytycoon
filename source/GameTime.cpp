#include "StdAfx.h"
#include "GameTime.h"
#include "StringsLoc.h"
#include "Event.h"
#include "GUI.h"

/*-----------------------------------------------------------------------------------------------*/

GameTime::GameTime(void)
: mYear(1),
  mMonth(1),
  mDay(1),
  mSpeed(cDefaultSpeed),
  mTimeHours(24.0),
  mWeekCounter(0.0)
{
	REGISTER_CALLBACK(eGameSpeedSlower, GameTime::slower);
	REGISTER_CALLBACK(eGameSpeedFaster, GameTime::faster);
	REGISTER_CALLBACK(eGameSpeedPause, GameTime::pause);
}

/*-----------------------------------------------------------------------------------------------*/

void GameTime::getTime(int &oYear, int &oMonth, int &oDay)
{
	oYear = mYear;
	oMonth = mMonth;
	oDay = mDay;
}

/*-----------------------------------------------------------------------------------------------*/

void GameTime:: serializeIntoXMLElement(TiXmlElement* pParentElement)
{
	TiXmlElement* lGameTimeSerialized;
	lGameTimeSerialized = new TiXmlElement("gametime");
	lGameTimeSerialized->SetAttribute("year", mYear);
	lGameTimeSerialized->SetAttribute("month", mMonth);
	lGameTimeSerialized->SetAttribute("day", mDay);

	if (mSpeed == ePause)
		lGameTimeSerialized->SetAttribute("speed", 0);
	else if (mSpeed == eSlow)
		lGameTimeSerialized->SetAttribute("speed", 1);
	else if (mSpeed == eNormal)
		lGameTimeSerialized->SetAttribute("speed", 2);
	else if (mSpeed == eFast)
		lGameTimeSerialized->SetAttribute("speed", 3);

	pParentElement->LinkEndChild(lGameTimeSerialized);
}

/*-----------------------------------------------------------------------------------------------*/

void GameTime::deserializeFromXMLElement(TiXmlNode* pParentNode)
{
	TiXmlNode* lGameTimeNode = pParentNode->FirstChildElement("gametime");

	mYear = toNumber<int>(lGameTimeNode->ToElement()->Attribute("year"));
	mMonth = toNumber<int>(lGameTimeNode->ToElement()->Attribute("month"));
	mDay = toNumber<int>(lGameTimeNode->ToElement()->Attribute("day"));
	int lSpeed = toNumber<int>(lGameTimeNode->ToElement()->Attribute("speed"));

	mTimeHours = mDay * 24;

	if (lSpeed == 0)
		mSpeed = ePause;
	else if (lSpeed == 1)
		mSpeed = eSlow;
	else if (lSpeed == 2)
		mSpeed = eNormal;
	else if (lSpeed == 3)
		mSpeed = eFast;
}

/*-----------------------------------------------------------------------------------------------*/

eSpeed GameTime::getSpeed(void)
{
	return mSpeed;
}

/*-----------------------------------------------------------------------------------------------*/

int GameTime::getSpeedInt(void)
{
	if (mSpeed == ePause)
		return 0;
	else if (mSpeed == eSlow)
		return 1;
	else if (mSpeed == eNormal)
		return 2;
	else
		return 3;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::UTFString GameTime::getTime(void)
{
	std::wstringstream lTemp, lTemp2;
	std::wstring lReturn;

	lTemp << mYear;
	lReturn += StrLoc::get()->Year() + " " + lTemp.str();

	lTemp2 << mDay;
	lReturn += L", " + lTemp2.str() + L". ";

	switch (mMonth) {
	case 1:
		lReturn += StrLoc::get()->January();
		break;
	case 2:
		lReturn += StrLoc::get()->February();
		break;
	case 3:
		lReturn += StrLoc::get()->March();
		break;
	case 4:
		lReturn += StrLoc::get()->April();
		break;
	case 5:
		lReturn += StrLoc::get()->May();
		break;
	case 6:
		lReturn += StrLoc::get()->June();
		break;
	case 7:
		lReturn += StrLoc::get()->July();
		break;
	case 8:
		lReturn += StrLoc::get()->August();
		break;
	case 9:
		lReturn += StrLoc::get()->September();
		break;
	case 10:
		lReturn += StrLoc::get()->October();
		break;
	case 11:
		lReturn += StrLoc::get()->November();
		break;
	case 12:
		lReturn += StrLoc::get()->December();
		break;
	default:
		break;
	}
	
	return Ogre::UTFString(lReturn);
}

/*-----------------------------------------------------------------------------------------------*/

void GameTime::setGameSpeed(eSpeed pSpeed)
{
	mSpeed = pSpeed;
}

/*-----------------------------------------------------------------------------------------------*/

void GameTime::update(float pElapsedTime)
{
	int lSlowSpeedFactor = GameConfig::getInt("SlowSpeedFactor");
	int lNormalSpeedFactor = GameConfig::getInt("NormalSpeedFactor");
	int lFastSpeedFactor = GameConfig::getInt("FastSpeedFactor");

	// add time to hour (->day) and week counter
	if (mSpeed == eSlow) {
		mTimeHours += pElapsedTime * lSlowSpeedFactor;
		mWeekCounter += pElapsedTime * lSlowSpeedFactor / 24;
	}
	else if (mSpeed == eNormal) {
		mTimeHours += pElapsedTime * lNormalSpeedFactor;
		mWeekCounter += pElapsedTime * lNormalSpeedFactor / 24;
	}
	else if (mSpeed == eFast) {
		mTimeHours += pElapsedTime * lFastSpeedFactor;
		mWeekCounter += pElapsedTime * lFastSpeedFactor / 24;
	}
	
	int lDayPrev = mDay;
	mDay = mTimeHours/24;

	if(mWeekCounter >= 7.0) {
		mWeekCounter = 0;
		EventHandler::raiseEvent(eWeekPassed);
	}

	if (((mDay - 1) == lDayPrev) || (lDayPrev > mDay))	// check if day passed
    EventHandler::raiseEvent(eDayPassed);

	if ((mMonth == 1) || (mMonth == 3) || (mMonth == 5) ||
		  (mMonth == 7) || (mMonth == 8) || (mMonth == 10) || (mMonth == 12)) {
		if (mDay > 31)	{
			EventHandler::raiseEvent(eMonthPassed);

			if ((mMonth == 1) || (mMonth == 7)) {
				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->GameAutoSaved();

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
				EventHandler::raiseEvent(eSerializeGame, new EventArg<std::string>("autosave"));
			}

			mTimeHours = 24.0;
			mMonth++;

			if (mMonth > 12) {
				mMonth = 1;
				mYear++;

				EventHandler::raiseEvent(eYearPassed);
			}
		}
	}
	else if ((mMonth == 4) || (mMonth == 6) || (mMonth == 9) || (mMonth == 11)) {
		if (mDay > 30) {
			EventHandler::raiseEvent(eMonthPassed);

			mTimeHours = 24.0;
			mMonth++;
		}
	}
	else if (mMonth == 2) {
		if (mDay > 28)	{
			EventHandler::raiseEvent(eMonthPassed);

			mTimeHours = 24.0;
			mMonth = 3;
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void GameTime::slower(EventData* pData)
{
	if (mSpeed == eSlow)
		mSpeed = ePause;
	else if (mSpeed == eNormal)
		mSpeed = eSlow;
	else if (mSpeed == eFast)
		mSpeed = eNormal;
}

/*-----------------------------------------------------------------------------------------------*/

void GameTime::faster(EventData* pData)
{
	if (mSpeed == ePause)
		mSpeed = eSlow;
	else if (mSpeed == eSlow)
		mSpeed = eNormal;
	else if (mSpeed == eNormal)
		mSpeed = eFast;
}

/*-----------------------------------------------------------------------------------------------*/

void GameTime::pause(EventData* pData)
{
	mSpeed = ePause;
}
