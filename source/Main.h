#ifndef MAIN_H
#define MAIN_H

/*-----------------------------------------------------------------------------------------------*/

#include "Game.h"

/*-----------------------------------------------------------------------------------------------*/

//! The main application
class Application {
public:

	//! Create it
  Application(void);

	//! Shut it down
  ~Application(void);

	//! Run it
  void go(void);

private:

	//! Setup everything
  void setup(void);

	//! Setup resources (paths)
  void setupResources(void);

	//! Load resources
	void loadResources(void);

	//! Parse video settings from file
	void parseSettings(void);

	//! Restart event handler
	void restartGame(EventData* pData);

	//! Start render queue
	void startRendering(void);

	Ogre::Root *mRoot; //!< OGRE root
  Ogre::RenderWindow* mWindow; //!< OGRE render window
  Ogre::SceneManager* mSceneManager; //!< OGRE scene manager
	boost::shared_ptr<Game> mGame; //!< The main game object
	int mLastWindowWidth;
	int mLastWindowHeight;
	size_t mLastBPP;
	bool mLastFullscreen;
	bool mLastGraphicsQuality;
	bool mDebug; //!< Debug (->GL) mode?
	bool mRestart; //!< Restart imminent? (-> leave render queue)
	StartData mStartData; //!< Start information for new game
};

/*-----------------------------------------------------------------------------------------------*/

#endif // MAIN_H
