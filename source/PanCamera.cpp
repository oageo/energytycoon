#include "StdAfx.h"
#include "PanCamera.h"
#include "Event.h"

/*-----------------------------------------------------------------------------------------------*/

const float cScrollSpeed = 150.0f; //!< Keyboard scrolling speed
const float cRotateSpeed = 255.0f; //!< Keyboard rotating speed
const Ogre::Vector3 cDefaultStartPosition = Ogre::Vector3(512,249,1536); // start position

/*-----------------------------------------------------------------------------------------------*/

PanCamera::PanCamera(Ogre::SceneManager *pSceneManager, Ogre::RenderWindow* pWindow)
: mTerrainHeight(0),
  mTerrainWidth(0),
  mWindow(pWindow)
{
	mCamera = pSceneManager->createCamera("PanCam");
	mCamera->setPosition(cDefaultStartPosition);
	mCamera->pitch(Ogre::Radian(-Ogre::Math::PI/3));
	mCamera->setNearClipDistance(1);

	mCamera->setFarClipDistance(400);

	mWindow->addViewport(mCamera);
	mWindow->getViewport(0)->setBackgroundColour(Ogre::ColourValue(0,0,0));
	setAspectRatio( (float)pWindow->getWidth()/ (float)pWindow->getHeight());

	REGISTER_CALLBACK(eMoveCamera, PanCamera::eventMoveTo);
	REGISTER_CALLBACK(eCameraZoom, PanCamera::eventZoom);
	REGISTER_CALLBACK(eCameraRotate, PanCamera::eventRotate);
	REGISTER_CALLBACK(eGraphicOptionsChange, PanCamera::graphicOptionsChange);

	rotateLeft(-0.3);
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::graphicOptionsChange(EventData* pData)
{
	setAspectRatio((float)mWindow->getWidth()/ (float)mWindow->getHeight());
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::moveUp(float pTimeElapsed)
{
	float lScaleY = pTimeElapsed * cScrollSpeed;
	float lScaleZ = -pTimeElapsed * cScrollSpeed * tan(Ogre::Math::PI/6);

	mCamera->moveRelative(Ogre::Vector3(0, lScaleY, lScaleZ));

	Ogre::Vector3 lCamPos = mCamera->getPosition();

	if (!(   (lCamPos.x > 0.0)
		 && (lCamPos.x < mTerrainWidth)
		 && (lCamPos.z > 0.0)
		 && (lCamPos.z < mTerrainHeight))) {
		mCamera->moveRelative(Ogre::Vector3(0, -lScaleY, -lScaleZ));
	}
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::moveDown(float pTimeElapsed)
{
	float lScaleY = -pTimeElapsed * cScrollSpeed;
	float lScaleZ = pTimeElapsed * cScrollSpeed * tan(Ogre::Math::PI/6);

	mCamera->moveRelative(Ogre::Vector3(0, lScaleY, lScaleZ));

	Ogre::Vector3 lCamPos = mCamera->getPosition();

	if (!(  (lCamPos.x > 0.0)
		&& (lCamPos.x < mTerrainWidth)
		&& (lCamPos.z > 0.0)
		&& (lCamPos.z < mTerrainHeight))) {
		mCamera->moveRelative(Ogre::Vector3(0, -lScaleY, -lScaleZ));
	}
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::moveLeft(float pTimeElapsed)
{
	float lScaleX = -pTimeElapsed * cScrollSpeed;

	mCamera->moveRelative(Ogre::Vector3(lScaleX, 0, 0));

	Ogre::Vector3 lCamPos = mCamera->getPosition();

	if (!(  (lCamPos.x > 0.0)
		&& (lCamPos.x < mTerrainWidth)
		&& (lCamPos.z > 0.0)
		&& (lCamPos.z < mTerrainHeight))) {
		mCamera->moveRelative(Ogre::Vector3(-lScaleX, 0, 0));
	}
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::moveRight(float pTimeElapsed)
{
	float lScaleX = pTimeElapsed * cScrollSpeed;

	mCamera->moveRelative(Ogre::Vector3(lScaleX, 0, 0));

	Ogre::Vector3 lCamPos = mCamera->getPosition();

	if(!(  (lCamPos.x > 0.0)
		&& (lCamPos.x < mTerrainWidth)
		&& (lCamPos.z > 0.0)
		&& (lCamPos.z < mTerrainHeight))) {
		mCamera->moveRelative(Ogre::Vector3(-lScaleX, 0, 0));
	}
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::rotateLeft(float pTimeElapsed)
{
	mCamera->rotate(Ogre::Vector3(0,1,0), Ogre::Radian(pTimeElapsed));
	moveRight(pTimeElapsed * mCamera->getPosition().y / cRotateSpeed);
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::rotateRight(float pTimeElapsed)
{
	mCamera->rotate(Ogre::Vector3(0,1,0), Ogre::Radian(-pTimeElapsed));
	moveLeft(pTimeElapsed * mCamera->getPosition().y / cRotateSpeed);
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::zoomOut(float pTimeElapsed)
{
	if ((mCamera->getPosition() + (mCamera->getUp() 
		* (pTimeElapsed * cScrollSpeed))).y <= GameConfig::getDouble("MaxZoomOut")) {
		mCamera->moveRelative(Ogre::Vector3(0, 0, pTimeElapsed * cScrollSpeed));

		Ogre::Vector3 lCamPos = mCamera->getPosition();

		if (!(   (lCamPos.x > 0.0)
			&& (lCamPos.x < mTerrainWidth)
			&& (lCamPos.z > 0.0)
			&& (lCamPos.z < mTerrainHeight))) {
			mCamera->moveRelative(Ogre::Vector3(0, 0, -pTimeElapsed * cScrollSpeed));
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::zoomIn(float pTimeElapsed)
{
	if((mCamera->getPosition() + (mCamera->getUp()*(-pTimeElapsed 
		* cScrollSpeed))).y >= GameConfig::getDouble("MaxZoomIn")) {
		mCamera->moveRelative(Ogre::Vector3(0, 0, -pTimeElapsed * cScrollSpeed));
	}

	Ogre::Vector3 lCamPos = mCamera->getPosition();

	if (!(   (lCamPos.x > 0.0)
		&& (lCamPos.x < mTerrainWidth)
		&& (lCamPos.z > 0.0)
		&& (lCamPos.z < mTerrainHeight))) {
		mCamera->moveRelative(Ogre::Vector3(0, 0, pTimeElapsed * cScrollSpeed));
	}
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::addRotate(float pTimeElapsed)
{
	SmoothMove lRotate;

	if (pTimeElapsed > 0)
		lRotate.mAmount = 0.4;
	else
		lRotate.mAmount = -0.4;

	lRotate.mTimePassed = 0.0;

	mRotators.push_back(lRotate);
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::addZoom(float pTimeElapsed)
{
	SmoothMove lZoom;

	if (pTimeElapsed > 0)
		lZoom.mAmount = 0.4;
	else
		lZoom.mAmount = -0.4;

	lZoom.mTimePassed = 0.0;

	mZoomers.push_back(lZoom);
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::update(float pTimeElapsed)
{
	std::vector<SmoothMove>::iterator it = mZoomers.begin();

	while (it != mZoomers.end()) {
		it->mTimePassed += pTimeElapsed;

		float lAmountNormalized;

		if(it->mAmount < 0.0)
			lAmountNormalized = it->mAmount * -1.0;
		else
			lAmountNormalized = it->mAmount;


		float lPart = pTimeElapsed / (lAmountNormalized*lAmountNormalized);

		float lFrameMove = ( ((lAmountNormalized/2.0) * (lAmountNormalized/2.0))- 
			(((lAmountNormalized/2.0) - it->mTimePassed) 
			* ((lAmountNormalized/2.0) - it->mTimePassed)))
			* lPart;

		if(lFrameMove < 0)
			lFrameMove *= -1;

		if(it->mAmount > 0.0)
			zoomIn(lFrameMove*2.5);
		else
			zoomOut(lFrameMove*2.5);

		float lCheck;

		if(it->mAmount < 0.0)
			lCheck = it->mAmount * -1.0;
		else
			lCheck = it->mAmount;

		if(it->mTimePassed >= lCheck)
			it = mZoomers.erase(it);
		else
			it++;
	}

	it = mRotators.begin();

	while (it != mRotators.end()) {
		it->mTimePassed += pTimeElapsed;

		float lAmountNormalized;

		if(it->mAmount < 0.0)
			lAmountNormalized = it->mAmount * -1.0;
		else
			lAmountNormalized = it->mAmount;


		float lPart = pTimeElapsed / (lAmountNormalized*lAmountNormalized);

		float lFrameMove = ( ((lAmountNormalized/2.0) * (lAmountNormalized/2.0))- 
			(((lAmountNormalized/2.0) - it->mTimePassed) * ((lAmountNormalized/2.0)
			- it->mTimePassed)))
			* lPart;

		if(lFrameMove < 0)
			lFrameMove *= -1;

		if(it->mAmount > 0.0)
			rotateLeft(lFrameMove*3.0);
		else
			rotateRight(lFrameMove*3.0);

		float lCheck;

		if(it->mAmount < 0.0)
			lCheck = it->mAmount * -1.0;
		else
			lCheck = it->mAmount;

		if(it->mTimePassed >= lCheck)
			it = mRotators.erase(it);
		else
			it++;
	}
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Camera* PanCamera::getCam(void)
{
	return mCamera;
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::setAspectRatio(float pRatio)
{
	mCamera->setAspectRatio(pRatio);
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::setMapSize(float pTerrainWidth, float pTerrainHeight)
{
	mTerrainWidth = pTerrainWidth;
	mTerrainHeight = pTerrainHeight;
}

/*-----------------------------------------------------------------------------------------------*/

float PanCamera::getHeight(void)
{
	return mCamera->getPosition().y;
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::eventMoveTo(EventData* pData)
{
	if((static_cast< EventArg< int >*>(pData)->mData1 != -1) &&
		(static_cast< EventArg< int >*>(pData)->mData2 != -1)) {
		Ogre::Vector3 lMoveTo;

		lMoveTo.x = static_cast< EventArg< int >* >(pData)->mData1;
		lMoveTo.y = 0;
		lMoveTo.z = static_cast <EventArg< int >* >(pData)->mData2;

		float lOldHeight = mCamera->getPosition().y;

		mCamera->setPosition(lMoveTo);

		while (lOldHeight > mCamera->getPosition().y) {
			mCamera->moveRelative(Ogre::Vector3(0,0,0.5));
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::eventZoom(EventData* pData)
{
	if(static_cast< EventArg< int >* >(pData)->mData1 == 1)
		addZoom(-1.0);
	else
		addZoom(1.0);
}

/*-----------------------------------------------------------------------------------------------*/

void PanCamera::eventRotate(EventData* pData)
{
	if(static_cast< EventArg< int >* >(pData)->mData1 == 1)
		addRotate(1.0);
	else
		addRotate(-1.0);
}
