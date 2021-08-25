#ifndef INSTANCEDGEOMETRYMANAGER_H
#define INSTANCEDGEOMETRYMANAGER_H

/*-----------------------------------------------------------------------------------------------*/

const int cOGREBatchCountMax = 80; //!< Limit set by OGRE

/*-----------------------------------------------------------------------------------------------*/

//! Instanced geometry manager
class InstancedGeometryManager {
public:

  //! Construct it
  InstancedGeometryManager(Ogre::SceneManager* pSceneManager, std::string pName, 
                           bool pCastShadow = false);

  //! Add a already created entity with info
  void addEntity(std::string pID, Ogre::Entity *pEntity, Ogre::Vector3 pPosition,
    Ogre::Quaternion pOrientation = Ogre::Quaternion::IDENTITY,
    Ogre::Vector3 pScale = Ogre::Vector3::UNIT_SCALE);

  //! Set entity orientation (costly)
  void setOrientation(std::string pID, Ogre::Quaternion pOrientation);

  //! Set entity position (costly)
  void setPosition(std::string pID, Ogre::Vector3 pPosition);

  //! Set entity visibility state (costly)
  void setEntityVisible(std::string pID, bool pVisible);

  //! (Re)build batch (very costly)
  void build(void);

private:

  //! Find a entity & cache indices by its GameObject ID
  Ogre::InstancedGeometry::InstancedObject* findObject(std::string pID, int &oK, int &oJ);

  //! Find a entity by its GameObject ID
  Ogre::InstancedGeometry::InstancedObject* findObject(std::string pID);

  //! Modify existing entity material to make it batchable
  void setupMaterial(Ogre::Entity* pEntity);

  //! Build the modified material
  std::string buildInstancedMaterial(std::string pOriginalMaterialName);


  int mObjectCount; //!< Number of entities in this instanced geometry
  size_t mCurrentBatchCounter; //!< Number of entities in current batch (up to cOGREBatchCountMax)
  size_t mCurrentBatchCount; //!< Number batches in this instance
  Ogre::InstancedGeometry* mBatch; //!< OGRE batch instance
  std::vector< std::vector< Ogre::Vector3 > > mPositions; //!< Cached positions
  std::vector< std::vector< Ogre::Vector3 > > mScales; // !< Cached scales
  std::map< std::string, Ogre::Vector2 > mIDs; // !< Batch/Object indices map
};

/*-----------------------------------------------------------------------------------------------*/

#endif // INSTANCEDGEOMETRYMANAGER_H
