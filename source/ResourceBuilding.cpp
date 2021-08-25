#include "StdAfx.h"
#include "ResourceBuilding.h"
#include "Company.h"
#include "GUI.h"
#include "PowerNet.h"
#include "Terrain.h"

/*-----------------------------------------------------------------------------------------------*/

ResourceBuilding::ResourceBuilding(Ogre::Vector2 pPosition, eCompanyObjectType pType,
								   int pCounter, boost::shared_ptr<Company> pCompany)
: mPosition(pPosition),
  mType(pType),
  mInheritSettings(true),
  mMaintenance(100),
	mDestroyed(false),
  mDamaged(false),
  mCounter(pCounter),
  mCondition(100.0),
	mCompany(pCompany),
  mRepairDays(0)
{
	REGISTER_CALLBACK(eWeekPassed, ResourceBuilding::weekPassed);
	REGISTER_CALLBACK(eDayPassed, ResourceBuilding::dayPassed);

	if(mType == eREUraniumSmall)
		mName = StrLoc::get()->Uranium() + " " + toString(pCounter) 
		+ " (" + StrLoc::get()->Small() + ")";
	else if(mType == eREUraniumLarge)
		mName = StrLoc::get()->Uranium() + " " + toString(pCounter) 
		+ " (" + StrLoc::get()->Large() + ")";
	else if(mType == eREGasLarge)
		mName = StrLoc::get()->Gas() + " " + toString(pCounter) 
		+ " (" + StrLoc::get()->Large() + ")";
	else if(mType == eREGasSmall)
		mName = StrLoc::get()->Gas() + " " + toString(pCounter) 
		+ " (" + StrLoc::get()->Small() + ")";
	else if(mType == eRECoalLarge)
		mName = StrLoc::get()->Coal() + " " + toString(pCounter) 
		+ " (" + StrLoc::get()->Large() + ")";
	else if(mType == eRECoalSmall)
		mName = StrLoc::get()->Coal() + " " + toString(pCounter) 
		+ " (" + StrLoc::get()->Small() + ")";
}

/*-----------------------------------------------------------------------------------------------*/

bool ResourceBuilding::isAutoremove(void)
{
	return false;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<RotatedRect> ResourceBuilding::getGroundRects()
{
	std::vector<RotatedRect> lTemp;
	lTemp.push_back(RotatedRect(Ogre::Rect(mPosition.x - 12, mPosition.y - 12,
									mPosition.x + 12, mPosition.y + 12)));
	return lTemp;
}

/*-----------------------------------------------------------------------------------------------*/

void ResourceBuilding::addToSceneManager(Ogre::SceneManager* pSceneManager,
										 boost::shared_ptr< Terrain > pTerrain)
{
	if (mType == eREUraniumSmall) {
		mEntity = pSceneManager->createEntity("re" + getID(), "re_uranium_small.mesh");
		mEntityFloor = pSceneManager->createEntity("re_floor" + getID(),
			"re_uranium_small_ground.mesh");
	}
	else if (mType == eREUraniumLarge) {
		mEntity = pSceneManager->createEntity("re" + getID(), "re_uranium_large.mesh");
		mEntityFloor = pSceneManager->createEntity("re_floor" + getID(),
			"re_uranium_large_ground.mesh");
	}
	else if (mType == eREGasSmall) {
		mEntity = pSceneManager->createEntity("re" + getID(), "re_gas_small.mesh");
		mEntityFloor = pSceneManager->createEntity("re_floor" + getID(),
			"re_gas_small_ground.mesh");
	}
	else if (mType == eREGasLarge) {
		mEntity = pSceneManager->createEntity("re" + getID(), "re_gas_large.mesh");
		mEntityFloor = pSceneManager->createEntity("re_floor" + getID(),
			"re_gas_large_ground.mesh");
	}
	else if (mType == eRECoalSmall) {
		mEntity = pSceneManager->createEntity("re" + getID(), "re_coal_small.mesh");
		mEntityFloor = pSceneManager->createEntity("re_floor" + getID(),
			"re_coal_small_ground.mesh");
	}
	else if (mType == eRECoalLarge) {
		mEntity = pSceneManager->createEntity("re" + getID(), "re_coal_large.mesh");
		mEntityFloor = pSceneManager->createEntity("re_floor" + getID(),
			"re_coal_large_ground.mesh");
	}

	mEntity->setCastShadows(false);
	mEntityFloor->setCastShadows(false);

	float lHeight = pTerrain->getHighestFromQuad(mPosition.x, mPosition.y,
		cStructureBorderLength[mType]);

	pTerrain->flattenQuad(mPosition, cStructureBorderLength[mType], lHeight);
	pTerrain->update();

	mNode = pSceneManager->getRootSceneNode()->createChildSceneNode("res_node" + getID());
	mNode->attachObject(mEntity);
	mNode->attachObject(mEntityFloor);
	mNode->setScale(0.85, 0.85, 0.85);
	mNode->translate(Ogre::Vector3( mPosition.x, pTerrain->
		getHeightAt(mPosition.x,mPosition.y)+cFloorDistance,mPosition.y));
}

/*-----------------------------------------------------------------------------------------------*/

void ResourceBuilding::removeFromSceneManager(void)
{
	mNode->detachAllObjects();
	mNode->getCreator()->destroyEntity(mEntity);
	mNode->getCreator()->destroyEntity(mEntityFloor);

	mDestroyed = true;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 ResourceBuilding::getPosition(void)
{
	return mPosition;
}

/*-----------------------------------------------------------------------------------------------*/

bool ResourceBuilding::isSelectable(void)
{
	return true;
}

/*-----------------------------------------------------------------------------------------------*/

eGameObjectType ResourceBuilding::getType(void)
{
	return eResource;
}

/*-----------------------------------------------------------------------------------------------*/

void ResourceBuilding::highlight(bool pHighlight)
{
	if (mDestroyed)
		return;

	if (!mEntity)
		return;

	if (pHighlight) {
		mEntityFloor->setVisible(false);
		mEntity->setMaterialName("highlight");
	}
	else {	
		mEntityFloor->setVisible(true);

		if (mType == eREUraniumSmall)
			mEntity->setMaterialName("re_uran_small");
		else if (mType == eREUraniumLarge)
			mEntity->setMaterialName("re_uran_large");
		else if (mType == eREGasSmall)
			mEntity->setMaterialName("re_gas_small");
		else if (mType == eREGasLarge)
			mEntity->setMaterialName("re_gas_large");
		else if (mType == eRECoalSmall)
			mEntity->setMaterialName("re_coal_small");
		else if (mType == eRECoalLarge)
			mEntity->setMaterialName("re_coal_large");
	}
}

/*-----------------------------------------------------------------------------------------------*/

int ResourceBuilding::getOperatingCosts(void)
{
	return GameConfig::getInt(GameConfig::cCOTS(mType), cCOVOpCost);
}

/*-----------------------------------------------------------------------------------------------*/

int ResourceBuilding::getEnvironmentalCosts(void)
{
	return GameConfig::getInt(GameConfig::cCOTS(mType), cCOVEnvCost);
}

/*-----------------------------------------------------------------------------------------------*/

void ResourceBuilding::weekPassed(EventData *pData)
{
	if (mDestroyed)
		return;

	if (!mDamaged) {
		int lAmount = GameConfig::getInt(GameConfig::cCOTS(mType), cCOVRate);

		if ((mType == eREUraniumSmall) || (mType == eREUraniumLarge)) {
			EventHandler::raiseEvent(eChangeResource, 
				new EventArg< int >((int)eUranium, lAmount));
		}
		else if ((mType == eREGasLarge) || (mType == eREGasSmall)) {
			EventHandler::raiseEvent(eChangeResource, 
				new EventArg< int >((int)eGas, lAmount));
		}
		else if ((mType == eRECoalLarge) || (mType == eRECoalSmall)) {
			EventHandler::raiseEvent(eChangeResource, 
				new EventArg< int >((int)eCoal, lAmount));
		}
	}

	double lConditionFactor = GameConfig::getDouble("BuildingConditionFactor");

	if (mInheritSettings)
		mCondition += ((float)(mCompany->getMaintenance())-101.0)/75.0*lConditionFactor;
	else
		mCondition += ((float)(mMaintenance)-101.0)/75.0*lConditionFactor;

	if (mCondition > 100)
		mCondition = 100;

	if (mCondition < 0)
		mCondition = 0;
}

/*-----------------------------------------------------------------------------------------------*/

void ResourceBuilding::dayPassed(EventData* pData)
{
	if (mRepairDays != 0) {
		mRepairDays--;
		if (mRepairDays == 0)
			repairNow();
	}
}

/*-----------------------------------------------------------------------------------------------*/

void ResourceBuilding::setDamaged(void)
{
	mDamaged = true;
}

/*-----------------------------------------------------------------------------------------------*/

void ResourceBuilding::repairNow(void)
{
	mDamaged = false;
	mCondition = 100;

	TickerMessage lMessage;
	lMessage.mMessage = StrLoc::get()->RepairsComplete1() + mName +
    StrLoc::get()->RepairsComplete2();
	lMessage.mPointOfInterest = mPosition;
	lMessage.mUrgent = false;
	lMessage.mID = 0;

	EventHandler::raiseEvent(eTickerMessage, new EventArg< TickerMessage >(lMessage));
}

/*-----------------------------------------------------------------------------------------------*/

void ResourceBuilding::repair(size_t pMonths)
{
	mRepairDays = pMonths*30;
}

/*-----------------------------------------------------------------------------------------------*/

bool ResourceBuilding::getDamaged(void)
{
	return mDamaged;
}