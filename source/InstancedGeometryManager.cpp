#include "StdAfx.h"
#include "InstancedGeometryManager.h"

/*-----------------------------------------------------------------------------------------------*/

InstancedGeometryManager::InstancedGeometryManager(Ogre::SceneManager* pSceneManager,
                                                   std::string pName, bool pCastShadow)
: mObjectCount(0),
  mCurrentBatchCount(0),
  mCurrentBatchCounter(0)
{
  mBatch = new Ogre::InstancedGeometry(pSceneManager, pName);
  mBatch->setCastShadows(pCastShadow);

  std::vector< Ogre::Vector3 > lTemp;
  mPositions.push_back(lTemp);
  mScales.push_back(lTemp);
}

/*-----------------------------------------------------------------------------------------------*/

void InstancedGeometryManager::addEntity(std::string pID, Ogre::Entity *pEntity,
                                         Ogre::Vector3 pPosition, Ogre::Quaternion pOrientation,
                                         Ogre::Vector3 pScale)
{
  ++mObjectCount;
  ++mCurrentBatchCounter;

  if (mObjectCount < cOGREBatchCountMax) {
    setupMaterial(pEntity);
    mBatch->addEntity(pEntity, Ogre::Vector3::ZERO);
  }

  mPositions[mCurrentBatchCount].push_back(pPosition);
  mScales[mCurrentBatchCount].push_back(pScale);
  mIDs[pID] = Ogre::Vector2(mCurrentBatchCount, mCurrentBatchCounter - 1);

  if (mCurrentBatchCounter == cOGREBatchCountMax) {
    mCurrentBatchCounter = 0;
    ++mCurrentBatchCount;
    std::vector< Ogre::Vector3 > lTemp;
    mPositions.push_back(lTemp);
    mScales.push_back(lTemp);
  }
}

/*-----------------------------------------------------------------------------------------------*/

void InstancedGeometryManager::setOrientation(std::string pID, Ogre::Quaternion pOrientation)
{
  Ogre::InstancedGeometry::InstancedObject* lObject = findObject(pID);

  if (lObject)
    lObject->setOrientation(pOrientation);
}

/*-----------------------------------------------------------------------------------------------*/

void InstancedGeometryManager::setPosition(std::string pID, Ogre::Vector3 pPosition)
{
  Ogre::InstancedGeometry::InstancedObject* lObject = findObject(pID);

  if (lObject)
    lObject->setPosition(pPosition);
}

/*-----------------------------------------------------------------------------------------------*/

void InstancedGeometryManager::setEntityVisible(std::string pID, bool pVisible)
{
  int k, j;
  Ogre::InstancedGeometry::InstancedObject* lObject = findObject(pID, k, j);

  if (lObject) {
    if (pVisible)
      lObject->setPosition(mPositions[k][j]);
    else
      lObject->setPosition(Ogre::Vector3(0.0, -40.0, 0.0));
  }
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::InstancedGeometry::InstancedObject* InstancedGeometryManager::findObject(std::string pID,
                                                                               int &oK, int &oJ)
{
  if (mIDs.find(pID) == mIDs.end())
    return 0;

  Ogre::Vector2 lIndex = mIDs[pID];

  Ogre::InstancedGeometry::BatchInstanceIterator lInstanceIterator =
    mBatch->getBatchInstanceIterator();

  size_t k = 0;

  while (lInstanceIterator.hasMoreElements ()) {
    Ogre::InstancedGeometry::BatchInstance *lCurrentInstance = lInstanceIterator.getNext();

    Ogre::InstancedGeometry::BatchInstance::InstancedObjectIterator lObjectIterator = 
      lCurrentInstance->getObjectIterator();

    size_t j = 0;

    while (lObjectIterator.hasMoreElements()) {
      Ogre::InstancedGeometry::InstancedObject* lObject = lObjectIterator.getNext();

      if ((mPositions[k].size() > j) && (k == lIndex.x) && (j == lIndex.y)) {
        oK = k;
        oJ = j;
        return lObject;
      }
      ++j;
    }
    ++k;
  }

  return 0;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::InstancedGeometry::InstancedObject* InstancedGeometryManager::findObject(std::string pID)
{
  int lTemp1, lTemp2;
  return findObject(pID, lTemp1, lTemp2);
}

/*-----------------------------------------------------------------------------------------------*/

void InstancedGeometryManager::build(void)
{
  mBatch->setOrigin(Ogre::Vector3::ZERO);
  mBatch->build();

  size_t lNumberRender = 1;
  size_t lObjectCount = mObjectCount;

  while (lObjectCount > cOGREBatchCountMax) {
    lNumberRender++;
    lObjectCount -= cOGREBatchCountMax;
  }

  for (size_t i = 0; i < lNumberRender - 1; ++i)
    mBatch->addBatchInstance();

  Ogre::InstancedGeometry::BatchInstanceIterator lInstanceIterator = 
    mBatch->getBatchInstanceIterator();

  size_t k = 0;

  while (lInstanceIterator.hasMoreElements ()) {
    Ogre::InstancedGeometry::BatchInstance *lCurrentInstance = lInstanceIterator.getNext();

    Ogre::InstancedGeometry::BatchInstance::InstancedObjectIterator lObjectIterator = 
      lCurrentInstance->getObjectIterator();

    size_t j = 0;

    while (lObjectIterator.hasMoreElements()) {
      Ogre::InstancedGeometry::InstancedObject* lObject = lObjectIterator.getNext();

      if (mPositions[k].size() > j)  {
        lObject->setPosition(mPositions[k][j]);
        lObject->setScale(mScales[k][j]);
      }
      ++j;
    }
    ++k;
  }

  mBatch->setVisible(true);
}

/*-----------------------------------------------------------------------------------------------*/

void InstancedGeometryManager::setupMaterial(Ogre::Entity* pEntity)
{
  for (size_t i = 0; i < pEntity->getNumSubEntities(); ++i) {
    Ogre::SubEntity* lSubEntity = pEntity->getSubEntity(i);
    std::string lMaterialName = lSubEntity->getMaterialName();
    lSubEntity->setMaterialName(buildInstancedMaterial(lMaterialName));
  }
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::String InstancedGeometryManager::buildInstancedMaterial(std::string pOriginalMaterialName)
{
  if (Ogre::StringUtil::endsWith(pOriginalMaterialName, "/instanced"))
    return pOriginalMaterialName;

  Ogre::MaterialPtr lOriginalMaterial = 
    Ogre::MaterialManager::getSingleton ().getByName (pOriginalMaterialName);

  std::string lInstancedMaterialName;
  
  if (lOriginalMaterial.isNull())
    lInstancedMaterialName = "Instancing";
  else
    lInstancedMaterialName = pOriginalMaterialName + "/Instanced";
  
  Ogre::MaterialPtr lInstancedMaterial = 
    Ogre::MaterialManager::getSingleton().getByName(lInstancedMaterialName);

  if (lInstancedMaterial.isNull()) {  
    lInstancedMaterial = lOriginalMaterial->clone(lInstancedMaterialName);
    lInstancedMaterial->load();

    Ogre::Technique::PassIterator lPassIterator = 
      lInstancedMaterial->getBestTechnique ()->getPassIterator();

    while (lPassIterator.hasMoreElements()) {
      Ogre::Pass* lPass = lPassIterator.getNext();
      lPass->setVertexProgram("Instancing", false);
      lPass->setShadowCasterVertexProgram("InstancingShadowCaster");
    }
  }

  lInstancedMaterial->load();
  return lInstancedMaterialName;
}
