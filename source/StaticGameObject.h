#ifndef STATICGAMEOBJECT_H
#define STATICGAMEOBJECT_H

/*-----------------------------------------------------------------------------------------------*/

#include "Collidable.h"

/*-----------------------------------------------------------------------------------------------*/

class Company;
class InstancedGeometryManager;
class Terrain;

/*-----------------------------------------------------------------------------------------------*/

//! Types of static objects
enum eStaticObjectType { eTree, eFarm, eHouse, eAirplane, eVehicle, eCollider,
						 eReCoal, eReGas, eReUranium };

/*-----------------------------------------------------------------------------------------------*/

//! Class to represent static objects that don't interfere with the game
class StaticGameObject : public Collidable {
public:
	
	//! Create it
	StaticGameObject(Ogre::Vector2 pPosition, eStaticObjectType pType, Ogre::Rect pRect, 
    bool pIsAutoremove);

	//! Add to scene
	void addToSceneManager(Ogre::SceneManager* pSceneManager, boost::shared_ptr<Terrain> pTerrain);

	//! Remove from Scene
	void removeFromSceneManager(void);

  //! Rotate object
	void rotate(float pTimeElapsed);

	// Implement collidable interface
  bool isAutoremove(void);
	std::vector< RotatedRect > getGroundRects(void);
	Ogre::MovableObject* getMainEntityObject(void);
	void setVisible(bool pVisible);

	// Implement GameObject interface
	eGameObjectType getType(void);
	eCompanyObjectType getSubtype(void);
	Ogre::Vector2 getPosition(void);
	bool isSelectable(void);
	eStaticObjectType getSubType(void);

  // Instanced geometry
	static boost::shared_ptr<InstancedGeometryManager> mTree1Batch;
	static boost::shared_ptr<InstancedGeometryManager> mTree2Batch;
	static boost::shared_ptr<InstancedGeometryManager> mTree1FloorBatch;
	static boost::shared_ptr<InstancedGeometryManager> mTree2FloorBatch;

private:

	bool mIsAutoremove; //!< Remove on build?
	Ogre::Rect mRect; //!< Occupied rect
	eStaticObjectType mType; //!< Subtype
	Ogre::Vector2 mPosition; //!!< Position
	Ogre::SceneNode* mNode; //!< OGRE scene node
	Ogre::Entity* mEntity; //!< OGRE entities
	Ogre::Entity* mEntityFloor; //!< Floor(shadow) entity
};

/*-----------------------------------------------------------------------------------------------*/

#endif // STATICGAMEOBJECT_H