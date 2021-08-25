#ifndef TRANSFORMATORSET_H
#define TRANSFORMATORSET_H

/*-----------------------------------------------------------------------------------------------*/

#include "GameObject.h"

/*-----------------------------------------------------------------------------------------------*/

enum eTransformatorType { eCityTrans, ePowerplantTrans }; //!< Determines mesh type

/*-----------------------------------------------------------------------------------------------*/

//! 4 possible transformators for a city or powerplant
class TransformatorSet : public GameObject {
public:

  //! Create a transformator set
  TransformatorSet(Ogre::SceneManager* pSceneManager, Ogre::Vector3 pNodeCenter, 
    eTransformatorType pType, eCompanyObjectType pSubType = eNull);

  //! Use a transformator (autocreate if not available) and return attaching points
  std::vector< Ogre::Vector3 > use(Ogre::Vector2 pFrom, bool pCheckOnly = false);

  //! Clear all (on redistribution)
  void clear(void);

  // Implement GameObject interface
  Ogre::Vector2 getPosition(void) { return Ogre::Vector2(mNodeCenter.x, mNodeCenter.z); }
  bool isSelectable(void) { return false; }
  eGameObjectType getType(void) { return eStatic; }
  eCompanyObjectType getSubtype(void) { return eNull; }

private:

  Ogre::SceneManager* mSceneManager; //!< OGRE scene manager
  Ogre::Vector3 mNodeCenter; //!< Center of the node to attach to
  Ogre::Entity* mTransformator; //!< Transformator mesh
  Ogre::SceneNode* mNorthWest,* mNorthEast,* mSouthEast,* mSouthWest; //!< OGRE scene nodes
  Ogre::Vector3 mOffsetNorthWest, mOffsetNorthEast; //!< Offset to mNodeCenter
  Ogre::Vector3 mOffsetSouthEast, mOffsetSouthWest; //!< Offset to mNodeCenter
  eTransformatorType mType; //!< City or powerplant
};

/*-----------------------------------------------------------------------------------------------*/

#endif // TRANSFORMATORSET_H