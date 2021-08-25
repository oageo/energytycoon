#ifndef MAP_H
#define MAP_H

/*-----------------------------------------------------------------------------------------------*/

#include "Shared.h"
#include "Company.h"

/*-----------------------------------------------------------------------------------------------*/

class City;
class Street;
class River;
class Collidable;
class PowerNet;
class StaticGameObject;
class Terrain;

/*-----------------------------------------------------------------------------------------------*/

const int cMinimapRealWidth = 148; //!< Minimap size in game
const int cMinimapRealHeight = 148;
const int cMinimapSourceWidth = 148; //!< Minimap texture size
const int cMinimapSourceHeight = 148;
const int cMinimapTextureWidth = 256; //!< Real minimap texture size (non pow. of 2 gpus)
const int cMinimapTextureHeight = 256;

/*-----------------------------------------------------------------------------------------------*/

enum eMinimapMode { eDefault, eSupply, eLines, eResources }; //!< The available minimap modes

/*-----------------------------------------------------------------------------------------------*/

//! Holds the terrain and all collidables
class Map {
public:

	//! Create it
	Map(std::string pMapPrefix, Ogre::SceneManager* pSceneManager);

	~Map(void);

	//! Add a collidable
	void addCollidable(boost::shared_ptr<Collidable>, bool pAutoRemove = false);

	//! Remove a collidable
	void removeCollidable(std::string pID);

	//! Check for collisions of build node
	bool collides(Ogre::Rect pRect, boost::shared_ptr<Collidable> &oColliding,
    bool pResourceCheck = false, bool pWaterCheck = false, bool pRiverCheck = false,
    bool pPoleCheck = false, bool pTreeCheck = false);

	//! Is build node over resource
	bool isOverResource(int pPositionX, int pPositionZ, eResourceType pType);

	//! Return minimap texture
	Ogre::TexturePtr getMinimapTexture(void);

	//! Update minimap
	void updateMinimap(void);

	//! Repaint everything !Takes a long time!
	void repaintMinimap(eMinimapMode pMode);

	//! Get map width
	int getWidth(void);

	//! Get map width
	int getHeight(void);

	//! Autoremove trees/farms when loading
	void autoremoveFromSerializedBuilding(Ogre::Rect pRect);

	//! Set selector position
	void moveSelector(Ogre::Vector2 pPosition);

  //! set selector visibility
	void showSelector(bool pShow, float pScale = 0.6);

	//! Set current power net
	void setPowerNet(boost::shared_ptr<PowerNet> pPowerNet);

	//! Find the nearest city to pPosition (for PP naming)
	std::string findNearestCity(Ogre::Vector2 pPosition);

	//! Return terrain page (for heights)
	boost::shared_ptr<Terrain> getTerrain(void);

	//! Get map name (e.g. "de", "uk")
	std::string getName() { return mMapPrefix; }

	//! Serialize
	void serializeIntoXMLElement(TiXmlElement* pParentElement);
	
	//! Load
	void deserializeFromXMLElement(TiXmlNode* pParentNode);

	//! Return city by ID (For serializing)
	boost::shared_ptr<Collidable> getCityByID(int pID);

	//! Return all cities
	std::vector<boost::shared_ptr<City> > getAllCities(void) { return mCities; }

	//! Return ALL collidables (incl. static objects)
	std::vector<boost::shared_ptr<Collidable> > getAllCollidables(void) { return mCollidables; }

	//! Return all resources
	std::vector<boost::shared_ptr<StaticGameObject> > getResources(void) { return mResources; }

	//! Update moving object
	void updateAirplanes(float pElapsedTime);

	//! Update moving object
	void updateCars(float pElapsedTime);

	//! Update moving object
	void updateShips(float pElapsedTime);

private:

	//! Hide the selector (from GUI)
	void hideSelector(EventData* pData);

	//! Create the minimap
	void createMinimap(void);

	//! "Prepaint" backgrounds
	void paintMinimapBackground(void);

	//! Add everything to the scene
	void addDataToSceneManager(void);

	//! Add everything to collidables
	void addDataToCollidables(void);

	//! Load map from XML file
	void processXML(void);

	//! Shortcut to reading a point attribute from XML
	void getPointAttributeXML(TiXmlNode* pNode, int &oX, int &oY);

	//! Create the one and only light
	void createMapLight(void);

	//! Read scale factors from XML
	void evaluateScaleFactors(std::string pMapPrefix);

	//! Check areas for build node
	bool isAreaColourAt(float pR, float pG, float pB, int pX, int pY);

	//! Check areas for build node
	bool isResourceColourAt(float pR, float pG, float pB, int pX, int pY);

	//! Add some trees
	void addRandomTrees(void);

	//! Create the selector
	void createSelector(void);

	//! Event handler for minimap mode change
	void minimapModeChange(EventData* pEventData);

	//! "Prepaint" resource overlay !takes a long time!
	void paintResourceOverlay(void);

	//! Create bridges at all river <-> road intersections
	void buildBridges(void);

	//! random distribution of resources
	void distributeResources(void);

	//! load airplane mesh & initialise it
	void loadAirplanes(void);

	//! load ship mesh
	void loadShips(void);


	boost::shared_ptr<TiXmlDocument> mMapXML; //!< Map XML file
	boost::shared_ptr<Ogre::Image> mAreaDefinition; //!< Area image file (oceans, forrests, etc)
	boost::shared_ptr<Ogre::Image> mResourceDefinition; //!< Resource image file
	std::vector<boost::shared_ptr<City> > mCities;
	std::vector<boost::shared_ptr<Street> > mStreets;
	std::vector<boost::shared_ptr<River> > mRivers;
	std::vector<boost::shared_ptr<StaticGameObject> > mStaticObjects;
	std::vector<boost::shared_ptr<Collidable> > mCollidables; //!< All collidables on the map
	Ogre::SceneManager* mSceneManager; //!< OGRE scene manager
	Ogre::TexturePtr mMinimap; //!< Minimap texture
	Ogre::Entity* mSelectorEntity; //!< OGRE entity for selector
	Ogre::SceneNode* mSelectorNode; //!< OGRE node for selector
	int mScaleXData; //!< Scale factors for data from map-editor
	int mScaleZData;
	int mScaleZAreas; //!< Scale factors for "area"-images
	int mScaleXAreas;
	int mMinimapWidth; //!< Minimap size
	int mMinimapHeight;
	std::vector<std::vector<Ogre::ColourValue> > mMinimapData; //!< Raw data for minimap
	std::vector<std::vector<Ogre::ColourValue> > mMinimapBackground;
	std::vector<std::vector<Ogre::ColourValue> > mMinimapResourceOverlay;
	boost::shared_ptr<Terrain> mTerrain; //!< Terrain page
	eMinimapMode mCurrentMinimapMode;
	std::string mMapPrefix; //!< Map name (e.g. "de", "uk")
	int mBridgeID;
	std::vector<Ogre::Vector2> mRiverLineList;
	std::vector<Ogre::Vector2> mStreetLineList;
	boost::shared_ptr<StaticGameObject> mCollider;
	std::vector<boost::shared_ptr<StaticGameObject> > mResources;
	Ogre::Entity* mAirplane;
	Ogre::SceneNode* mAirplaneNode;
	Ogre::Vector3 mAirplaneDirection;
	bool mAirplaneFlying;
	Ogre::Entity* mAirplane2;
	Ogre::SceneNode* mAirplaneNode2;
	Ogre::Vector3 mAirplaneDirection2;
	bool mAirplaneFlying2;
	Ogre::Entity* mCargoShip;
	Ogre::SceneNode* mCargoShipNode;
	Ogre::Vector3 mCargoShipDirection;
	Ogre::Vector3 mOldCargoShipDirection;
	bool mCargoShipRotating;
};

/*-----------------------------------------------------------------------------------------------*/

#endif // MAP_H
