#ifndef WEATHER_H
#define WEATHER_H

/*-----------------------------------------------------------------------------------------------*/

enum eWindSpeed { eStrongSpeed = 0, eNormalSpeed = 1, eNoSpeed = 2}; //!< Wind speed
enum eSunshineDuration { eVeryLong = 0, eAverage = 1, eLow = 2}; //!< Sunshine duration

/*-----------------------------------------------------------------------------------------------*/

//! Weather type
class Weather {
public:

  //! Create new random weather
	Weather(void)
	{
		mWindSpeed = (eWindSpeed)(rand() % 3);
		mSunshineDuration = (eSunshineDuration)(rand() % 3);
	}


	eWindSpeed mWindSpeed; //!< Current wind speed
	eSunshineDuration mSunshineDuration; //!< Current sunshine duration
};

/*-----------------------------------------------------------------------------------------------*/

#endif // WEATHER_H