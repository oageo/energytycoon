#ifndef GAME_H
#define GAME_H

/*-----------------------------------------------------------------------------------------------*/

#include "Shared.h"
#include "Company.h"

/*-----------------------------------------------------------------------------------------------*/

class Map;
class PanCamera;
class GUI;
class EventData;
class GameTime;
class Collidable;
class Pole;
class PowerLine;

/*-----------------------------------------------------------------------------------------------*/

//! Possible gamestates
enum eGamestate { eSelectMode, eBuildMode, eDemolishMode, eGUIExclusiveMode };

/*-----------------------------------------------------------------------------------------------*/

const int cMaxLineLength = 42;	//!< Power net node <-> power net node
const float cDefaultColliderSize = 4.0;	//!< Select collider

/*-----------------------------------------------------------------------------------------------*/

//! Shortcut to get the resource for a resource gathering building
static eResourceType resourceFromBuilding(eCompanyObjectType pBuilding)
{
	if((pBuilding == eREUraniumLarge) || (pBuilding == eREUraniumSmall))
		return eUranium;
	else if((pBuilding == eREGasLarge) || (pBuilding == eREGasSmall))
		return eGas;
	else if((pBuilding == eRECoalLarge) || (pBuilding == eRECoalSmall))
		return eCoal;
	else
		return eNone;
}

/*-----------------------------------------------------------------------------------------------*/

//! Data to start/load a game
class StartData {
public:
	StartData()
    : mMission(""),
      mSerialized(false),
      mSerializedFile(""),
      mSandbox(false),
			mNew(false)
  {}

	std::string mMission; // Mission XML
	bool mSerialized; // Load?
	bool mNew;
	std::string mSerializedFile; // Savegame XML
	bool mSandbox; // Sandbox mode?
};

/*-----------------------------------------------------------------------------------------------*/

//! Floating info panel
class InfoPanel {
public:
	eGameObjectType mType;
	std::string mName;
	Ogre::Vector2 mPosition;
	bool mConnected;
};

/*-----------------------------------------------------------------------------------------------*/

//! The game logic and Listeners implementation
class Game: public Ogre::FrameListener,
			      public Ogre::WindowEventListener,
			      public OIS::MouseListener,
			      public OIS::KeyListener {
public:
	
	//! Start the game
	Game(Ogre::RenderWindow* pRenderWindow, Ogre::SceneManager* pSceneManager, Ogre::Root* pRoot,
		 bool pRestart = false);

	// Implement WindowEventListener
	void windowResized(Ogre::RenderWindow* pRenderWindow);
	void windowClosed(Ogre::RenderWindow* pRenderWindow);
	void windowFocusChange(Ogre::RenderWindow* pRenderWindow);

	// Implement FrameListener
	bool frameRenderingQueued(const Ogre::FrameEvent& pEvent);
	bool frameStarted(const Ogre::FrameEvent &pEvent);
	bool frameEnded(const Ogre::FrameEvent& pEvent);

	// Implement OIS::MouseListener
  bool mouseMoved(const OIS::MouseEvent &pEvent);
  bool mousePressed(const OIS::MouseEvent &pEvent, OIS::MouseButtonID pID);
  bool mouseReleased(const OIS::MouseEvent &pEvent, OIS::MouseButtonID pID);

	// Implement OIS::KeyListener
  bool keyPressed(const OIS::KeyEvent &pEvent);
  bool keyReleased(const OIS::KeyEvent &pEvent);

  float getDemoTime(void) { return mDemoTime; }
  void setDemoTime(float pTime) { mDemoTime = pTime; }

private:

	//! Change gamestate event handler
	void enableBuildMode(EventData* pData);

	//! Change gamestate event handler
	void enableDemolishMode(EventData* pData);

	//! Change gamestate event handler
	void enableSelectMode(EventData* pData);

	//! Change gamestate event handler
	void enableGUIMode(EventData* pData);

	//! Unload game
	void unload(EventData* pData);

	//! Serialize game
	void serialize(EventData* pData);

	//! Shutdown game, leave rendering queue
	void dismantle(EventData* pData);

	//! Load game
	void load(EventData* pData);

	//! Load map
	void loadMap(std::string pMapPrefix);

	//! Get current mouse <-> terrain intersection
	Ogre::Vector3 getMouseTerrainIntersection(void);

	//! Get current view (center of the screen) <-> terrain intersection
	Ogre::Vector2 getViewTerrainIntersetction(void);

	//! Update hovering text (e.g. city names)
	void updateHoverText(float pElapsedTime);

	//! Handle camera input from mouse/keyboard
	void handleMouseMovedMapView(const OIS::MouseEvent &pEvent);

  //! handle keyboard view keys
	void handleMapViewKeys(const Ogre::FrameEvent &pEvent);

	//! Save last mouse data for non OIS::MouseListener functions
	void updateLastMouseData(const OIS::MouseEvent &pEvent);

	//! Startup OIS
	void startInputSystem(void);

	//! Capture input
	void captureInput(void);

	//! Setup camera
	void setupCamera(void);

	//! Build a structure
	void buildCompanyObject(void);

	//! Demolish a structure
	void demolishCompanyObject(boost::shared_ptr<Collidable> pObject);

	//! Remove build entity (e.g. for Select gamestate)
	void destroyBuildEntity(void);

	//! check if suggested line intersects with other suggested lines
	bool intersectsBuildRequestLine(void);

	//! see Terrain class
	Ogre::Vector3 getQuadHighestTerrainIntersection(int pSideLength);

	//! Stop the current pan motion in a smooth way
	void smoothOutPan(float pElapsedTime);

	//! Run demolish particle system
	void runDemolishPart(float pElapsedTime);

	//! Get screen coordinates for floating info panel from entity
	Ogre::Vector2 getScreenCoordInfoPanel(Ogre::MovableObject* pEntity);

  //! Create demolish particle system
  void setupDemolishParticles(void);

	
	eGamestate mGamestate; //!< Current gamestate
	Ogre::Root* mRoot; //!< OGRE root
	Ogre::RenderWindow* mWindow; //!< OGRE render window
	Ogre::SceneManager *mSceneManager; //!< OGRE scene manager
	OIS::InputManager* mInputManager; //!< OIS stuff
	OIS::Mouse* mMouse;
	OIS::Keyboard* mKeyboard;
	Ogre::RaySceneQuery* mRaySceneQuery; //!< OGRE ray scene query (for intersections)
	Ogre::SceneNode *mBuildRequestNode; //!< OGRE node for build request
	Ogre::Vector2 mLastMousePos; //!< Saved mouse data
	Ogre::Vector2 mLastMouseSize;
	boost::shared_ptr<GUI> mGUI; //!< Game components
	boost::shared_ptr<GameTime> mGameTime;
	boost::shared_ptr<PanCamera> mPanCamera;
	boost::shared_ptr<Map> mMap;
	boost::shared_ptr<Company> mCompany;
	bool mBuildable; //!< Currently buildable?
	eCompanyObjectType mBuildType; //!< Type the player wants to build
	std::vector<boost::shared_ptr<Collidable> > mPoleNodes; //!< Poles in build queue
	std::vector<boost::shared_ptr<PowerLine> > mBuildLines; //!< Lines in build queue
	boost::shared_ptr<Collidable> mOverPoleNode; //!< Current attachable node (or 0)
	boost::shared_ptr<Collidable> mMouseOver; //!< Any current node
	bool mOverPoleConnectableNode; //!< see mOverPoleNode
	boost::shared_ptr<Highlightable> mHighlighted; //!< Current higlightable node
	boost::shared_ptr<PowerLine> mPowerLineSuggestion; //!< Current suggestion for powerline
	bool mDismantle; //!< Should the game be shutdown?
	bool mSandbox; //!< Sandbox mode?
	StartData mStartData; //!< Start data for the current game
	std::string mMission; //!< Mission name
	float mLastFrameTime; //!< Last frame duration
	bool mRightButtonDown;
	bool mSmoothOutPan; //!< Smooth out current pan (mouse only)
	Ogre::Vector2 mLastPanDelta;
	Ogre::ParticleSystem* mDemolishPartSys; //!< Demolish particle stuff
	Ogre::SceneNode* mDemolishPartNode;
	float mDemolishPartTime;
  float mDemoTime;
	bool mRunDemolishPart;
	int mCoalCounter; //!< Counter for resource building naming
	int mGasCounter;
	int mUraniumCounter;
	bool mRun;
	int mBuildLock;
  bool mDemoEventRaised;
};

/*-----------------------------------------------------------------------------------------------*/

#endif // GAME_H
