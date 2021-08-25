#ifndef PANCAMERA_H
#define PANCAMERA_H

/*-----------------------------------------------------------------------------------------------*/

class EventData;

/*-----------------------------------------------------------------------------------------------*/

const float cMouseWheelZoomSteps = 450.0f; //!< Mousewheel zoom speed
const float cMouseRotationSteps = 175.0f; //!< Mouse rotation speed

/*-----------------------------------------------------------------------------------------------*/

//! Smooth move 'unit'
class SmoothMove {
public:

  //! Create with default values
	SmoothMove(void) :
	  mAmount(0.0), mTimePassed(0.0)
	{}

	float mAmount; //!< fixed
	float mTimePassed; //!< time passed
};

/*-----------------------------------------------------------------------------------------------*/

//! Camera for the game
class PanCamera {
public:

	//! Create it
	PanCamera(Ogre::SceneManager* pSceneManager, Ogre::RenderWindow* pRenderWindow);

	//! Set the aspect ratio
	void setAspectRatio(float pRatio);

	//! Set the map borders
	void setMapSize(float pTerrainWidth, float pTerrainHeight);

	//! move up
	void moveUp(float pTimeElapsed);

	//! move down
	void moveDown(float pTimeElapsed);

	//! move left
	void moveLeft(float pTimeElapsed);

	//! move right
	void moveRight(float pTimeElapsed);

	//! zoom in
	void zoomIn(float pTimeElapsed);

	//! zoom out
	void zoomOut(float pTimeElapsed);

	//! rotate left
	void rotateLeft(float pTimeElapsed);

	//! rotate right
	void rotateRight(float pTimeElapsed);

	//! Return OGRE camera ptr
	Ogre::Camera* getCam(void);

	//! Get current height
	float getHeight(void);

	//! update camera (smooth out moves)
	void update(float pTimeElapsed);

	//! add a zoom unit to the stack
	void addZoom(float pTimeElapsed);

	//! add a rotate unit to the stack
	void addRotate(float pTimeElapsed);

private:

	//! move to event (e.g. message ticker click)
	void eventMoveTo(EventData* pData);

  //! zoom event (main panel)
	void eventZoom(EventData* pData);

  //! rotate event (main panel)
	void eventRotate(EventData* pData);

	//! Event handler if graphic options change
	void graphicOptionsChange(EventData* pData);


	Ogre::Camera* mCamera; //!< OGRE camera
	Ogre::SceneNode* mCameraNode; //!< OGRE node
	Ogre::RenderWindow* mWindow; //!< OGRE render window
	int mTerrainWidth; //!< Terrain width used in game logic
	int mTerrainHeight; //!< Terrain height
	std::vector<SmoothMove> mZoomers; //!< zoom stack
	std::vector<SmoothMove> mRotators; //!< rotate stack
};

/*-----------------------------------------------------------------------------------------------*/

#endif // PANCAMERA_H
