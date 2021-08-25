#include "StdAfx.h"
#include "TransformatorSet.h"

/*-----------------------------------------------------------------------------------------------*/

const Ogre::Vector3 cHeight = Ogre::Vector3(0.0, 5.0, 0.0); //!< Default height

/*-----------------------------------------------------------------------------------------------*/

TransformatorSet::TransformatorSet(Ogre::SceneManager* pSceneManager, Ogre::Vector3 pNodeCenter, 
                                   eTransformatorType pType, eCompanyObjectType pSubType)
: mSceneManager(pSceneManager),
  mNodeCenter(pNodeCenter),
  mType(pType)
{
  if (pType == eCityTrans) {
    mTransformator = pSceneManager->createEntity("trans1" + getID(), "trafo.mesh");

    mOffsetSouthEast = GameConfig::getVector3("OffsetSECity");
    mOffsetSouthWest = GameConfig::getVector3("OffsetSWCity");
    mOffsetNorthEast = GameConfig::getVector3("OffsetNECity");
    mOffsetNorthWest = GameConfig::getVector3("OffsetNWCity");
  } else {
    mTransformator = pSceneManager->createEntity("trans1" + getID(), "umspann.mesh");

    mOffsetSouthEast = GameConfig::getVector3("OffsetSEPP");
    mOffsetSouthWest = GameConfig::getVector3("OffsetSWPP");
    mOffsetNorthEast = GameConfig::getVector3("OffsetNEPP");
    mOffsetNorthWest = GameConfig::getVector3("OffsetNWPP");

    if (GameConfig::hasKey(GameConfig::cCOTS(pSubType) + "_OffsetSE"))
      mOffsetSouthEast *= GameConfig::getDouble(GameConfig::cCOTS(pSubType) + "_OffsetSE");

    if (GameConfig::hasKey(GameConfig::cCOTS(pSubType) + "_OffsetSW"))
      mOffsetSouthWest *= GameConfig::getDouble(GameConfig::cCOTS(pSubType) + "_OffsetSW");

    if (GameConfig::hasKey(GameConfig::cCOTS(pSubType) + "_OffsetNE"))
      mOffsetNorthEast *= GameConfig::getDouble(GameConfig::cCOTS(pSubType) + "_OffsetNE");

    if (GameConfig::hasKey(GameConfig::cCOTS(pSubType) + "_OffsetNW"))
      mOffsetNorthWest *= GameConfig::getDouble(GameConfig::cCOTS(pSubType) + "_OffsetNW");
  }

  mSouthEast = pSceneManager->getRootSceneNode()
    ->createChildSceneNode("transnode1" + getID());
  mSouthWest = pSceneManager->getRootSceneNode()
    ->createChildSceneNode("transnode2" + getID());
  mNorthEast = pSceneManager->getRootSceneNode()
    ->createChildSceneNode("transnode3" + getID());
  mNorthWest = pSceneManager->getRootSceneNode()
    ->createChildSceneNode("transnode4" + getID());

  mSouthEast->attachObject(mTransformator);
  mSouthWest->attachObject(mTransformator->clone("trans2" + getID()));
  mNorthEast->attachObject(mTransformator->clone("trans3" + getID()));
  mNorthWest->attachObject(mTransformator->clone("trans4" + getID()));

  mSouthEast->translate(pNodeCenter + mOffsetSouthEast);
  mSouthWest->translate(pNodeCenter + mOffsetSouthWest);
  mNorthEast->translate(pNodeCenter + mOffsetNorthEast);
  mNorthWest->translate(pNodeCenter + mOffsetNorthWest);

  if (pType == eCityTrans) {
    mSouthEast->scale(Ogre::Vector3(0.10, 0.12, 0.10));
    mSouthWest->scale(Ogre::Vector3(0.10, 0.12, 0.10));
    mNorthEast->scale(Ogre::Vector3(0.10, 0.12, 0.10));
    mNorthWest->scale(Ogre::Vector3(0.10, 0.12, 0.10));
  }
  else if (pType == ePowerplantTrans) {
    mNorthEast->rotate(Ogre::Vector3(0.0, 1.0, 0.0), Ogre::Radian(Ogre::Math::PI/2.0));
    mNorthWest->rotate(Ogre::Vector3(0.0, 1.0, 0.0), Ogre::Radian(Ogre::Math::PI/2.0));
  }

  mSouthEast->setVisible(false);
  mSouthWest->setVisible(false);
  mNorthEast->setVisible(false);
  mNorthWest->setVisible(false);
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<Ogre::Vector3> TransformatorSet::use(Ogre::Vector2 pFrom, bool pCheckOnly)
{
  std::vector<Ogre::Vector3> lAttachingPoints;

  float lDistanceSouthEast = ((mNodeCenter + mOffsetSouthEast)
    - Ogre::Vector3(pFrom.x, 0, pFrom.y)).length();
  float lDistanceSouthWest = ((mNodeCenter + mOffsetSouthWest)
    - Ogre::Vector3(pFrom.x, 0, pFrom.y)).length();
  float lDistanceNorthEast = ((mNodeCenter + mOffsetNorthEast)
    - Ogre::Vector3(pFrom.x, 0, pFrom.y)).length();
  float lDistanceNorthWest = ((mNodeCenter + mOffsetNorthWest)
    - Ogre::Vector3(pFrom.x, 0, pFrom.y)).length();

  if ((lDistanceNorthEast <= lDistanceNorthWest)
    && (lDistanceNorthEast <= lDistanceSouthEast)
    && (lDistanceNorthEast <= lDistanceSouthWest))
  {
    mNorthEast->setVisible(!pCheckOnly);

    if (mType == eCityTrans) {
      lAttachingPoints.push_back(mNodeCenter + mOffsetNorthEast + cHeight);
      lAttachingPoints.push_back(mNodeCenter + mOffsetNorthEast + cHeight);
      lAttachingPoints.push_back(mNodeCenter + mOffsetNorthEast + cHeight);
      lAttachingPoints.push_back(mNodeCenter + mOffsetNorthEast + cHeight);
    } else {
      lAttachingPoints.push_back(mNodeCenter + mOffsetNorthEast
        + (cHeight*0.25) + Ogre::Vector3(1.75,2.5,-0.6));
      lAttachingPoints.push_back(mNodeCenter + mOffsetNorthEast
        + (cHeight*0.25) + Ogre::Vector3(1.75,2.5,0.0));
      lAttachingPoints.push_back(mNodeCenter + mOffsetNorthEast
        + (cHeight*0.25) + Ogre::Vector3(1.75,2.5,1.2));
      lAttachingPoints.push_back(mNodeCenter + mOffsetNorthEast
        + (cHeight*0.25) + Ogre::Vector3(1.75,2.5,0.6));
    }
  }
  else if ((lDistanceNorthWest <= lDistanceSouthEast) 
    && (lDistanceNorthWest <= lDistanceSouthWest)) {
      mNorthWest->setVisible(!pCheckOnly);

      if (mType == eCityTrans) {
        lAttachingPoints.push_back(mNodeCenter + mOffsetNorthWest + cHeight);
        lAttachingPoints.push_back(mNodeCenter + mOffsetNorthWest + cHeight);
        lAttachingPoints.push_back(mNodeCenter + mOffsetNorthWest + cHeight);
        lAttachingPoints.push_back(mNodeCenter + mOffsetNorthWest + cHeight);
      } else {
        lAttachingPoints.push_back(mNodeCenter + mOffsetNorthWest 
          + (cHeight*0.25) + Ogre::Vector3(-1.75,2.5,-0.6));
        lAttachingPoints.push_back(mNodeCenter + mOffsetNorthWest 
          + (cHeight*0.25) + Ogre::Vector3(-1.75,2.5,0.0));
        lAttachingPoints.push_back(mNodeCenter + mOffsetNorthWest 
          + (cHeight*0.25) + Ogre::Vector3(-1.75,2.5,1.2));
        lAttachingPoints.push_back(mNodeCenter + mOffsetNorthWest 
          + (cHeight*0.25) + Ogre::Vector3(-1.75,2.5,0.6));
      }
  }
  else if (lDistanceSouthEast <= lDistanceSouthWest) {
    mSouthEast->setVisible(!pCheckOnly);

    if (mType == eCityTrans) {
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthEast + cHeight);
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthEast + cHeight);
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthEast + cHeight);
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthEast + cHeight);
    } else {
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthEast 
        + (cHeight*0.25) + Ogre::Vector3(0.6,2.5,1.75));
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthEast 
        + (cHeight*0.25) + Ogre::Vector3(0.0,2.5,1.75));
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthEast 
        + (cHeight*0.25) + Ogre::Vector3(-1.2,2.5,1.75));
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthEast 
        + (cHeight*0.25) + Ogre::Vector3(-0.6,2.5,1.75));
    }
  } else {
    mSouthWest->setVisible(!pCheckOnly);

    if (mType == eCityTrans) {
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthWest + cHeight);
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthWest + cHeight);
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthWest + cHeight);
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthWest + cHeight);
    }  else {
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthWest 
        + (cHeight*0.25) + Ogre::Vector3(0.6,2.5,-1.75));
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthWest 
        + (cHeight*0.25) + Ogre::Vector3(0.0,2.5,-1.75));
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthWest 
        + (cHeight*0.25) + Ogre::Vector3(-1.2,2.5,-1.75));
      lAttachingPoints.push_back(mNodeCenter + mOffsetSouthWest 
        + (cHeight*0.25) + Ogre::Vector3(-0.6,2.5,-1.75));
    }
  }

  return lAttachingPoints;
}

/*-----------------------------------------------------------------------------------------------*/

void TransformatorSet::clear(void)
{
  mSouthEast->setVisible(false);
  mSouthWest->setVisible(false);
  mNorthEast->setVisible(false);
  mNorthWest->setVisible(false);
}