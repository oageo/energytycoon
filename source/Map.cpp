#include "StdAfx.h"
#include "Map.h"
#include "Event.h"
#include "Pole.h"
#include "Company.h"
#include "InstancedGeometryManager.h"
#include "City.h"
#include "Street.h"
#include "River.h"
#include "Collidable.h"
#include "PowerNet.h"
#include "StaticGameObject.h"
#include "Terrain.h"
#include "Company.h"
#include "Event.h"
#include "Powerplant.h"

/*-----------------------------------------------------------------------------------------------*/

Map::Map(std::string pMapPrefix, Ogre::SceneManager* pSceneManager)
: mSceneManager(pSceneManager),
  mMinimapWidth(cMinimapSourceWidth),
	mMinimapHeight(cMinimapSourceWidth),
  mCurrentMinimapMode(eDefault),
	mMapPrefix(pMapPrefix),
  mBridgeID(0),
  mAirplaneFlying(false),
  mAirplaneFlying2(false),
	mCargoShipRotating(false)
{
	createMapLight();
	mTerrain.reset(new Terrain(pMapPrefix, pSceneManager));
	evaluateScaleFactors(pMapPrefix);

	EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(38));

	mAreaDefinition.reset(new Ogre::Image());
	mAreaDefinition->load(pMapPrefix + "_areamap.png",
		Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

	mResourceDefinition.reset(new Ogre::Image());
	mResourceDefinition->load(pMapPrefix + "_resourcemap.png",
		Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

	createMinimap();

	EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(45));
	EventHandler::raiseEvent(eSetLoadingStatus, new EventArg<std::string>(
		StrLoc::get()->LoadingPopulace()));

	mMapXML.reset(new TiXmlDocument((cDataDirPre + "maps/" + pMapPrefix + "_data.xml").c_str()));
	mMapXML->LoadFile(TIXML_ENCODING_UTF8);
	processXML();

	addDataToSceneManager();

	EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(55));
	EventHandler::raiseEvent(
		eSetLoadingStatus, new EventArg<std::string>(StrLoc::get()->LoadingTreesResources()));

	createSelector();
	showSelector(false);

	addDataToCollidables();

	EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(80));

	REGISTER_CALLBACK(eMinimapModeChange, Map::minimapModeChange);
	REGISTER_CALLBACK(eHideSelector, Map::hideSelector);
	
	mCollider.reset( new StaticGameObject(Ogre::Vector2(-1000,-1000), eCollider,
		rectFromPoint2(Ogre::Vector2(-1000,-1000), 2), false) );

	loadAirplanes();
	loadShips();
}

/*-----------------------------------------------------------------------------------------------*/

Map::~Map(void)
{
}

/*-----------------------------------------------------------------------------------------------*/

void Map::processXML(void)
{
	TiXmlNode* rootNode;
	TiXmlNode* node;
	TiXmlNode* subNode;

	std::vector<std::string> lBorderPoints;

	rootNode =  mMapXML->FirstChildElement("map_data");
	if (rootNode) {
		node = rootNode->FirstChildElement("city");
		while (node) {
			int lX,lY;

			getPointAttributeXML(node, lX, lY);

			std::string lSize = node->ToElement()->Attribute("size");
			Ogre::UTFString lName = node->ToElement()->GetText();

			mCities.push_back(boost::shared_ptr<City>(
				new City(Ogre::Vector2(lX * mScaleXData, lY * mScaleZData),
				lName, lSize)));

			node = node->NextSiblingElement("city");
		}

		node = rootNode->FirstChildElement("street");
		while (node) {
			int lX1,lY1,lX2,lY2;

			lX1 = toNumber<int>(node->ToElement()->Attribute("from_x"));
			lY1 = toNumber<int>(node->ToElement()->Attribute("from_y"));
			lX2 = toNumber<int>(node->ToElement()->Attribute("to_x"));
			lY2 = toNumber<int>(node->ToElement()->Attribute("to_y"));

			mStreets.push_back(boost::shared_ptr<Street>(new Street(lX1 * mScaleXData,
									  lY1 * mScaleZData, lX2 
									  * mScaleXData, lY2 * mScaleZData)));

			node = node->NextSiblingElement("street");
		}

		node = rootNode->FirstChildElement("river");
		while (node) {
			std::vector<Ogre::Vector2> lRiverPoints;

			std::string lOceanEndString = node->ToElement()->Attribute("ocean_end");

			bool lOceanEnd = (lOceanEndString == "yes") ? true : false;

			subNode = node->FirstChildElement("river_point");
			while(subNode)
			{
				int lX,lY;
				getPointAttributeXML(subNode, lX, lY);
				lRiverPoints.push_back(Ogre::Vector2(lX * mScaleXData, lY * mScaleZData));

				subNode = subNode->NextSiblingElement("river_point");
			}
			
			mRivers.push_back(boost::shared_ptr<River>(new River(lRiverPoints, lOceanEnd)));
			node = node->NextSiblingElement("river");
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Map::loadShips(void)
{
	mCargoShip = mSceneManager->createEntity("CargoShip", "shipt.mesh");
	mCargoShipNode = mSceneManager->getRootSceneNode()->createChildSceneNode("CargoShipNode");
	mCargoShipNode->attachObject(mCargoShip);

	mCargoShipNode->setScale(0.35,0.35,0.35);

	bool pStartPositionFound = false;

	int lRandZ, lRandX;

	do {
		lRandZ = ( rand() % mTerrain->getHeight() );
		lRandX = ( rand() % mTerrain->getWidth() );

		if(isAreaColourAt(0.0, 0.0, 1.0, lRandX, lRandZ))
			pStartPositionFound = true;
	} while (!pStartPositionFound); // TODO: map boundaries check

	mCargoShipDirection = Ogre::Vector3((rand() % 100)-50,0, (rand() % 100)-50);
	mCargoShipDirection = mCargoShipDirection.normalisedCopy();
	
	mCargoShipNode->setPosition(lRandX, 4.2, lRandZ);
}

/*-----------------------------------------------------------------------------------------------*/

void Map::updateShips(float pElapsedTime)
{// TODO: FIX FOR ALL MAPS
/*	Ogre::Vector3 lNextPosition = mCargoShipNode->getPosition() + mCargoShipDirection 
    * pElapsedTime * 500;

	if(!isAreaColourAt(0.0, 0.0, 1.0, lNextPosition.x, lNextPosition.z) || ((lNextPosition.x < 10) 
								|| (lNextPosition.z < 10)
								|| (lNextPosition.z > 2038) 
								|| (lNextPosition.x > 1014))) {
		do {
			mCargoShipDirection = Ogre::Vector3((rand() % 100)-50,0, (rand() % 100)-50);
			mCargoShipDirection = mCargoShipDirection.normalisedCopy();

			lNextPosition = mCargoShipNode->getPosition() + mCargoShipDirection  * pElapsedTime * 500;

		} while (!isAreaColourAt(0.0, 0.0, 1.0, lNextPosition.x, lNextPosition.z) 
                      || ((lNextPosition.x < 10) 
											|| (lNextPosition.z < 10)
											|| (lNextPosition.z > 2038) 
											|| (lNextPosition.x > 1014)));
	}

	mCargoShipNode->resetOrientation();
	mCargoShipNode->setDirection(mCargoShipDirection);
	mCargoShipNode->translate(mCargoShipDirection * pElapsedTime * 5);*/
}

/*-----------------------------------------------------------------------------------------------*/

void Map::loadAirplanes(void)
{
	mAirplane = mSceneManager->createEntity("Airplane1", "plane.mesh");
	mAirplane->getMesh()->buildEdgeList();
	mAirplaneNode = mSceneManager->getRootSceneNode()->createChildSceneNode("AirplaneNode1");
	mAirplaneNode->attachObject(mAirplane);

	mAirplane2 = mSceneManager->createEntity("Airplane2", "plane.mesh");
	mAirplane2->getMesh()->buildEdgeList();
	mAirplaneNode2 = mSceneManager->getRootSceneNode()->createChildSceneNode("AirplaneNode2");
	mAirplaneNode2->attachObject(mAirplane2);
}

/*-----------------------------------------------------------------------------------------------*/

void Map::updateAirplanes(float pElapsedTime)
{
	if (mAirplaneFlying == false) {
		mAirplaneDirection = Ogre::Vector3((rand() % 100)-50,0, (rand() % 100)-50);
		mAirplaneDirection.normalise();

		mAirplaneNode->resetOrientation();
		mAirplaneNode->setPosition(-256,100,1024);
		mAirplaneNode->setDirection(mAirplaneDirection);
		mAirplaneNode->setScale(Ogre::Vector3(0.04,0.04,0.04));

		mAirplaneFlying = true;
	} else {
		mAirplaneNode->translate(mAirplaneDirection * pElapsedTime * 21);

		if ((mAirplaneNode->getPosition().x < -256) 
			|| (mAirplaneNode->getPosition().z < -128)
			|| (mAirplaneNode->getPosition().z > 2166) 
			|| (mAirplaneNode->getPosition().x > 1280)) {
			mAirplaneFlying = false;
		}
	}

	if (mAirplaneFlying2 == false) {
		mAirplaneDirection2 = Ogre::Vector3((rand() % 100)-50,0, (rand() % 100)-50);
		mAirplaneDirection2.normalise();

		mAirplaneNode2->resetOrientation();
		mAirplaneNode2->setPosition(1280,95,1524);
		mAirplaneNode2->setDirection(mAirplaneDirection2);
		mAirplaneNode2->setScale(Ogre::Vector3(0.04,0.04,0.04));

		mAirplaneFlying2 = true;
	} else {
		mAirplaneNode2->translate(mAirplaneDirection2 * pElapsedTime * 21);

		if ((mAirplaneNode2->getPosition().x < -256) 
			|| (mAirplaneNode2->getPosition().z < -128)
			|| (mAirplaneNode2->getPosition().z > 2166) 
			|| (mAirplaneNode2->getPosition().x > 1280)) {
			mAirplaneFlying2 = false;
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Map::updateCars(float pElapsedTime)
{
	for (size_t i = 0; i < mStreets.size(); i++)
		mStreets[i]->updateCars(pElapsedTime);
}

/*-----------------------------------------------------------------------------------------------*/

void Map::hideSelector(EventData* pData)
{
	showSelector(false);
}

/*-----------------------------------------------------------------------------------------------*/

void Map::getPointAttributeXML(TiXmlNode *pNode, int &oX, int &oY)
{
	oX = toNumber< int >(pNode->ToElement()->Attribute("x"));
	oY = toNumber< int >(pNode->ToElement()->Attribute("y"));
}

/*-----------------------------------------------------------------------------------------------*/

void Map::serializeIntoXMLElement(TiXmlElement* pParentElement)
{
}

/*-----------------------------------------------------------------------------------------------*/

void Map::addDataToSceneManager()
{
	for (unsigned int i = 0; i < mCities.size(); i++)
		mCities[i]->addToSceneManager(mSceneManager, mTerrain);

	for (unsigned int k = 0; k < mStreets.size(); k++) {
		mStreets[k]->generateAndAddToSceneManager(mSceneManager, mTerrain);

		std::vector< Ogre::Vector2 > lLineList = mStreets[k]->getLineList();
		
		for(unsigned int j = 0; j < lLineList.size(); j++) {
			mStreetLineList.push_back(lLineList[j]);
		}
	}

	for(unsigned int l = 0; l < mRivers.size(); l++) {
		mRivers[l]->generateAndAddToSceneManager(mSceneManager, mTerrain);

		std::vector< Ogre::Vector2 > lLineList = mRivers[l]->getLineList();

		for(unsigned int m = 0; m < lLineList.size(); m++)
			mRiverLineList.push_back(lLineList[m]);
	}

	buildBridges();

	mTerrain->update();
}

/*-----------------------------------------------------------------------------------------------*/

void Map::buildBridges(void)
{
	for (unsigned int i = 0; i < mStreetLineList.size() - 1; i += 2) {
		for (unsigned int j = 0; j < mRiverLineList.size() - 1; j += 2) {
			if (intersectLines(mStreetLineList[i], mStreetLineList[i+1],
				mRiverLineList[j], mRiverLineList[j+1])) {
				Ogre::Vector2 lIntersection = intersection(mStreetLineList[i],
					mStreetLineList[i+1], mRiverLineList[j], mRiverLineList[j+1]);

				Ogre::Entity* lEntity = mSceneManager->createEntity("bridge" 
					+ toString(mBridgeID), "bruecke.mesh");
				Ogre::SceneNode* lNode = mSceneManager->getRootSceneNode()->
					createChildSceneNode("bridge_node" + toString(mBridgeID));
				lEntity->setRenderQueueGroup(99);
				float lHeight = mTerrain->getHighestFromQuad( lIntersection.x,
					lIntersection.y, 20) + 0.2;

				lNode->attachObject(lEntity);
				lNode->scale(0.027, 0.03, 0.04);

				Ogre::Radian lRotation = Ogre::Math::ACos((mStreetLineList[i+1] 
				- mStreetLineList[i]).normalisedCopy().dotProduct(Ogre::Vector2(1.0,0.0)));

				if(mStreetLineList[i+1].y > mStreetLineList[i].y)
					lNode->rotate(Ogre::Vector3(0.0, 1.0, 0.0), -1*(lRotation 
          - Ogre::Radian(Ogre::Math::PI/2.0)));
				else
					lNode->rotate(Ogre::Vector3(0.0, 1.0, 0.0), lRotation 
          - Ogre::Radian(Ogre::Math::PI/2.0));

				lNode->translate( Ogre::Vector3( lIntersection.x, lHeight, lIntersection.y));

				mBridgeID++;
			}
		}
	}
}


void Map::addCollidable(boost::shared_ptr<Collidable> pCollidable, bool pAutoremove)
{
	RotatedRect lRequestRect = pCollidable->getGroundRects()[0];

	if (pAutoremove) {
		std::vector<boost::shared_ptr<Collidable> >::iterator it = mCollidables.begin();

		while (it != mCollidables.end()) {
			bool lDelete = false;

			for(unsigned int j = 0; j < (*it)->getGroundRects().size(); j++) {
				RotatedRect lCheckRect = (*it)->getGroundRects()[j];

				if (intersectRotated(lRequestRect, lCheckRect)) {
					if ((*it)->getType() == eStatic) {
						boost::dynamic_pointer_cast<StaticGameObject>(*it)->
							removeFromSceneManager();
						lDelete = true;
					}
				}
			}

			if (lDelete) {
				if ((boost::dynamic_pointer_cast<StaticGameObject>(*it)->getSubType() == eReCoal) ||
					(boost::dynamic_pointer_cast<StaticGameObject>(*it)->getSubType() == eReGas) ||
					(boost::dynamic_pointer_cast<StaticGameObject>(*it)->getSubType() == eReUranium)) {
					std::vector<boost::shared_ptr<StaticGameObject> >::iterator it2 = mResources.begin();

					while (it2 != mResources.end()) {
						if ((*it2)->getID() == boost::dynamic_pointer_cast<StaticGameObject>(*it)->getID())
							it2 = mResources.erase(it2);
						else
							it2++;
					}
				}

				it = mCollidables.erase(it);
			} else {
				it++;
      }
		}
	}

	mCollidables.push_back(pCollidable);
	repaintMinimap(mCurrentMinimapMode);

	EventHandler::raiseEvent(eUpdateMinimap);
	updateMinimap();
}

/*-----------------------------------------------------------------------------------------------*/

void Map::removeCollidable(std::string pID)
{
	for (unsigned int i = 0; i < mCollidables.size(); i++)
		if (mCollidables[i]->getID() == pID)
			mCollidables.erase(mCollidables.begin() + i);
}

/*-----------------------------------------------------------------------------------------------*/

bool Map::collides(Ogre::Rect pRect, boost::shared_ptr<Collidable> &oColliding,
				   bool pResourceCheck, bool pWaterCheck, bool pRiverCheck,
				   bool pPoleCheck, bool pTreeCheck)
{
	RotatedRect lRequestRect(pRect);

	for (unsigned int i = 0; i < mCollidables.size(); i++) {
		for (unsigned int j = 0; j < mCollidables[i]->getGroundRects().size(); j++) {
			if (!pResourceCheck) {
				if (!mCollidables[i]->isAutoremove() || pTreeCheck) {
					RotatedRect lCheckRect = mCollidables[i]->getGroundRects()[j];

					if (intersectRotated(lCheckRect, lRequestRect)) {
						oColliding = mCollidables[i];

						if ((oColliding->getType() == eRiver) && pRiverCheck)
							return false;
						else
							return true;
					}
				}
			} else {
				RotatedRect lCheckRect = mCollidables[i]->getGroundRects()[j];

				if (intersectRotated(lCheckRect, lRequestRect)) {
					if (mCollidables[i]->getType() == eStatic) {
						boost::shared_ptr<StaticGameObject> lResourceCheck
								= boost::dynamic_pointer_cast<StaticGameObject > (mCollidables[i]);

						if ((lResourceCheck->getSubType() == eReCoal)
								|| ((lResourceCheck->getSubType() == eReGas)
								|| (lResourceCheck->getSubType() == eReUranium))) {
							oColliding = mCollidables[i];
							return true;
						}
					}
				}
			}
		}
	}

	if (!pPoleCheck) {
		if ((mTerrain->getHeightValueAt(pRect.left/mTerrain->getScaleX(),
			pRect.top/mTerrain->getScaleZ()) > 0.43)
			||(mTerrain->getHeightValueAt(pRect.left/mTerrain->getScaleX(),
			pRect.top/mTerrain->getScaleZ()) < 0.12)) {
			oColliding = mCollider;

			if(!pWaterCheck)
				return true;
			else
				return false;
		} else {
			if(pWaterCheck)
				return true;
		}
	}

	if(pRiverCheck)
		return true;

	return false;
}

/*-----------------------------------------------------------------------------------------------*/

void Map::createMapLight(void)
{
	Ogre::Light *lMapLight = mSceneManager->createLight("MapLight");
	lMapLight->setType(Ogre::Light::LT_DIRECTIONAL);
    lMapLight->setDiffuseColour(Ogre::ColourValue(1.0, 1.0, 1.0));
    lMapLight->setDirection(Ogre::Vector3( 1, -1.5, -0.9 )); 
	lMapLight->setCastShadows(true);
}

/*-----------------------------------------------------------------------------------------------*/

void Map::evaluateScaleFactors(std::string pMapPrefix)
{
	boost::shared_ptr< TiXmlDocument > lTerrainConfig;
	TiXmlNode* rootNode;

	lTerrainConfig.reset(new TiXmlDocument((cDataDirPre + "maps/" 
		+ pMapPrefix + "_terrain.xml").c_str()));
	lTerrainConfig->LoadFile(TIXML_ENCODING_UTF8);

	std::vector<std::string> lBorderPoints;

	rootNode = lTerrainConfig->FirstChildElement("terrain_config");

	mScaleXData = mTerrain->getWidth() / (toNumber<int>(
		rootNode->ToElement()->Attribute("data_size_x")) - 1);
	mScaleZData = mTerrain->getHeight() / (toNumber<int>(
		rootNode->ToElement()->Attribute("data_size_z")) - 1);
	mScaleXAreas = mTerrain->getWidth() / (toNumber<int>(
		rootNode->ToElement()->Attribute("areas_size_x")) - 1);
	mScaleZAreas = mTerrain->getHeight() / (toNumber<int>(
		rootNode->ToElement()->Attribute("areas_size_z")) - 1);
}

/*-----------------------------------------------------------------------------------------------*/

bool Map::isAreaColourAt(float pR, float pG, float pB, int pX, int pY)
{
	return (mAreaDefinition->getColourAt(pX / mScaleXAreas, pY / mScaleZAreas, 0)
				== Ogre::ColourValue(pR,pG,pB,1.0));
}

/*-----------------------------------------------------------------------------------------------*/

bool Map::isResourceColourAt(float pR, float pG, float pB, int pX, int pY)
{
	return (mResourceDefinition->getColourAt(pX / mScaleXAreas, pY / mScaleZAreas, 0)
				== Ogre::ColourValue(pR,pG,pB,1.0));
}

/*-----------------------------------------------------------------------------------------------*/

int Map::getWidth(void)
{
	return mTerrain->getWidth();
}

/*-----------------------------------------------------------------------------------------------*/

int Map::getHeight(void)
{
	return mTerrain->getHeight();
}

/*-----------------------------------------------------------------------------------------------*/

void Map::addRandomTrees(void)
{
	for (int j = 0; j < GameConfig::getInt("NumberFarms"); j++) {
		bool lValidPosition = false;

		float lRandZ = 0.0;
		float lRandX = 0.0;

		while (!lValidPosition) {
			lRandZ = ( rand() % mTerrain->getHeight() );
			lRandX = ( rand() % mTerrain->getWidth() );

			boost::shared_ptr<Collidable> lTemp;

			if(!collides(rectFromPoint2(Ogre::Vector2(lRandX, lRandZ), 36), lTemp, false,
          false, false, false, true))
				lValidPosition = true;
		}

		boost::shared_ptr< StaticGameObject > lTemp(new StaticGameObject(Ogre::Vector2(lRandX,
			lRandZ), eFarm, rectFromPoint2(Ogre::Vector2(lRandX, lRandZ), 36), true));

		lTemp->addToSceneManager(mSceneManager, mTerrain);
		mCollidables.push_back(lTemp);
	}

	//StaticGameObject::mTree1Batch.reset(new InstancedGeometryManager(mSceneManager, "Tree1Batch"));
	//StaticGameObject::mTree2Batch.reset(new InstancedGeometryManager(mSceneManager, "Tree2Batch"));

	//StaticGameObject::mTree1FloorBatch.reset(
 //   new InstancedGeometryManager(mSceneManager, "Tree1FloorBatch"));
//	StaticGameObject::mTree2FloorBatch.reset(
 //   new InstancedGeometryManager(mSceneManager, "Tree2FloorBatch"));

	for (int j = 0; j < GameConfig::getInt("NumberTreesForest"); j++) {
		bool lValidPosition = false;

		float lRandZ = 0.0;
		float lRandX = 0.0;

		while (!lValidPosition) {
			lRandZ = ( rand() % mTerrain->getHeight() );
			lRandX = ( rand() % mTerrain->getWidth() );

			if(isAreaColourAt(0.0, 1.0, 0.0, lRandX, lRandZ)) {
				boost::shared_ptr< Collidable > lTemp;

				if(!collides(rectFromPoint2(Ogre::Vector2(lRandX, lRandZ), 2), lTemp, false, false,
          false, false, true))
					lValidPosition = true;
			}
		}

    if(j == 800)
	    EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(62));

    if(j == 1600)
	    EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(69));

    if(j == 2400)
	    EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(76));

		boost::shared_ptr<StaticGameObject> lTemp(new StaticGameObject(Ogre::Vector2(lRandX,
			lRandZ), eTree, rectFromPoint2(Ogre::Vector2(lRandX, lRandZ), 8), true));

		lTemp->addToSceneManager(mSceneManager, mTerrain);
		mCollidables.push_back(lTemp);
	}

	for (int j = 0; j < GameConfig::getInt("NumberTreesRandom"); j++) {
		bool lValidPosition = false;

		float lRandZ = 0.0;
		float lRandX = 0.0;

		while(!lValidPosition) {
			lRandZ = ( rand() % mTerrain->getHeight() );
			lRandX = ( rand() % mTerrain->getWidth() );

			boost::shared_ptr< Collidable > lTemp;

			if(!collides(rectFromPoint2(Ogre::Vector2(lRandX, lRandZ), 2), lTemp, false, false,
        false, false, true))
				lValidPosition = true;
		}

		boost::shared_ptr< StaticGameObject > lTemp(new StaticGameObject(Ogre::Vector2(lRandX,
			lRandZ), eTree, rectFromPoint2(Ogre::Vector2(lRandX, lRandZ), 8), true));

		lTemp->addToSceneManager(mSceneManager, mTerrain);
		mCollidables.push_back(lTemp);
	}

	//StaticGameObject::mTree1Batch->build();
	//StaticGameObject::mTree1FloorBatch->build();

	//StaticGameObject::mTree2Batch->build();
	//StaticGameObject::mTree2FloorBatch->build();
}

/*-----------------------------------------------------------------------------------------------*/

void Map::distributeResources(void)
{
	float lRandZ = 0.0;
	float lRandX = 0.0;

	for (int i = 0; i < GameConfig::getInt("NumberResourcesCoal"); i++) {
		bool lValidPosition = false;

		while (!lValidPosition) {
			lRandZ = ( rand() % mTerrain->getHeight() );
			lRandX = ( rand() % mTerrain->getWidth() );

			if (isResourceColourAt(0.0, 0.0, 1.0, lRandX, lRandZ)) {
				boost::shared_ptr< Collidable > lTemp;

				if(!collides(rectFromPoint2(Ogre::Vector2(lRandX, lRandZ), 10), lTemp))
					lValidPosition = true;
			}
		}

		boost::shared_ptr< StaticGameObject > lTemp(new StaticGameObject(Ogre::Vector2(lRandX,
			lRandZ), eReCoal, rectFromPoint2(Ogre::Vector2(lRandX, lRandZ), 10), true));

		lTemp->addToSceneManager(mSceneManager, mTerrain);
		lTemp->setVisible(false);
		mCollidables.push_back(lTemp);
		mResources.push_back(lTemp);
	}

	for (int j = 0; j < GameConfig::getInt("NumberResourcesGas"); j++) {
		bool lValidPosition = false;

		while (!lValidPosition) {
			lRandZ = ( rand() % mTerrain->getHeight() );
			lRandX = ( rand() % mTerrain->getWidth() );

			if (isResourceColourAt(1.0, 0.0, 0.0, lRandX, lRandZ)) {
				boost::shared_ptr< Collidable > lTemp;

				if (!collides(rectFromPoint2(Ogre::Vector2(lRandX, lRandZ), 10), lTemp))
					lValidPosition = true;
			}
		}

		boost::shared_ptr<StaticGameObject> lTemp(new StaticGameObject(Ogre::Vector2(lRandX,
			lRandZ), eReGas, rectFromPoint2(Ogre::Vector2(lRandX, lRandZ), 10), true));

		lTemp->addToSceneManager(mSceneManager, mTerrain);
		lTemp->setVisible(false);
		mCollidables.push_back(lTemp);
		mResources.push_back(lTemp);
	}

	for (int k = 0; k < GameConfig::getInt("NumberResourcesUranium"); k++) {
		bool lValidPosition = false;

		while (!lValidPosition) {
			lRandZ = ( rand() % mTerrain->getHeight() );
			lRandX = ( rand() % mTerrain->getWidth() );

			if (isResourceColourAt(0.0, 1.0, 0.0, lRandX, lRandZ)) {
				boost::shared_ptr< Collidable > lTemp;

				if(!collides(rectFromPoint2(Ogre::Vector2(lRandX, lRandZ), 8), lTemp))
					lValidPosition = true;
			}
		}

		boost::shared_ptr< StaticGameObject > lTemp(new StaticGameObject(Ogre::Vector2(lRandX,
			lRandZ), eReUranium, rectFromPoint2(Ogre::Vector2(lRandX, lRandZ), 8), true));

		lTemp->addToSceneManager(mSceneManager, mTerrain);
		lTemp->setVisible(false);
		mCollidables.push_back(lTemp);
		mResources.push_back(lTemp);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Map::addDataToCollidables(void)
{
	for (unsigned int i = 0; i < mCities.size(); i++)
		mCollidables.push_back(mCities[i]);

	for (unsigned int j = 0; j < mStreets.size(); j++)
		mCollidables.push_back(mStreets[j]);

	for (unsigned int k = 0; k < mRivers.size(); k++)
		mCollidables.push_back(mRivers[k]);

	addRandomTrees(); // disabled for debugging

	distributeResources();

	repaintMinimap(mCurrentMinimapMode);

	EventHandler::raiseEvent(eUpdateMinimap);
}

/*-----------------------------------------------------------------------------------------------*/

void Map::createSelector(void)
{
	mSelectorEntity = mSceneManager->createEntity("selector", "select.mesh");
	mSelectorNode = mSceneManager->getRootSceneNode()->createChildSceneNode("selector_node");

	//mSelectorEntity->setRenderQueueGroup(110);

	mSelectorNode->attachObject(mSelectorEntity);
	mSelectorNode->translate(Ogre::Vector3(0.0, -4.0, 0.0));
	mSelectorNode->scale(0.5, 0.5, 0.5);
}

/*-----------------------------------------------------------------------------------------------*/

void Map::moveSelector(Ogre::Vector2 pPosition)
{
	mSelectorNode->setPosition(pPosition.x,
		mTerrain->getHeightAt(pPosition.x, pPosition.y) + 3, pPosition.y);
}

/*-----------------------------------------------------------------------------------------------*/

void Map::showSelector(bool pShow, float pScale)
{
	mSelectorNode->setScale(Ogre::Vector3(pScale, pScale, pScale));
	mSelectorNode->setVisible(pShow);
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::TexturePtr Map::getMinimapTexture(void)
{
	return mMinimap;
}

/*-----------------------------------------------------------------------------------------------*/

void Map::updateMinimap(void)
{
	Ogre::HardwarePixelBufferSharedPtr lPixelBuffer = mMinimap->getBuffer();

	lPixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
	const Ogre::PixelBox& lPixelBox = lPixelBuffer->getCurrentLock();

	Ogre::uint8* lDest = static_cast<Ogre::uint8*>(lPixelBox.data);

	for (int j = 0; j < cMinimapTextureWidth; j++) {
		for (int i = 0; i < cMinimapTextureHeight; i++) {
			*lDest++ = mMinimapData[i][j].b*254; // B
			*lDest++ = mMinimapData[i][j].g*254; // G
			*lDest++ = mMinimapData[i][j].r*254; // R
			*lDest++ = mMinimapData[i][j].a*254; // A
		}
	}

	lPixelBuffer->unlock();
}

/*-----------------------------------------------------------------------------------------------*/

void Map::repaintMinimap(eMinimapMode pMode)
{
	mCurrentMinimapMode = pMode;

	if (pMode == eDefault)
		mMinimapData = mMinimapBackground;
	else if (pMode == eSupply)
		mMinimapData = mMinimapBackground;
	else if (pMode == eLines)
		mMinimapData = mMinimapBackground;
	else if (pMode == eResources)
		mMinimapData = mMinimapResourceOverlay;

	int lOffsetX = 0;
	int lOffsetY = 0;
	int lLargerSize = 0;

	if (mTerrain->getWidth() > mTerrain->getHeight()) {
		lOffsetY = (mTerrain->getWidth() - mTerrain->getHeight())/2;
		lLargerSize = mTerrain->getWidth();
	}
	else if (mTerrain->getWidth() < mTerrain->getHeight()) {
		lOffsetX = (mTerrain->getHeight() - mTerrain->getWidth())/2;
		lLargerSize = mTerrain->getHeight();
	} else {
		lLargerSize = mTerrain->getHeight();
	}

	for (unsigned int j = 0; j < mCollidables.size(); j++) {
		if (pMode == eDefault) {
			if ((mCollidables[j]->getType() != eStatic) 
				&& (mCollidables[j]->getType() != ePole)) {
				Ogre::ColourValue lColour = Ogre::ColourValue(0.0,0.0,0.0,0.0);

				if(mCollidables[j]->getType() == ePowerplant)
					lColour = Ogre::ColourValue(0.9,0.0,0.0,1.0);
				else if(mCollidables[j]->getType() == eCity)
					lColour = Ogre::ColourValue(0.9,0.9,0.9,1.0);
				else if(mCollidables[j]->getType() == eResource)
					lColour = Ogre::ColourValue(0.0,0.9,0.0,1.0);
				else if(mCollidables[j]->getType() == eCompany)
					lColour = Ogre::ColourValue(0.0,0.0,1.0,1.0);

				int x = ((float)(mCollidables[j]->getPosition().x + lOffsetX)
						/ (float)lLargerSize) * (float)(mMinimapWidth-1);

				int y = ((float)(mCollidables[j]->getPosition().y + lOffsetY)
						/ (float)lLargerSize) * (float)(mMinimapHeight-1);

				mMinimapData[x][y] = lColour;
				mMinimapData[x+1][y] = lColour;
				mMinimapData[x][y+1] = lColour;
				mMinimapData[x+1][y+1] = lColour;
			}
		}
		else if (pMode == eSupply) {
			Ogre::ColourValue lColour = Ogre::ColourValue(0.0,0.0,0.0,0.0);

			if ((mCollidables[j]->getType() != eStatic) 
				&& (mCollidables[j]->getType() != ePole)) {
				if (mCollidables[j]->getType() == ePowerplant) {
					float lStrain = (float)boost::dynamic_pointer_cast< Powerplant >(
						mCollidables[j])->getPowerLeft()/
						(float)boost::dynamic_pointer_cast< Powerplant >(
						mCollidables[j])->getPowerInMW();

					if (lStrain > 0.8)
						lColour = Ogre::ColourValue(1.0,0.0,0.0,1.0);
					else if ((lStrain <= 0.8) && (lStrain > 0.3))
						lColour = Ogre::ColourValue(1.0,1.0,0.0,1.0);
					else if (lStrain <= 0.3)
						lColour = Ogre::ColourValue(0.0,1.0,0.0,1.0);

					int x = ((float)(mCollidables[j]->getPosition().x + lOffsetX)
						/ (float)lLargerSize) * (float)(mMinimapWidth-1);

					int y = ((float)(mCollidables[j]->getPosition().y + lOffsetY)
						/ (float)lLargerSize) * (float)(mMinimapHeight-1);

					mMinimapData[x][y] = lColour;
					mMinimapData[x+1][y] = lColour;
					mMinimapData[x][y+1] = lColour;
					mMinimapData[x+1][y+1] = lColour;

					mMinimapData[x+2][y+2] = lColour;
					mMinimapData[x+1][y+2] = lColour;
					mMinimapData[x+2][y+1] = lColour;
					mMinimapData[x+2][y] = lColour;
					mMinimapData[x][y+2] = lColour;
				}
				else if (mCollidables[j]->getType() == eCity) {
					float lStrain = (float)boost::dynamic_pointer_cast< City >(
						mCollidables[j])->getPowerNeeded()/
						(float)boost::dynamic_pointer_cast<City>(
						mCollidables[j])->getConsumption();

					if (lStrain > 0.8)
						lColour = Ogre::ColourValue(1.0,0.0,0.0,1.0);
					else if ((lStrain <= 0.8) && (lStrain > 0.3))
						lColour = Ogre::ColourValue(1.0,1.0,0.0,1.0);
					else if (lStrain <= 0.3)
						lColour = Ogre::ColourValue(0.0,1.0,0.0,1.0);

					int x = ((float)(mCollidables[j]->getPosition().x + lOffsetX)
						/ (float)lLargerSize) * (float)(mMinimapWidth-1);

					int y = ((float)(mCollidables[j]->getPosition().y + lOffsetY)
						/ (float)lLargerSize) * (float)(mMinimapHeight-1);

					mMinimapData[x][y] = lColour;
					mMinimapData[x+1][y] = lColour;
					mMinimapData[x][y+1] = lColour;
					mMinimapData[x+1][y+1] = lColour;
				}
			}
		}
		else if ((pMode == eLines) && (mCollidables[j]->getType() == ePole)) {
			Ogre::ColourValue lColour = Ogre::ColourValue(0.0,0.0,0.0,0.0);

			float lStrain = (float)boost::dynamic_pointer_cast<Pole>(
				mCollidables[j])->getCapacityLeft()/
				(float)boost::dynamic_pointer_cast<Pole>(
				mCollidables[j])->getCapacity();

			if (lStrain > 0.99)
				lColour = Ogre::ColourValue(0.0,1.0,0.0,1.0);
			else if ((lStrain <= 0.99) && (lStrain > 0.6))
				lColour = Ogre::ColourValue(1.0,1.0,0.0,1.0);
			else if (lStrain <= 0.6)
				lColour = Ogre::ColourValue(1.0,0.0,0.0,1.0);

			int x = ((float)(mCollidables[j]->getPosition().x + lOffsetX)
				/ (float)lLargerSize) * (float)(mMinimapWidth-1);

			int y = ((float)(mCollidables[j]->getPosition().y + lOffsetY)
				/ (float)lLargerSize) * (float)(mMinimapHeight-1);

			mMinimapData[x][y] = lColour;
			mMinimapData[x+1][y] = lColour;
			mMinimapData[x][y+1] = lColour;
			mMinimapData[x+1][y+1] = lColour;
		}
	}

	updateMinimap();
}

/*-----------------------------------------------------------------------------------------------*/

void Map::paintResourceOverlay(void)
{
	int lOffsetX = 0;
	int lOffsetY = 0;
	int lLargerSize = 0;

	if (mTerrain->getWidth() > mTerrain->getHeight()) {
		lOffsetY = (mTerrain->getWidth() - mTerrain->getHeight())/2;
		lLargerSize = mTerrain->getWidth();
	}
	else if (mTerrain->getWidth() < mTerrain->getHeight()) {
		lOffsetX = (mTerrain->getHeight() - mTerrain->getWidth())/2;
		lLargerSize = mTerrain->getHeight();
	} else {
		lLargerSize = mTerrain->getHeight();
	}

	for (int k = 0; k < cMinimapRealWidth; k++) {
		for (int l = 0; l < cMinimapRealHeight; l++) {
			int lXOnRes = (float)((float)k / (float)cMinimapRealWidth) 
				* (float)((mTerrain->getWidth()/mScaleXAreas) - 1);
			int lYOnRes = (float)((float)l / (float)cMinimapRealHeight) 
				* (float)((mTerrain->getHeight()/mScaleZAreas) - 1);

			if (mResourceDefinition->getColourAt(lXOnRes, lYOnRes, 0) 
				== Ogre::ColourValue(0.0,0.0,0.0,1.0)) {
				continue;
			} else {
				Ogre::ColourValue lResColor = mResourceDefinition->getColourAt(lXOnRes, lYOnRes, 0);
				lResColor.a = 0.5;

				int lX = (float)((float)k / (float)cMinimapRealWidth) * mTerrain->getWidth();
				int lY = (float)((float)l / (float)cMinimapRealHeight) * mTerrain->getHeight();

				lX = ((float)(lX  + lOffsetX) / (float)lLargerSize) * (float)(cMinimapRealWidth-1);
				lY = ((float)(lY  + lOffsetY) / (float)lLargerSize) * (float)(cMinimapRealHeight-1);

				mMinimapResourceOverlay[lX][lY] = lResColor;
			}
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Map::createMinimap(void)
{
	mMinimapData.resize(cMinimapTextureWidth);
	mMinimapBackground.resize(cMinimapTextureWidth);
	mMinimapResourceOverlay.resize(cMinimapTextureWidth);

	for (int k = 0; k < cMinimapTextureWidth; k++) {
		mMinimapData[k].resize(cMinimapTextureHeight);
		mMinimapBackground[k].resize(cMinimapTextureHeight);
		mMinimapResourceOverlay[k].resize(cMinimapTextureHeight);
	}

	paintMinimapBackground();

	mMinimapResourceOverlay = mMinimapBackground;
	paintResourceOverlay();

	Ogre::ResourceGroupManager::getSingleton().createResourceGroup("MinimapGroup");

	mMinimap = Ogre::TextureManager::getSingleton().createManual(
		"minimap",
		"MinimapGroup",
		Ogre::TEX_TYPE_2D,
		cMinimapTextureWidth, cMinimapTextureHeight,
		1, 1,
		Ogre::PF_BYTE_BGRA,
		Ogre::TU_DEFAULT);
}

/*-----------------------------------------------------------------------------------------------*/

void Map::paintMinimapBackground(void)
{
	for (int i = 0; i < cMinimapTextureWidth; i++)
		for (int j = 0; j < cMinimapTextureHeight; j++)
			mMinimapBackground[i][j] = Ogre::ColourValue(0.0,0.0,0.0,0.0);
}

/*-----------------------------------------------------------------------------------------------*/

boost::shared_ptr< Terrain > Map::getTerrain(void)
{
	return mTerrain;
}

/*-----------------------------------------------------------------------------------------------*/

std::string Map::findNearestCity(Ogre::Vector2 pPosition)
{
	Ogre::Vector2 lDistance = Ogre::Vector2(100000, 100000);
	unsigned int lCityIndex;

	for (unsigned int i = 0; i < mCities.size(); i++) {
		if ((mCities[i]->getPosition() - pPosition).length() < lDistance.length()) {
			lDistance = mCities[i]->getPosition() - pPosition;
			lCityIndex = i;
		}
	}

	return mCities[lCityIndex]->getName() + " " + 
		toString(mCities[lCityIndex]->increaseNamedAfter());
}

/*-----------------------------------------------------------------------------------------------*/

bool Map::isOverResource(int pPositionX, int pPositionZ, eResourceType pType)
{
	Ogre::ColourValue lColour;

	if (pType == eCoal)
		lColour = Ogre::ColourValue(0.0,0.0,1.0,1.0);
	else if (pType == eGas)
		lColour = Ogre::ColourValue(1.0,0.0,0.0,1.0);
	else if (pType == eUranium)
		lColour = Ogre::ColourValue(0.0,1.0,0.0,1.0);
	else
		return true;

	if(mResourceDefinition->getColourAt(pPositionX / mScaleXAreas, pPositionZ / mScaleZAreas, 0)
    == lColour)
		return true;

	return false;
}

/*-----------------------------------------------------------------------------------------------*/

void Map::minimapModeChange(EventData* pEventData)
{
	if(static_cast< EventArg< int >* >(pEventData)->mData1 == -1)
		repaintMinimap(mCurrentMinimapMode);
	else
		repaintMinimap((eMinimapMode)static_cast< EventArg< int >* >(pEventData)->mData1);
}

/*-----------------------------------------------------------------------------------------------*/

boost::shared_ptr< Collidable > Map::getCityByID(int pID)
{
	for (unsigned int i = 0; i < mCities.size(); i++)
		if (toNumber< int >(mCities[i]->getID()) == pID)
			return mCities[i];

	return mCities[0]; // TODO: should not happen (?)
}

/*-----------------------------------------------------------------------------------------------*/

void Map::autoremoveFromSerializedBuilding(Ogre::Rect pRect)
{
	// deprecated
}
