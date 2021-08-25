#include "StdAfx.h"
#include "CompanyBuilding.h"
#include "PowerNet.h"
#include "Terrain.h"


/*-----------------------------------------------------------------------------------------------*/

CompanyBuilding::CompanyBuilding(Ogre::Vector2 pPosition, eCompanyObjectType pType)
: mPosition(pPosition),
  mType(pType)
{
}

/*-----------------------------------------------------------------------------------------------*/

bool CompanyBuilding::isAutoremove(void)
{
	return false;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<RotatedRect> CompanyBuilding::getGroundRects()
{
	std::vector<RotatedRect> lTemp;
	lTemp.push_back(RotatedRect(Ogre::Rect(mPosition.x - 12, mPosition.y - 12, 
									mPosition.x + 12, mPosition.y + 12)));
	return lTemp;
}

/*-----------------------------------------------------------------------------------------------*/

void CompanyBuilding::addToSceneManager(Ogre::SceneManager* pSceneManager,
										boost::shared_ptr<Terrain> pTerrain)
{
	mNode = pSceneManager->getRootSceneNode()->createChildSceneNode("co_node" + getID());

	if (mType == eCOHeadquarters) {
		mEntity = pSceneManager->createEntity("co" + getID(), "co_headquarters.mesh");
		mEntityFloor = pSceneManager->createEntity("co_ground" + getID(),
			"co_headquarters_ground.mesh");
	}
	else if (mType == eCOResearch) {
		mEntity = pSceneManager->createEntity("co" + getID(), "co_research.mesh");
		mEntityFloor = pSceneManager->createEntity("co_ground" + getID(),
			"co_research_ground.mesh");
		mNode->setScale(1.7,1.7,1.7);
	}
	else if (mType == eCOPublicRelations) {
		mEntity = pSceneManager->createEntity("co" + getID(), "co_public_relations.mesh");
		mEntityFloor = pSceneManager->createEntity("co_ground" + getID(),
			"co_public_relations_ground.mesh");
	}

	mEntity->setCastShadows(false);
	mEntityFloor->setCastShadows(false);

	float lHeight = pTerrain->getHighestFromQuad(mPosition.x, mPosition.y,
		cStructureBorderLength[mType]);

	pTerrain->flattenQuad(mPosition, cStructureBorderLength[mType], lHeight);
	pTerrain->update();

	mNode->attachObject(mEntity);
	mNode->attachObject(mEntityFloor);
	mNode->translate(Ogre::Vector3( mPosition.x, pTerrain->
		getHeightAt(mPosition.x,mPosition.y)+cFloorDistance,mPosition.y));
}

/*-----------------------------------------------------------------------------------------------*/

void CompanyBuilding::removeFromSceneManager(void)
{
	mNode->detachAllObjects();
	mNode->getCreator()->destroyEntity(mEntity);
	mNode->getCreator()->destroyEntity(mEntityFloor);
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 CompanyBuilding::getPosition(void)
{
	return mPosition;
}

/*-----------------------------------------------------------------------------------------------*/

bool CompanyBuilding::isSelectable(void)
{
	return true;
}

/*-----------------------------------------------------------------------------------------------*/

eGameObjectType CompanyBuilding::getType(void)
{
	return eCompany;
}

/*-----------------------------------------------------------------------------------------------*/

void CompanyBuilding::highlight(bool pHighlight)
{
	mEntityFloor->setVisible(!pHighlight);

	if (pHighlight) {
		mEntity->setMaterialName("highlight");
	} else {	
		if (mType == eCOHeadquarters)
			mEntity->setMaterialName("co_headquarters");
		else if (mType == eCOResearch)
			mEntity->setMaterialName("co_research");
		else if (mType == eCOPublicRelations)
			mEntity->setMaterialName("co_public_relations");
	}
}
