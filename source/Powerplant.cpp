#include "StdAfx.h"
#include "Powerplant.h"
#include "Event.h"
#include "GUI.h"
#include "City.h"
#include "Powerplant.h"
#include "Company.h"
#include "TransformatorSet.h"
#include "Terrain.h"

/*-----------------------------------------------------------------------------------------------*/

Powerplant::Powerplant(Ogre::Vector2 pPosition, eCompanyObjectType pType, std::string pName,
					   boost::shared_ptr< Company > pCompany, int pConstructionYear)
: mPosition(pPosition),
  mType(pType),
  mNode(0),
  mEntity(0),
  mName(pName),
  mPowerUsed(0),
	mWasFullCapacity(false),
  mPowerAvailable(0),
  mDestroyed(false),
  mInheritSettings(true),
	mMaintenance(GameConfig::getInt("PPDefaultMaintenance")),
  mCompany(pCompany),
	mCondition(GameConfig::getDouble("PPDefaultCondition")),
  mConstructionYear(pConstructionYear),
	mDamaged(false),
  mRepairDays(0)
{
	REGISTER_CALLBACK(eWeekPassed, Powerplant::weekPassed);
	REGISTER_CALLBACK(eResourceGone, Powerplant::resourceGone);
	REGISTER_CALLBACK(eResourceGain, Powerplant::resourceGain);
	REGISTER_CALLBACK(eNewWeather, Powerplant::newWeather);
	REGISTER_CALLBACK(eDayPassed, Powerplant::dayPassed);

	setDefaultPowerAvailable();
}

/*-----------------------------------------------------------------------------------------------*/

bool Powerplant::isAutoremove(void)
{
	return false;
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::newWeather(EventData* pData)
{
	if (getDamaged())
		return;

	mCurrentWeather = static_cast<EventArg<Weather>*>(pData)->mData1;

	double lWindEfficiency = 1.0;
	if (mCompany->getResearchSet().mResearched[4])
		lWindEfficiency = GameConfig::getDouble("WindEfficiencyFactor");

	double lSolarEfficiency = 1.0;
	if (mCompany->getResearchSet().mResearched[17])
		lSolarEfficiency = GameConfig::getDouble("SolarEfficiencyFactor");

	double lStrongWindFactor = GameConfig::getDouble("StrongWindPowerFactor");
	double lNormalWindFactor = GameConfig::getDouble("NormalWindPowerFactor");
	double lNoWindFactor = GameConfig::getDouble("NoWindPowerFactor");

	double lLongSunFactor = GameConfig::getDouble("LongSunFactor");
	double lAverageSunFactor = GameConfig::getDouble("AverageSunFactor");
	double lLowSunFactor = GameConfig::getDouble("LowSunFactor");

	if (mType == ePPWindSmall) {
		if(mCurrentWeather.mWindSpeed == eStrongSpeed)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lStrongWindFactor*lWindEfficiency;
		else if(mCurrentWeather.mWindSpeed == eNormalSpeed)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lNormalWindFactor*lWindEfficiency;
		else if(mCurrentWeather.mWindSpeed == eNoSpeed)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lNoWindFactor*lWindEfficiency;
	}
	else if (mType == ePPWindLarge) {
		if(mCurrentWeather.mWindSpeed == eStrongSpeed)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lStrongWindFactor*lWindEfficiency;
		else if(mCurrentWeather.mWindSpeed == eNormalSpeed)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lNormalWindFactor*lWindEfficiency;
		else if(mCurrentWeather.mWindSpeed == eNoSpeed)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lNoWindFactor*lWindEfficiency;
	}
	else if (mType == ePPSolarSmall) {
		if(mCurrentWeather.mSunshineDuration == eVeryLong)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lLongSunFactor*lSolarEfficiency;
		else if(mCurrentWeather.mSunshineDuration == eAverage)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lAverageSunFactor*lSolarEfficiency;
		else if(mCurrentWeather.mSunshineDuration == eLow)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lLowSunFactor*lSolarEfficiency;
	}
	else if (mType == ePPSolarLarge) {
		if(mCurrentWeather.mSunshineDuration == eVeryLong)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lLongSunFactor*lSolarEfficiency;
		else if(mCurrentWeather.mSunshineDuration == eAverage)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lAverageSunFactor*lSolarEfficiency;
		else if(mCurrentWeather.mSunshineDuration == eLow)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lLowSunFactor*lSolarEfficiency;
	}
	else if (mType == ePPSolarUpdraft) {
		if(mCurrentWeather.mSunshineDuration == eVeryLong)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lLongSunFactor*lSolarEfficiency;
		else if(mCurrentWeather.mSunshineDuration == eAverage)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lAverageSunFactor*lSolarEfficiency;
		else if(mCurrentWeather.mSunshineDuration == eLow)
			mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType),
      cCOVPower)*lLowSunFactor*lSolarEfficiency;
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::setDefaultPowerAvailable()
{
	mPowerAvailable = GameConfig::getInt(GameConfig::cCOTS(mType), cCOVPower);
}

/*-----------------------------------------------------------------------------------------------*/

int Powerplant::getOperatingCosts(void)
{
	return GameConfig::getInt(GameConfig::cCOTS(mType), cCOVOpCost);
}

/*-----------------------------------------------------------------------------------------------*/

int Powerplant::getEnvironmentalCosts(void)
{
	return GameConfig::getInt(GameConfig::cCOTS(mType), cCOVEnvCost);
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<Ogre::Vector3> Powerplant::getLineAttachingPoints(
  Ogre::Vector2 pAdjacentVertexPosition, bool pCheckOnly)
{
	return mTransSet->use(pAdjacentVertexPosition, pCheckOnly);
}

/*-----------------------------------------------------------------------------------------------*/

std::vector< RotatedRect > Powerplant::getGroundRects()
{
	std::vector< RotatedRect > lTemp;
	lTemp.push_back(Ogre::Rect(mPosition.x - 12, mPosition.y - 12, 
						 mPosition.x + 12, mPosition.y + 12));
	return lTemp;
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::addPowerUsed(int pAmount)
{
	mPowerUsed += pAmount;

	if (mPowerUsed == getPowerInMW() && !mWasFullCapacity) {
		TickerMessage lMessage;
		lMessage.mMessage = StrLoc::get()->PowerplantText() + " " 
			+ mName + StrLoc::get()->FullCapacity();
		lMessage.mPointOfInterest = mPosition;
		lMessage.mUrgent = false;
		lMessage.mID = 0;
		lMessage.mDetail = StrLoc::get()->FullCapacityDetail();

		EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));

		mWasFullCapacity = true;
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::updateAnimations(float pElapsedTime)
{
	double lRotationSpeed = GameConfig::getDouble("PPWindRotationSpeed");
	
	if ((mType == ePPWindSmall) || (mType == ePPWindOffshore)) {
		mRotorNode->rotate(Ogre::Vector3(0.0, -1.0, 0.0), Ogre::Radian(-pElapsedTime*lRotationSpeed));
	}
	else if (mType == ePPWindLarge) {
		mRotorNode->rotate(Ogre::Vector3(0.0, -1.0, 0.0), Ogre::Radian(-pElapsedTime*lRotationSpeed));
		mRotorNode2->rotate(Ogre::Vector3(0.0, -1.0, 0.0), Ogre::Radian(-pElapsedTime*lRotationSpeed));
		mRotorNode3->rotate(Ogre::Vector3(0.0, -1.0, 0.0), Ogre::Radian(-pElapsedTime*lRotationSpeed));
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::addToSceneManager(Ogre::SceneManager* pSceneManager,
                                   boost::shared_ptr<Terrain> pTerrain)
{
	if (mType == ePPNuclearSmall) {
		mEntity = pSceneManager->createEntity("powerplant" + getID(), "nuc_small.mesh");
		mEntityFloor = pSceneManager->createEntity("powerplantground" 
			+ getID(), "nuc_small_ground.mesh");

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());

		mPartSys = pSceneManager->createParticleSystem("smokesys1" + getID(),
			"SmokeSystemLowCap");
		Ogre::SceneNode* lPartNode = mNode->createChildSceneNode("part_nuc_1" + getID());
		lPartNode->translate(11.0,2.25,-7.5);
		lPartNode->attachObject(mPartSys);

		mPartSys->setRenderQueueGroup(99);
	}
	else if (mType == ePPNuclearLarge) {
		mEntity = pSceneManager->createEntity("powerplant" + getID(), "pp_nuclear_large.mesh");
		mEntityFloor = pSceneManager->createEntity("powerplantground" 
			+ getID(), "pp_nuclear_large_ground.mesh");

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());

		mNode->setScale(0.65,0.65,0.65);
		mPartSys = pSceneManager->createParticleSystem("smokesys1" + getID(),
			"SmokeSystemLowCap");
		Ogre::SceneNode* lPartNode = mNode->createChildSceneNode("part_nuc_1" + getID());
		lPartNode->translate(12.31,6.915,+6.127);
		lPartNode->attachObject(mPartSys);

		mPartSys->setRenderQueueGroup(99);

		mPartSys2 = pSceneManager->createParticleSystem("smokesys2" + getID(),
			"SmokeSystemLowCap");
		Ogre::SceneNode* lPartNode2 = mNode->createChildSceneNode("part_nuc_2" + getID());
		lPartNode2->translate(15.936,6.915,-13.49);
		lPartNode2->attachObject(mPartSys2);

		mPartSys2->setRenderQueueGroup(99);
	}
	else if (mType == ePPCoalSmall) {
		mEntity = pSceneManager->createEntity("powerplant" + getID(), "pp_coal_small.mesh");
		mEntityFloor = pSceneManager->createEntity("powerplantground" 
			+ getID(), "pp_coal_small_ground.mesh");

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());

		mPartSys = pSceneManager->createParticleSystem("smokesys1" + getID(),
			"SmokeSystemCoal");
		Ogre::SceneNode* lPartNode = mNode->createChildSceneNode("part_coal1" + getID());
		lPartNode->translate(1.2,12.5,4.1);
		lPartNode->attachObject(mPartSys);

		mPartSys2 = pSceneManager->createParticleSystem("smokesys2" + getID(),
			"SmokeSystemCoal");
		Ogre::SceneNode* lPartNode2 = mNode->createChildSceneNode("part_coal2" + getID());
		lPartNode2->translate(1.2,12.5,11.0);
		lPartNode2->attachObject(mPartSys2);

		mPartSys->setRenderQueueGroup(99);
		mPartSys2->setRenderQueueGroup(99);
	}
	else if (mType == ePPCoalLarge) {
		mEntity = pSceneManager->createEntity("powerplant" + getID(), "pp_coal_large.mesh");
		mEntityFloor = pSceneManager->createEntity("powerplantground" 
			+ getID(), "pp_coal_large_ground.mesh");

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());

		mPartSys = pSceneManager->createParticleSystem("smokesys1" + getID(),
			"SmokeSystemCoal");
		Ogre::SceneNode* lPartNode = mNode->createChildSceneNode("part_coal1" + getID());
		lPartNode->translate(11.6,15.0,-6.6);
		lPartNode->attachObject(mPartSys);

		mPartSys2 = pSceneManager->createParticleSystem("smokesys2" + getID(),
			"SmokeSystemCoal");
		Ogre::SceneNode* lPartNode2 = mNode->createChildSceneNode("part_coal2" + getID());
		lPartNode2->translate(11.6,15.0,1.9);
		lPartNode2->attachObject(mPartSys2);

		mPartSys3 = pSceneManager->createParticleSystem("smokesys3" + getID(),
			"SmokeSystemCoal");
		Ogre::SceneNode* lPartNode3 = mNode->createChildSceneNode("part_coal3" + getID());
		lPartNode3->translate(11.6,15.0,10.5);
		lPartNode3->attachObject(mPartSys3);

		mPartSys->setRenderQueueGroup(99);
		mPartSys2->setRenderQueueGroup(99);
		mPartSys3->setRenderQueueGroup(99);
	}
	else if (mType == ePPWindSmall) {
		mEntity = pSceneManager->createEntity("powerplant" + getID(), "pp_wind_small.mesh");
		mEntityFloor = pSceneManager->createEntity("powerplantground" 
			+ getID(), "wind_small_ground.mesh");

		mEntityRotor = pSceneManager->createEntity("powerplantrot" + getID(),
			"rotor_small.mesh");

		mEntityRotor->getMesh()->buildEdgeList();

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());
		mRotorNode = mNode->createChildSceneNode("Powerplantrot_node" + getID());
		mRotorNode->attachObject(mEntityRotor);
		mRotorNode->rotate(Ogre::Vector3(1.0, 0.0, 0.0), Ogre::Radian(-Ogre::Math::HALF_PI));
		mRotorNode->translate(0.04, 24.3, -0.4);

		mNode->setScale(0.75,0.75,0.75);
	}
	else if (mType == ePPWindLarge) {
		mEntity = pSceneManager->createEntity("powerplant" + getID(), "pp_wind_large.mesh");
		mEntityFloor = pSceneManager->createEntity("powerplantground" 
			+ getID(), "pp_wind_large_ground.mesh");

		mEntityRotor = pSceneManager->createEntity("powerplantrot" + getID(),
			"rotor_large_a.mesh");
		mEntityRotor2 = pSceneManager->createEntity("powerplantrot2" + getID(),
			"rotor_large_b.mesh");
		mEntityRotor3 = pSceneManager->createEntity("powerplantrot3" + getID(),
			"rotor_large_c.mesh");

		mEntityRotor->getMesh()->buildEdgeList();
		mEntityRotor2->getMesh()->buildEdgeList();
		mEntityRotor3->getMesh()->buildEdgeList();

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());

		mNode->setScale(0.75,0.75,0.75);

		mRotorNode = mNode->createChildSceneNode("Powerplantrot_node" + getID());
		mRotorNode->attachObject(mEntityRotor);
		mRotorNode->rotate(Ogre::Vector3(1.0, 0.0, 0.0), Ogre::Radian(-Ogre::Math::HALF_PI));
		mRotorNode->translate(0.0968914, 31.8108, 4.46678);

		mRotorNode2 = mNode->createChildSceneNode("Powerplantrot_node2" + getID());
		mRotorNode2->attachObject(mEntityRotor2);
		mRotorNode2->rotate(Ogre::Vector3(1.0, 0.0, 0.0), Ogre::Radian(-Ogre::Math::HALF_PI));
		mRotorNode2->translate(-15.6651, 24.3254, -13.1099);

		mRotorNode3 = mNode->createChildSceneNode("Powerplantrot_node3" + getID());
		mRotorNode3->attachObject(mEntityRotor3);
		mRotorNode3->rotate(Ogre::Vector3(1.0, 0.0, 0.0), Ogre::Radian(-Ogre::Math::HALF_PI));
		mRotorNode3->translate(17.7568, 24.3255, -7.0941);
	}
	else if (mType == ePPWindOffshore) {
		mEntity = pSceneManager->createEntity("powerplant" + getID(), "pp_wind_offshore.mesh");
		mEntityFloor = pSceneManager->createEntity("powerplantground" 
			+ getID(), "pp_wind_offshore_ground.mesh");

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());

		mEntityRotor = pSceneManager->createEntity("powerplantrot" + getID(),
			"rotor_offshore.mesh");

		mEntityRotor->getMesh()->buildEdgeList();

		mRotorNode = mNode->createChildSceneNode("Powerplantrot_node" + getID());
		mRotorNode->attachObject(mEntityRotor);
		mRotorNode->rotate(Ogre::Vector3(1.0, 0.0, 0.0), Ogre::Radian(Ogre::Math::HALF_PI));
		mRotorNode->translate(0.097, 37.892, -1.969);

		mNode->setScale(0.75,0.75,0.75);
	}
	else if (mType == ePPSolarLarge) {
		mEntity = pSceneManager->createEntity("powerplant" + getID(), "pp_solar_large.mesh");
		mEntityFloor = pSceneManager->createEntity("powerplantground"
			+ getID(), "pp_solar_large_ground.mesh");

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());
		mNode->setScale(0.6,0.6,0.6);
	}
	else if (mType == ePPNuclearFusion) {
		mEntity = pSceneManager->createEntity("powerplant"
			+ getID(), "pp_nuclear_fusion.mesh");
		mEntityFloor = pSceneManager->createEntity("powerplantground" 
			+ getID(), "pp_nuclear_fusion_ground.mesh");

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());
		mNode->setScale(0.6,0.6,0.6);
	}
	else if (mType == ePPGasSmall) {
		mEntity = pSceneManager->createEntity("powerplant" + getID(), "pp_gas_small.mesh");
		mEntityFloor = pSceneManager->createEntity("powerplantground" 
			+ getID(), "pp_gas_small_ground.mesh");

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());

		mPartSys = pSceneManager->createParticleSystem("smokesys1" + getID(),
			"SmokeSystemGas");
		Ogre::SceneNode* lPartNode = mNode->createChildSceneNode("part_gas1" + getID());
		lPartNode->translate(5.4761,8.9619,0.110319);
		lPartNode->attachObject(mPartSys);

		mPartSys->setRenderQueueGroup(99);
	}
	else if (mType == ePPGasLarge) {
		mEntity = pSceneManager->createEntity("powerplant" + getID(), "pp_gas_large.mesh");
		mEntityFloor = pSceneManager->createEntity("powerplantground" 
			+ getID(), "pp_gas_large_ground.mesh");

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());

		mPartSys = pSceneManager->createParticleSystem("smokesys1" + getID(),
			"SmokeSystemGas");
		Ogre::SceneNode* lPartNode = mNode->createChildSceneNode("part_gas1" + getID());
		lPartNode->translate(1.78783,6.861,-9.58981);
		lPartNode->attachObject(mPartSys);

		mPartSys2 = pSceneManager->createParticleSystem("smokesys2" + getID(),
			"SmokeSystemGas");
		Ogre::SceneNode* lPartNode2 = mNode->createChildSceneNode("part_gas2" + getID());
		lPartNode2->translate(-1.31053,6.861,-9.58981);
		lPartNode2->attachObject(mPartSys2);

		mPartSys->setRenderQueueGroup(99);
		mPartSys2->setRenderQueueGroup(99);
	}
	else if (mType == ePPBio) {
		mEntity = pSceneManager->createEntity("powerplant" + getID(), "pp_bio.mesh");
		mEntityFloor = pSceneManager->createEntity("powerplantground" 
			+ getID(), "pp_bio_ground.mesh");

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());

		mPartSys = pSceneManager->createParticleSystem("smokesys1" + getID(),
			"SmokeSystemGas");
		Ogre::SceneNode* lPartNode = mNode->createChildSceneNode("part_bio1" + getID());
		lPartNode->translate(-2.40535,6.94559,-0.23922);
		lPartNode->attachObject(mPartSys);

		mPartSys->setRenderQueueGroup(99);
	} else {
		mEntity = pSceneManager->createEntity("powerplant" + getID(), cMeshFilename[mType]);
		mEntityFloor = pSceneManager->createEntity("powerplantground" 
			+ getID(), cMeshFilename[mType].substr(0, cMeshFilename[mType].find(".")) 
			+ "_ground.mesh");

		mNode = pSceneManager->getRootSceneNode()->
			createChildSceneNode("Powerplant_node" + getID());

		mNode->setScale(cStructureScale[mType]);
	}

	mEntity->setCastShadows(false);
	mEntityFloor->setCastShadows(false);

	float lHeight = pTerrain->getHighestFromQuad(mPosition.x, mPosition.y,
		cStructureBorderLength[mType]);

	mNode->attachObject(mEntity);
	mNode->attachObject(mEntityFloor);
	mNode->translate( Ogre::Vector3( mPosition.x, lHeight + cFloorDistance, mPosition.y));
	
	pTerrain->flattenQuad(mPosition, cStructureBorderLength[mType], lHeight);
	pTerrain->update();
	
	updateParticleSystems();

	mTransSet.reset(new TransformatorSet(pSceneManager, mNode->getPosition(),
		ePowerplantTrans, mType));
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::removeFromSceneManager(void)
{
	mNode->detachAllObjects();
	mNode->getCreator()->destroyEntity(mEntity);
	mNode->getCreator()->destroyEntity(mEntityFloor);

	if (mType == ePPNuclearSmall) {
		mPartSys->getEmitter(0)->setTimeToLive(0, 0);
		mPartSys->setParticleQuota(0);
		mPartSys->setVisible(false);
	}
	else if (mType == ePPNuclearLarge) {
		mPartSys->getEmitter(0)->setTimeToLive(0, 0);
		mPartSys->setParticleQuota(0);
		mPartSys->setVisible(false);
		mPartSys2->getEmitter(0)->setTimeToLive(0, 0);
		mPartSys2->setParticleQuota(0);
		mPartSys2->setVisible(false);
	}
	else if (mType == ePPCoalSmall) {
		mPartSys->getEmitter(0)->setTimeToLive(0, 0);
		mPartSys->setParticleQuota(0);
		mPartSys->setVisible(false);
		mPartSys2->getEmitter(0)->setTimeToLive(0, 0);
		mPartSys2->setParticleQuota(0);
		mPartSys2->setVisible(false);
	}
	else if (mType == ePPCoalLarge) {
		mPartSys->getEmitter(0)->setTimeToLive(0, 0);
		mPartSys->setParticleQuota(0);
		mPartSys->setVisible(false);
		mPartSys2->getEmitter(0)->setTimeToLive(0, 0);
		mPartSys2->setParticleQuota(0);
		mPartSys2->setVisible(false);
		mPartSys3->getEmitter(0)->setTimeToLive(0, 0);
		mPartSys3->setParticleQuota(0);
		mPartSys3->setVisible(false);
	}
	else if ((mType == ePPWindSmall) || (mType == ePPWindOffshore)) {
		mNode->getCreator()->destroyEntity(mEntityRotor);
	}
	else if (mType == ePPWindLarge) {
		mNode->getCreator()->destroyEntity(mEntityRotor);
		mNode->getCreator()->destroyEntity(mEntityRotor2);
		mNode->getCreator()->destroyEntity(mEntityRotor3);
	}
	else if (mType == ePPGasSmall) {
		mPartSys->getEmitter(0)->setTimeToLive(0, 0);
		mPartSys->setParticleQuota(0);
		mPartSys->setVisible(false);
	}
	else if (mType == ePPGasLarge) {
		mPartSys->getEmitter(0)->setTimeToLive(0, 0);
		mPartSys->setParticleQuota(0);
		mPartSys->setVisible(false);
		mPartSys2->getEmitter(0)->setTimeToLive(0, 0);
		mPartSys2->setParticleQuota(0);
		mPartSys2->setVisible(false);
	}
	else if (mType == ePPBio) {
		mPartSys->getEmitter(0)->setTimeToLive(0, 0);
		mPartSys->setParticleQuota(0);
		mPartSys->setVisible(false);
	}

	mTransSet->clear();

	mDestroyed = true;
}

/*-----------------------------------------------------------------------------------------------*/

int Powerplant::getPowerInMW(void)
{
	return mPowerAvailable;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 Powerplant::getPosition(void)
{
	return mPosition;
}

/*-----------------------------------------------------------------------------------------------*/

bool Powerplant::isSelectable(void)
{
	return true;
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::addReceiver(std::string pReceiver)
{
	bool lFound = false;

	for (size_t i = 0; i < mReceivers.size(); i++) {
		if (pReceiver.substr(0, pReceiver.find(":")) 
			    == mReceivers[i].substr(0, mReceivers[i].find(":"))) {
			mReceivers[i] = mReceivers[i].substr(0, mReceivers[i].find(":") + 2)
				+ toString(toNumber<int>(pReceiver.substr(pReceiver.find(":")+1))
				+ toNumber<int>(mReceivers[i].substr(mReceivers[i].find(":")+1))) + "MW";
			
			lFound = true;
		}
	}

	if(!lFound)
		mReceivers.push_back(pReceiver);

	updateParticleSystems();
}

/*-----------------------------------------------------------------------------------------------*/

std::string Powerplant::getReceivers(void)
{
	if (mReceivers.size() > 0) {
		std::string lReturn;

		for (size_t i = 0; i < mReceivers.size(); i++) {
			if (i < mReceivers.size() - 1)
				lReturn = lReturn + mReceivers[i] + "\n";
			else
				lReturn = lReturn + mReceivers[i];

			if (i == 4) {
				if (mReceivers.size() > 5)
					return lReturn + StrLoc::get()->CitySeeNet();
				else
					return lReturn + "\n..." + StrLoc::get()->CitySeeNet();
			}
		}

		return lReturn;
	} else {
		return StrLoc::get()->CityNone();
	}
}

/*-----------------------------------------------------------------------------------------------*/

eGameObjectType Powerplant::getType(void)
{
	return ePowerplant;
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::highlight(bool pHighlight)
{
	if (mDestroyed)
		return;

	if (pHighlight) {
		mEntity->setMaterialName("highlight");
		mEntityFloor->setVisible(false);
	} else {
		if (mType == ePPNuclearLarge)
			mEntity->setMaterialName("pp_nuclear_large");
		if (mType == ePPNuclearSmall)
			mEntity->setMaterialName("nuc_small");
		else if (mType == ePPWindSmall)
			mEntity->setMaterialName("wind_small");
		else if (mType == ePPWindLarge)
			mEntity->setMaterialName("pp_wind_large");
		else if (mType == ePPWindOffshore)
			mEntity->setMaterialName("pp_wind_offshore");
		else if (mType == ePPSolarSmall)
			mEntity->setMaterialName("pp_solar_small");
		else if (mType == ePPSolarLarge)
			mEntity->setMaterialName("pp_solar_large");
		else if (mType == ePPCoalSmall)
			mEntity->setMaterialName("pp_coal_small");
		else if (mType == ePPCoalLarge)
			mEntity->setMaterialName("pp_coal_large");
		else if (mType == ePPGasSmall)
			mEntity->setMaterialName("pp_gas_small");
		else if (mType == ePPGasLarge)
			mEntity->setMaterialName("pp_gas_large");
		else if (mType == ePPBio)
			mEntity->setMaterialName("pp_bio");
		else if (mType == ePPSolarUpdraft)
			mEntity->setMaterialName("pp_solar_updraft");
		else if (mType == ePPNuclearFusion)
			mEntity->setMaterialName("pp_nuclear_fusion");
		else if (mType == ePPWater)
			mEntity->setMaterialName("pp_water");

		mEntityFloor->setVisible(true);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::updateParticleSystems(void)
{
	if (mDestroyed)
		return;

	if (mType == ePPNuclearSmall) {
		if (getPowerInMW() > 0) {
			float lStrain = (float)mPowerUsed/(float)getPowerInMW();

			mPartSys->getEmitter(0)->setTimeToLive((3.0 * lStrain) + 1, (4.0 * lStrain) + 1);
			mPartSys->setParticleQuota(250 * lStrain);
		} else {
			mPartSys->getEmitter(0)->setTimeToLive(0, 0);
			mPartSys->setParticleQuota(0);
		}
	}
	else if (mType == ePPNuclearLarge) {
		if (getPowerInMW() > 0) {
			float lStrain = (float)mPowerUsed/(float)getPowerInMW();

			mPartSys->getEmitter(0)->setTimeToLive((3.0 * lStrain) + 1, (4.0 * lStrain) + 1);
			mPartSys->setParticleQuota(250 * lStrain);
			mPartSys2->getEmitter(0)->setTimeToLive((3.0 * lStrain) + 1, (4.0 * lStrain) + 1);
			mPartSys2->setParticleQuota(250 * lStrain);
		} else {
			mPartSys->getEmitter(0)->setTimeToLive(0, 0);
			mPartSys->setParticleQuota(0);
			mPartSys2->getEmitter(0)->setTimeToLive(0, 0);
			mPartSys2->setParticleQuota(0);
		}
	}
	else if (mType == ePPCoalSmall) {
		if (getPowerInMW() > 0) {
			float lStrain = (float)mPowerUsed/(float)getPowerInMW();

			mPartSys->getEmitter(0)->setTimeToLive((3.0 * lStrain) + 3, (4.0 * lStrain) + 5);
			mPartSys->setParticleQuota(400 +(150 * lStrain));
			mPartSys2->getEmitter(0)->setTimeToLive((3.0 * lStrain) + 3, (4.0 * lStrain) + 5);
			mPartSys2->setParticleQuota(400 + (150 * lStrain));
		} else {
			mPartSys->getEmitter(0)->setTimeToLive(0, 0);
			mPartSys->setParticleQuota(0);
			mPartSys2->getEmitter(0)->setTimeToLive(0, 0);
			mPartSys2->setParticleQuota(0);
		}
	}
	else if (mType == ePPCoalLarge) {
		if (getPowerInMW() > 0) {
			float lStrain = (float)mPowerUsed/(float)getPowerInMW();

			mPartSys->getEmitter(0)->setTimeToLive((3.0 * lStrain) + 3, (4.0 * lStrain) + 5);
			mPartSys->setParticleQuota(400 +(150 * lStrain));
			mPartSys2->getEmitter(0)->setTimeToLive((3.0 * lStrain) + 3, (4.0 * lStrain) + 5);
			mPartSys2->setParticleQuota(400 + (150 * lStrain));
			mPartSys3->getEmitter(0)->setTimeToLive((3.0 * lStrain) + 3, (4.0 * lStrain) + 5);
			mPartSys3->setParticleQuota(400 + (150 * lStrain));
		} else {
			mPartSys->getEmitter(0)->setTimeToLive(0, 0);
			mPartSys->setParticleQuota(0);
			mPartSys2->getEmitter(0)->setTimeToLive(0, 0);
			mPartSys2->setParticleQuota(0);
			mPartSys3->getEmitter(0)->setTimeToLive(0, 0);
			mPartSys3->setParticleQuota(0);
		}
	}
	else if (mType == ePPGasSmall) {
		if(getPowerInMW() > 0) {
			float lStrain = (float)mPowerUsed/(float)getPowerInMW();

			mPartSys->getEmitter(0)->setTimeToLive((3.0 * lStrain) + 3, (4.0 * lStrain) + 5);
			mPartSys->setParticleQuota(400 +(150 * lStrain));
		} else {
			mPartSys->getEmitter(0)->setTimeToLive(0, 0);
			mPartSys->setParticleQuota(0);
		}
	}
	else if(mType == ePPGasLarge) {
		if(getPowerInMW() > 0) {
			float lStrain = (float)mPowerUsed/(float)getPowerInMW();

			mPartSys->getEmitter(0)->setTimeToLive((3.0 * lStrain) + 3, (4.0 * lStrain) + 5);
			mPartSys->setParticleQuota(400 +(150 * lStrain));
			mPartSys2->getEmitter(0)->setTimeToLive((3.0 * lStrain) + 3, (4.0 * lStrain) + 5);
			mPartSys2->setParticleQuota(400 + (150 * lStrain));
		} else {
			mPartSys->getEmitter(0)->setTimeToLive(0, 0);
			mPartSys->setParticleQuota(0);
			mPartSys2->getEmitter(0)->setTimeToLive(0, 0);
			mPartSys2->setParticleQuota(0);
		}
	}
	else if(mType == ePPBio) {
		if(getPowerInMW() > 0) {
			float lStrain = (float)mPowerUsed/(float)getPowerInMW();

			mPartSys->getEmitter(0)->setTimeToLive((3.0 * lStrain) + 3, (4.0 * lStrain) + 5);
			mPartSys->setParticleQuota(400 +(150 * lStrain));
		} else {
			mPartSys->getEmitter(0)->setTimeToLive(0, 0);
			mPartSys->setParticleQuota(0);
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::dayPassed(EventData* pData)
{
	if(mRepairDays != 0) {
		mRepairDays--;
		if(mRepairDays == 0)
			repairNow();
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::weekPassed(EventData* pData)
{
	if ((!mDestroyed) && (mPowerUsed > 0)) {
		float lEfficiency = 1.0;

		if ((mType == ePPNuclearLarge) || (mType == ePPNuclearSmall)) {
			if (mCompany->getResearchSet().mResearched[14])
				lEfficiency = GameConfig::getDouble("UraniumEfficiencyFactor");

			double lMWUranium = GameConfig::getDouble("MWPerUraniumUnit");

			float lAmountCheck = -((float)mPowerUsed / lMWUranium * lEfficiency);
			int lAmount = 0;

			if((lAmountCheck < -0.1) && (lAmountCheck > -1.0))
				lAmount = -1;
			else if(lAmountCheck <= -1.0)
				lAmount = lAmountCheck;
			else
				lAmount = 0;

			EventHandler::raiseEvent(eChangeResource, new EventArg<int>((int)eUranium, lAmount));
		}
		else if ((mType == ePPCoalLarge) || (mType == ePPCoalSmall)) {
			if(mCompany->getResearchSet().mResearched[5])
				lEfficiency = GameConfig::getDouble("CoalEfficiencyFactor");

			double lMWCoal = GameConfig::getDouble("MWPerCoalUnit");

			float lAmountCheck = -((float)mPowerUsed / lMWCoal * lEfficiency);

			if((lAmountCheck > 0.1) && (lAmountCheck < 1.0))
				lAmountCheck = 1.0;

			int lAmount = lAmountCheck;

			EventHandler::raiseEvent(eChangeResource, new EventArg<int>((int)eCoal, lAmount));
		}
		else if ((mType == ePPGasLarge) || (mType == ePPGasSmall)) {
			if(mCompany->getResearchSet().mResearched[7])
				lEfficiency = GameConfig::getDouble("GasEfficiencyFactor");

			double lMWGas = GameConfig::getDouble("MWPerGasUnit");

			float lAmountCheck = -((float)mPowerUsed / lMWGas * lEfficiency);

			if((lAmountCheck > 0.1) && (lAmountCheck < 1.0))
				lAmountCheck = 1.0;

			int lAmount = lAmountCheck;

			EventHandler::raiseEvent(eChangeResource, new EventArg<int>((int)eGas, lAmount));
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

void Powerplant::resourceGone(EventData* pData)
{
	if (!mDestroyed) {
		if ((mType == ePPNuclearLarge) || (mType == ePPNuclearSmall)) {
			if (static_cast<EventArg<int>*>(pData)->mData1 == (int)eUranium) {
				mPowerAvailable = 0;
			}
		}
		else if ((mType == ePPCoalLarge) || (mType == ePPCoalSmall)) {
			if(static_cast<EventArg<int>*>(pData)->mData1 == (int)eCoal) {
				mPowerAvailable = 0;
			}
		}
		else if ((mType == ePPGasLarge) || (mType == ePPGasSmall)) {
			if(static_cast<EventArg<int>*>(pData)->mData1 == (int)eGas) {
				mPowerAvailable = 0;
			}
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::reset(void)
{
	mPowerUsed = 0;
	mReceivers.clear();
	mTransSet->clear();
	updateParticleSystems();
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::resourceGain(EventData* pData)
{
	if(!mDestroyed) {
		if((mType == ePPNuclearLarge) || (mType == ePPNuclearSmall)) {
			if(static_cast<EventArg<int>*>(pData)->mData1 == (int)eUranium) {
				setDefaultPowerAvailable();
			}
		}
		else if((mType == ePPCoalLarge) || (mType == ePPCoalSmall)) {
			if(static_cast<EventArg<int>*>(pData)->mData1 == (int)eCoal) {
				setDefaultPowerAvailable();
			}
		}
		else if((mType == ePPGasLarge) || (mType == ePPGasSmall)) {
			if(static_cast<EventArg<int>*>(pData)->mData1 == (int)eGas) {
				setDefaultPowerAvailable();
			}
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::setDamaged(void)
{
	mDamaged = true;
	mPowerAvailable = 0;
}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::repairNow(void)
{
	mDamaged = false;
	mCondition = 100;
	setDefaultPowerAvailable();

	TickerMessage lMessage;
	lMessage.mMessage = StrLoc::get()->RepairsComplete1() + mName +
    StrLoc::get()->RepairsComplete2();
	lMessage.mPointOfInterest = mPosition;
	lMessage.mUrgent = false;
	lMessage.mID = 0;

	EventHandler::raiseEvent(eTickerMessage, new EventArg< TickerMessage >(lMessage));

}

/*-----------------------------------------------------------------------------------------------*/

void Powerplant::repair(size_t pMonths)
{
	mRepairDays = pMonths*30; // close enough ;)
}

/*-----------------------------------------------------------------------------------------------*/

bool Powerplant::getDamaged(void)
{
	return mDamaged;
}