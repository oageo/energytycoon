#ifndef AUDIO_H
#define AUDIO_H

/*-----------------------------------------------------------------------------------------------*/

//! Possible effect types
enum eEffectType 
{
	eNothing
};

/*-----------------------------------------------------------------------------------------------*/

//! Handles all audio
class Audio  {
public:

	//! Start irrKlang
	static void start(void) 
	{
		mEngine = irrklang::createIrrKlangDevice();
	}

	//! Shut it down
	static void shutdown(void) 
	{
		mEngine->drop();
	}

	//! Start menu music
	static void setMenu(void) 
	{
		mEngine->play2D(GameConfig::getString("MenuMusic").c_str(), true);
	}

	//! Start game music
	static void setGame(void) 
	{
		mEngine->stopAllSounds();
		mEngine->play2D(GameConfig::getString("GameMusic").c_str(), true);
	}

	//! Play an effect
	static void playEffect(eEffectType pType) 
	{
	}

	//! Pause
	static void pause(bool pPause) 
	{
		mEngine->setAllSoundsPaused(pPause);
	}

private:

	static irrklang::ISoundEngine* mEngine; //!< irrKlang instance
};

/*-----------------------------------------------------------------------------------------------*/

#endif // AUDIO_H
