#include "StdAfx.h"
#include "City.h"
#include "Terrain.h"
#include "TransformatorSet.h"

/*-----------------------------------------------------------------------------------------------*/

const size_t cCityBorderLength = 30; //!< city length for collision

// Compensation for flawed 3DS exporter
const float cCityHeight[] = { -(3.70122*0.8)+0.01, -(3.70287*0.8)+0.01, (2.82402*0.8)+0.01 };

const std::string cCityFloorMesh[] = { "stadtkleinboden.mesh", "stadtmittelboden.mesh",
                                       "stadtgrossboden.mesh" };
const std::string cCityMaterial[] = { "city_small_build", "city_medium_build",
                                      "city_large_build" };
const std::string cCityMesh[] = { "stadtklein.mesh", "stadtmittel.mesh", "stadtgross.mesh" };

/*-----------------------------------------------------------------------------------------------*/

City::City(Ogre::Vector2 pPosition, std::string pName, std::string pCitySize)
: mPosition(pPosition),
  mName(pName),
  mSize(pCitySize),
  mEntity(0),
	mNode(0),
  mNamedAfterCounter(0),
  mCurrentSupply(0),
  mCustomers(0),
	mAdvertising(0),
  mInheritSettings(true),
  mPrice(0)
{
	srand(pPosition.x);

	if (getSize() == 0)
		mResidentCount = 50000 + ((rand()*1000) % 249999);
	else if (getSize() == 1)
		mResidentCount = 300000 + ((rand()*1000) % 699999);
	else
		mResidentCount = 1000000 + ((rand()*1000) % 2500000);

	mConsumption = 0;

	REGISTER_CALLBACK(eWeekPassed, City::weekPassed);
}

/*-----------------------------------------------------------------------------------------------*/

size_t City::getResidentCount(void)
{
	return mResidentCount;
}

/*-----------------------------------------------------------------------------------------------*/

void City::weekPassed(EventData *pData)
{
	double pCustomerConsumption = GameConfig::getDouble("AverageCustomerDemand");

	mConsumption = ((float)mCustomers * pCustomerConsumption) / 1000;
	
	if ((mCustomers > 0) && (mConsumption == 0))
		mConsumption = 1;
}

/*-----------------------------------------------------------------------------------------------*/

unsigned int City::getConsumption(void)
{
	return mConsumption;
}

/*-----------------------------------------------------------------------------------------------*/

void City::addPowerSupplied(int pAmount)
{
	mCurrentSupply += pAmount;
}

/*-----------------------------------------------------------------------------------------------*/

void City::reset(void)
{
	mCurrentSupply = 0;
	mSuppliers.clear();
	mTransSet->clear();
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<Ogre::Vector3> City::getLineAttachingPoints(Ogre::Vector2 pAdjacentVertexPosition,
                                                        bool pCheckOnly)
{
	std::vector<Ogre::Vector3> lAttachingPoints;

	if (pCheckOnly)
		for (int i = 0; i < 4; ++i)
			lAttachingPoints.push_back(mNode->getPosition());
	else
		lAttachingPoints = mTransSet->use(pAdjacentVertexPosition);

	return lAttachingPoints;
}

/*-----------------------------------------------------------------------------------------------*/

void City::addSupplier(std::string pSupplier)
{
	bool lFound = false;

	for (size_t i = 0; i < mSuppliers.size(); i++) {
		if (pSupplier.substr(0, pSupplier.find(":")) 
      == mSuppliers[i].substr(0, mSuppliers[i].find(":"))) {
			mSuppliers[i] = mSuppliers[i].substr(0, mSuppliers[i].find(":") + 2)
				+ toString(toNumber<int>(pSupplier.substr(pSupplier.find(":")+1))
				+ toNumber<int>(mSuppliers[i].substr(mSuppliers[i].find(":")+1))) + "MW";

			lFound = true;
		}
	}

	if (!lFound)
		mSuppliers.push_back(pSupplier);
}

/*-----------------------------------------------------------------------------------------------*/

std::string City::getSuppliers(void)
{
	if (mSuppliers.size() > 0) {
		std::string lReturn;

		for (size_t i = 0; i < mSuppliers.size(); i++) {
			if (i < mSuppliers.size() - 1)
				lReturn = lReturn + mSuppliers[i] + "\n";
			else
				lReturn = lReturn + mSuppliers[i];

			if (i == 4) {
				if(mSuppliers.size() > 5)
					return lReturn + StrLoc::get()->CitySeeNet();
				else
					return lReturn + "\n" + StrLoc::get()->CitySeeNet();
			}
		}
		return lReturn;
	} else {
		return StrLoc::get()->CityNone();
	}
}

/*-----------------------------------------------------------------------------------------------*/

size_t City::getPowerNeeded(void)
{
	return (mConsumption - mCurrentSupply);
}

/*-----------------------------------------------------------------------------------------------*/

bool City::isAutoremove(void)
{
	return false;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<RotatedRect> City::getGroundRects()
{
	std::vector<RotatedRect> lTemp;
	lTemp.push_back(RotatedRect(rectFromPoint2(Ogre::Vector2(mPosition.x, mPosition.y),
		cCityBorderLength)));
	return lTemp;
}

/*-----------------------------------------------------------------------------------------------*/

int City::getSize()
{
	if (mSize == "small")
		return 0;
	else if (mSize == "medium")
		return 1;
	else if (mSize == "large")
		return 2;
	else
		return 0;
}

/*-----------------------------------------------------------------------------------------------*/

void City::addToSceneManager(Ogre::SceneManager* pSceneManager,
                             boost::shared_ptr<Terrain> pTerrain)
{
	mEntity = pSceneManager->createEntity("city" + getID(), cCityMesh[getSize()]);
	mEntity->setCastShadows(false);

	mEntityFloor = pSceneManager->createEntity("city_boden" + getID(), cCityFloorMesh[getSize()]);
	mEntityFloor->setCastShadows(false);

	mNode = pSceneManager->getRootSceneNode()->createChildSceneNode("city_node" + getID());

	float lHeight = pTerrain->getHighestFromQuad(mPosition.x, mPosition.y, cCityBorderLength);

	pTerrain->flattenQuad(mPosition, cCityBorderLength, lHeight);

    mNode->attachObject(mEntity);
	mNode->attachObject(mEntityFloor);

	Ogre::Entity* mStreet = pSceneManager->createEntity("city_street" + getID(), "street.mesh");
	Ogre::SceneNode* mStreetNode = pSceneManager->getRootSceneNode()->
		createChildSceneNode("city_street_node" + getID());

	mStreetNode->attachObject(mStreet);
	mStreetNode->translate(Ogre::Vector3( mPosition.x, lHeight + 0.3, mPosition.y));
	mStreetNode->scale(Ogre::Vector3(0.72, 0.72, 0.72));

	mNode->scale(Ogre::Vector3(0.8, 0.8, 0.8));
	mNode->translate( Ogre::Vector3( mPosition.x, lHeight + cCityHeight[getSize()] 
			+ cFloorDistance, mPosition.y));
	mTransSet.reset(new TransformatorSet(pSceneManager, mNode->getPosition()
			+ Ogre::Vector3(0.0, -cCityHeight[getSize()] -0.5, 0.0), eCityTrans));
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 City::getPosition(void)
{
	return mPosition;
}

/*-----------------------------------------------------------------------------------------------*/

bool City::isSelectable(void)
{
	return true;
}

/*-----------------------------------------------------------------------------------------------*/

eGameObjectType City::getType(void)
{
	return eCity;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::UTFString City::getName(void)
{
	return mName;
}

/*-----------------------------------------------------------------------------------------------*/

void City::highlight(bool pHighlight)
{
	mEntityFloor->setVisible(!pHighlight);

	if (pHighlight)
		mEntity->setMaterialName("highlight");
	else
		mEntity->setMaterialName(cCityMaterial[getSize()]);
}
