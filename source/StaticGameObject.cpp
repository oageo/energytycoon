#include "StdAfx.h"
#include "StaticGameObject.h"
#include "InstancedGeometryManager.h"
#include "Terrain.h"

/*-----------------------------------------------------------------------------------------------*/

const std::string cStaticObjectMesh[] = { "", "farm1.mesh", "house.mesh", "airplane.mesh",
								     "vehicle.mesh", "", "resource_coal.mesh" , 
									 "resource_gas.mesh", "resource_uranium.mesh" };

const Ogre::Vector3 cStaticObjectScale[] = { 
                     Ogre::Vector3(0.1, 0.1, 0.1), Ogre::Vector3(0.7, 0.7, 0.7),
									   Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(1.0, 1.0, 1.0), 
									   Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(1.0, 1.0, 1.0),
									   Ogre::Vector3(1.6, 1.6, 1.6), Ogre::Vector3(1.6, 1.6, 1.6), 
									   Ogre::Vector3(1.6, 1.6, 1.6)}; //!< Scales for meshes

const std::string cStaticName = "static";

/*-----------------------------------------------------------------------------------------------*/

StaticGameObject::StaticGameObject(Ogre::Vector2 pPosition, eStaticObjectType pType,
                                   Ogre::Rect pRect, bool pIsAutoremove)
: mRect(pRect),
  mIsAutoremove(pIsAutoremove),
  mPosition(pPosition),
  mType(pType)
{
}

/*-----------------------------------------------------------------------------------------------*/

void StaticGameObject::addToSceneManager(Ogre::SceneManager* pSceneManager,
                                         boost::shared_ptr< Terrain > pTerrain)
{
	mNode = pSceneManager->getRootSceneNode()->createChildSceneNode(
      cStaticName + getID());

  if (mType == eTree) {
    std::string lTreeType = toString((rand() % 2) + 1);
    mEntity = pSceneManager->createEntity(cStaticName + getID(),
      "tree" + lTreeType + ".mesh");

    mEntityFloor =  pSceneManager->createEntity(cStaticName + "ground" + getID(),
      "tree" + lTreeType + "_ground.mesh");
  } else {
    if (mType == eFarm) {
      std::string lFarmType = toString((rand() % 3) + 1);

      mEntity = pSceneManager->createEntity(cStaticName + getID(),
        "farm" + lFarmType + ".mesh");
      mEntityFloor = pSceneManager->createEntity(cStaticName 
        + "ground" + getID(), "farm" + lFarmType + "_ground.mesh");
      float lHeight = pTerrain->getHighestFromQuad(mPosition.x,
        mPosition.y, 42, true);

      if (lHeight <= 0.00001)  {
        // recompute height
      } else {
        pTerrain->flattenQuad(mPosition, 42, lHeight);
        pTerrain->update();
        mNode->attachObject(mEntityFloor);
      }
    } else {
      mEntity = pSceneManager->createEntity(cStaticName + getID(),
        cStaticObjectMesh[mType]);
    }
  }

	mEntity->setCastShadows(false);
	mNode->attachObject(mEntity);
	mNode->translate(Ogre::Vector3(mPosition.x, pTerrain->getHeightAt(mPosition.x,
      mPosition.y) + 0.2, mPosition.y));
	mNode->scale(cStaticObjectScale[mType]);
}

/*-----------------------------------------------------------------------------------------------*/

void StaticGameObject::removeFromSceneManager(void)
{
	mNode->detachAllObjects();
	mNode->getCreator()->destroyEntity(mEntity);

	if (mType == eFarm || mType == eTree)
		mNode->getCreator()->destroyEntity(mEntityFloor);
}

/*-----------------------------------------------------------------------------------------------*/

bool StaticGameObject::isAutoremove(void)
{
  return mIsAutoremove;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector< RotatedRect > StaticGameObject::getGroundRects(void)
{
  std::vector< RotatedRect > lTemp;
  lTemp.push_back(mRect);
  return lTemp;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::MovableObject* StaticGameObject::getMainEntityObject(void)
{
  return mEntity;
}

/*-----------------------------------------------------------------------------------------------*/

void StaticGameObject::setVisible(bool pVisible)
{
  if (mEntity) {
    if (mEntity->isInitialised() && mEntity->isAttached()) {
      mEntity->setVisible(pVisible);

      if ((mType == eTree) || (mType == eFarm))
        mEntityFloor->setVisible(pVisible);
    }
  }
}

/*-----------------------------------------------------------------------------------------------*/

eGameObjectType StaticGameObject::getType(void)
{
  return eStatic;
}

/*-----------------------------------------------------------------------------------------------*/

eCompanyObjectType StaticGameObject::getSubtype(void)
{
  return eNull;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 StaticGameObject::getPosition(void)
{
  return mPosition;
}

/*-----------------------------------------------------------------------------------------------*/

bool StaticGameObject::isSelectable(void)
{
  return false;
}

/*-----------------------------------------------------------------------------------------------*/

eStaticObjectType StaticGameObject::getSubType(void)
{
  return mType;
}

/*-----------------------------------------------------------------------------------------------*/

void StaticGameObject::rotate(float pTimeElapsed)
{
  mNode->rotate(Ogre::Vector3(0.0,1.0,0.0), Ogre::Radian(-pTimeElapsed * 1.3));
}