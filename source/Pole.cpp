#include "StdAfx.h"
#include "Pole.h"
#include "Terrain.h"

/*-----------------------------------------------------------------------------------------------*/

Pole::Pole(Ogre::Vector2 pPosition, eCompanyObjectType pType)
: mPosition(pPosition),
  mType(pType),
  mCurrentStrain(0),
  mIntersection(false),
	mAttachingCounter(0),
  mHeight(400.0),
  mDestroyed(false)
{
	if ((mType == eDIPoleLarge) || (mType == eDIIntersectionLarge))
		mCapacity = GameConfig::getInt("PoleLargeCapacity");

	if ((mType == eDIPoleSmall) || (mType == eDIIntersectionSmall))
		mCapacity = GameConfig::getInt("PoleSmallCapacity");
}

/*-----------------------------------------------------------------------------------------------*/

void Pole::reset(void)
{
	if ((mType == eDIPoleLarge) || (mType == eDIIntersectionLarge))
		mCapacity = GameConfig::getInt("PoleLargeCapacity");

	if ((mType == eDIPoleSmall) || (mType == eDIIntersectionSmall))
		mCapacity = GameConfig::getInt("PoleSmallCapacity");

	mCurrentStrain = 0;
	mAttachingCounter = 0;

	showIntersection(false);
}

/*-----------------------------------------------------------------------------------------------*/

void Pole::showIntersection(bool pShow)
{
	if (pShow) {
		mShowIntersection = true;
		mNode->detachAllObjects();
		mNode->getCreator()->destroyEntity(mEntity);

		if (mHeight < 0.12) {
			mEntity = mSceneManager->createEntity("pole_w_w" + getID(), "kabelk.mesh");
			mNode->setScale(1.8,1.8,1.8);
		} else {
			if (mType == eDIPoleLarge) {
				mEntity = mSceneManager->createEntity("pole_i_l_" + getID(),
					"strommastkreuzung.mesh");
				mEntity->getMesh()->buildEdgeList();
				mNode->setScale(0.1,0.1,0.1);
			}
			else if (mType == eDIPoleSmall) {
				mEntity = mSceneManager->createEntity("pole_i_s_" + getID(),
					"strommastkleinkreuzung.mesh");
				mEntity->getMesh()->buildEdgeList();
				mNode->setScale(1.0,1.0,1.0);
			}
		}

		mNode->attachObject(mEntity);
	} else {
		mShowIntersection = false;
		mNode->detachAllObjects();
		mNode->getCreator()->destroyEntity(mEntity);

		if (mHeight < 0.12) {
			mEntity = mSceneManager->createEntity("pole_w_w" + getID(), "kabelk.mesh");
			mNode->setScale(1.8,1.8,1.8);
		} else {
			if (mType == eDIPoleLarge) {
				mEntity = mSceneManager->createEntity("pole_n_l" + getID(),
					"strommast.mesh");
				mEntity->getMesh()->buildEdgeList();
				mNode->setScale(0.1,0.1,0.1);
			}
			else if (mType == eDIPoleSmall) {
				mEntity = mSceneManager->createEntity("pole_n_s" + getID(),
					"strommastklein.mesh");
				mEntity->getMesh()->buildEdgeList();
				mNode->setScale(1.0,1.0,1.0);
			}
		}

		mNode->attachObject(mEntity);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Pole::changeIntoIntersection(void)
{
	mIntersection = true;
}

/*-----------------------------------------------------------------------------------------------*/

void Pole::removeIntersection(void)
{
	mIntersection = false;
}

/*-----------------------------------------------------------------------------------------------*/

void Pole::setRotation(Ogre::Radian pRotation)
{
	mCurrentRotation = pRotation;
	mNode->resetOrientation();
	mNode->rotate(Ogre::Vector3(0.0,1.0,0.0), pRotation);
}

/*-----------------------------------------------------------------------------------------------*/

void Pole::setType(eCompanyObjectType pType)
{

}

/*-----------------------------------------------------------------------------------------------*/

std::vector< Ogre::Vector3 > Pole::getLineAttachingPoints(Ogre::Vector2 pAdjacentVertexPosition,
											 bool pCheckOnly)
{
	if (!pCheckOnly)
		mAttachingCounter++;

	std::vector< Ogre::Vector3 > lAttachingPoints;
	Ogre::Vector3 lPos;

	bool mAttachToIntersection = false;

	Ogre::Radian lAngle;

	Ogre::Vector2 lToAdjacent, lDirection;

	Ogre::Radian lRealRotation;

	if(mCurrentRotation > Ogre::Radian(Ogre::Math::HALF_PI))
		lRealRotation = mCurrentRotation - Ogre::Radian(Ogre::Math::HALF_PI);
	else
		lRealRotation = mCurrentRotation + Ogre::Radian(Ogre::Math::HALF_PI);

	lToAdjacent = (pAdjacentVertexPosition - mPosition).normalisedCopy();
	lDirection.x = Ogre::Math::Cos(lRealRotation.valueRadians());
	lDirection.y = Ogre::Math::Sin(lRealRotation.valueRadians());
	lDirection.y *= -1;
	lDirection.normalise();

	lAngle = Ogre::Radian(Ogre::Math::ACos(lToAdjacent.dotProduct(lDirection)));

	if ((lAngle > Ogre::Radian(Ogre::Math::PI/4.0)) 
    && (lAngle < Ogre::Radian((3.0*Ogre::Math::PI)/4.0))) {
		if (mIntersection) {
			mAttachToIntersection = true;
			showIntersection(true);
		}
	}
	
	if (mType == eDIPoleLarge) {
		if (mAttachToIntersection && mIntersection) {
			lPos = mNode->getPosition();
			lPos.y += 5;
			lPos.x += (Ogre::Math::Cos(-mCurrentRotation + Ogre::Radian(Ogre::Math::PI/2)) * 5);
			lPos.z += (Ogre::Math::Sin(-mCurrentRotation + Ogre::Radian(Ogre::Math::PI/2)) * 5);

			lAttachingPoints.push_back(lPos);

			lPos = mNode->getPosition();
			lPos.y += 5;
			lPos.x += (Ogre::Math::Cos(-mCurrentRotation + Ogre::Radian(Ogre::Math::PI/2)) * 2.5);
			lPos.z += (Ogre::Math::Sin(-mCurrentRotation + Ogre::Radian(Ogre::Math::PI/2)) * 2.5);

			lAttachingPoints.push_back(lPos);

			//////////////////////////////////////

			lPos = mNode->getPosition();
			lPos.y += 5;
			lPos.x += (Ogre::Math::Cos(-mCurrentRotation + Ogre::Radian((3*Ogre::Math::PI)/2)) * 5);
			lPos.z += (Ogre::Math::Sin(-mCurrentRotation + Ogre::Radian((3*Ogre::Math::PI)/2)) * 5);

			lAttachingPoints.push_back(lPos);

			lPos = mNode->getPosition();
			lPos.y += 5;
			lPos.x += (Ogre::Math::Cos(-mCurrentRotation + Ogre::Radian((3*Ogre::Math::PI)/2)) * 2.5);
			lPos.z += (Ogre::Math::Sin(-mCurrentRotation + Ogre::Radian((3*Ogre::Math::PI)/2)) * 2.5);

			lAttachingPoints.push_back(lPos);
		} else {
			lPos = mNode->getPosition();
			lPos.y += 7;
			lPos.x += (Ogre::Math::Cos(-mCurrentRotation) * 5);
			lPos.z += (Ogre::Math::Sin(-mCurrentRotation) * 5);

			lAttachingPoints.push_back(lPos);

			lPos = mNode->getPosition();
			lPos.y += 10;
			lPos.x += (Ogre::Math::Cos(-mCurrentRotation) * 2.5);
			lPos.z += (Ogre::Math::Sin(-mCurrentRotation) * 2.5);

			lAttachingPoints.push_back(lPos);

			////////////////////////////////////// standard order: 

			lPos = mNode->getPosition();
			lPos.y += 7;
			lPos.x += (Ogre::Math::Cos(-mCurrentRotation + Ogre::Radian(Ogre::Math::PI)) * 5);
			lPos.z += (Ogre::Math::Sin(-mCurrentRotation + Ogre::Radian(Ogre::Math::PI)) * 5);

			lAttachingPoints.push_back(lPos);

			lPos = mNode->getPosition();
			lPos.y += 10;
			lPos.x += (Ogre::Math::Cos(-mCurrentRotation + Ogre::Radian(Ogre::Math::PI)) * 2.5);
			lPos.z += (Ogre::Math::Sin(-mCurrentRotation + Ogre::Radian(Ogre::Math::PI)) * 2.5);

			lAttachingPoints.push_back(lPos);
		}
	} else {
		if (mAttachToIntersection && mIntersection) {
			lPos = mNode->getPosition();
			lPos.y += 4.5;
			lPos.x += (Ogre::Math::Cos(-mCurrentRotation + Ogre::Radian(Ogre::Math::PI/2)) * 2.2);
			lPos.z += (Ogre::Math::Sin(-mCurrentRotation + Ogre::Radian(Ogre::Math::PI/2)) * 2.2);

			lAttachingPoints.push_back(lPos);

			lPos = mNode->getPosition();
			lPos.y += 4.5;
			lPos.x += (Ogre::Math::Cos(-mCurrentRotation + Ogre::Radian((3*Ogre::Math::PI)/2)) * 2.2);
			lPos.z += (Ogre::Math::Sin(-mCurrentRotation + Ogre::Radian((3*Ogre::Math::PI)/2)) * 2.2);

			lAttachingPoints.push_back(lPos);
		} else {
			lPos = mNode->getPosition();
			lPos.y += 6;
			lPos.x += (Ogre::Math::Cos(-mCurrentRotation) * 2.2);
			lPos.z += (Ogre::Math::Sin(-mCurrentRotation) * 2.2);

			lAttachingPoints.push_back(lPos);

			lPos = mNode->getPosition();
			lPos.y += 6;
			lPos.x += (Ogre::Math::Cos(-mCurrentRotation + Ogre::Radian(Ogre::Math::PI)) * 2.2);
			lPos.z += (Ogre::Math::Sin(-mCurrentRotation + Ogre::Radian(Ogre::Math::PI)) * 2.2);

			lAttachingPoints.push_back(lPos);
		}
	}

	return lAttachingPoints;
}

/*-----------------------------------------------------------------------------------------------*/

size_t Pole::getAttachedCount(void)
{
	if (mType == eDIPoleLarge) {
		if (mAttachingCounter <= 8)
			return 1;
		else if((mAttachingCounter > 8) && (mAttachingCounter <= 16))
			return 2;
		else if((mAttachingCounter > 16) && (mAttachingCounter <= 24))
			return 3;
		else
			return 4;
	} else {
		if(mAttachingCounter <= 4)
			return 1;
		else if((mAttachingCounter > 4) && (mAttachingCounter <= 8))
			return 2;
		else if((mAttachingCounter > 8) && (mAttachingCounter <= 12))
			return 3;
		else
			return 4;
	}
}

/*-----------------------------------------------------------------------------------------------*/

int Pole::getCapacity(void)
{
	if ((mType == eDIPoleLarge) || (mType == eDIIntersectionLarge))
		return GameConfig::getInt("PoleLargeCapacity");
	else
		return GameConfig::getInt("PoleSmallCapacity");
}

/*-----------------------------------------------------------------------------------------------*/

void Pole::addStrain(int pAmount)
{
	mCurrentStrain += pAmount;
}

/*-----------------------------------------------------------------------------------------------*/

int Pole::getCapacityLeft(void)
{
	return (mCapacity - mCurrentStrain);
}

/*-----------------------------------------------------------------------------------------------*/

bool Pole::isAutoremove(void)
{
	return false;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector <RotatedRect > Pole::getGroundRects()
{
	std::vector< RotatedRect > lTemp;
	lTemp.push_back(RotatedRect(rectFromPoint2(mPosition, 8)));
	return lTemp;
}

/*-----------------------------------------------------------------------------------------------*/

void Pole::addToSceneManager(Ogre::SceneManager* pSceneManager,
							 boost::shared_ptr< Terrain > pTerrain)
{
	mHeight = pTerrain->getHeightAt(mPosition.x,mPosition.y);

	mSceneManager = pSceneManager;

	if (mHeight < 0.12) {
		mEntity = mSceneManager->createEntity("pole_w_w" + getID(), "kabelk.mesh");
	} else {
		if (mType == eDIPoleLarge) {
			mEntity = pSceneManager->createEntity("pole_l_" + getID(),
				"strommast.mesh");
			mEntity->getMesh()->buildEdgeList();
		}
		else if (mType == eDIPoleSmall) {
			mEntity = pSceneManager->createEntity("pole_s_" + getID(),
				"strommastklein.mesh");
			mEntity->getMesh()->buildEdgeList();
		}
	}

	mNode = pSceneManager->getRootSceneNode()->createChildSceneNode("pole_node_" + getID());

	mNode->attachObject(mEntity);
	mNode->translate(Ogre::Vector3( mPosition.x, 
		(pTerrain->getHeightAt(mPosition.x,mPosition.y)+cFloorDistance + 1),mPosition.y));
	mNode->scale(0.1,0.1,0.1);
}

/*-----------------------------------------------------------------------------------------------*/

void Pole::removeFromSceneManager(void)
{
	mNode->detachAllObjects();
	mNode->getCreator()->destroyEntity(mEntity);

	mDestroyed = true;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 Pole::getPosition(void)
{
	return mPosition;
}

/*-----------------------------------------------------------------------------------------------*/

bool Pole::isSelectable(void)
{
	return false;
}

/*-----------------------------------------------------------------------------------------------*/

eGameObjectType Pole::getType(void)
{
	return ePole;
}

/*-----------------------------------------------------------------------------------------------*/

void Pole::highlight(bool pHighlight)
{
	if (mDestroyed)
		return;

	if (!mEntity)
		return;

	if (pHighlight) {
		mEntity->setMaterialName("highlight");
	} else {
		if(mHeight < 0.12) {
			mEntity->setMaterialName("kabelk");
		} else {
			if(mType == eDIPoleSmall) {
				if (mShowIntersection)
					mEntity->setMaterialName("strommastkleinkreuzung");
				else
					mEntity->setMaterialName("strommastklein");
			} else {
				if (mShowIntersection)
					mEntity->setMaterialName("strommastkreuzung");
				else
					mEntity->setMaterialName("strommast");
			}
		}
	}
}