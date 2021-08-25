#include "StdAfx.h"
#include "Terrain.h"

/*-----------------------------------------------------------------------------------------------*/

Terrain::Terrain(std::string pMapPrefix, Ogre::SceneManager* pSceneManager)
: mMapPrefix(pMapPrefix)
{
	evaluateParams();

	std::string lPrefix = cDataDirPre + "maps/" + pMapPrefix + "_";

	Ogre::Image lHeightmapImage;
	lHeightmapImage.load(lPrefix + "heightmap.png",
		Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME); 

	Ogre::MaterialPtr lMaterial = Ogre::MaterialManager::getSingleton().getByName(pMapPrefix + "_material");

	lMaterial->setReceiveShadows(true);

	if(lHeightmapImage.getData() != 0) {
		ET::RealArray2D lHeights = ET::loadRealArrayFromImage(lHeightmapImage);

		mFlatTerrain.reset(new ET::FlatTerrain(lHeights, Ogre::Vector2(mScaleX, mScaleZ),
			cTerrainScaleY));

		mHandle.reset(new ET::Page("TerrainPage", mFlatTerrain.get(), 129, 129,
			true, 6, 0.008f));

		mHandle->setCastShadows(false);

		mHandle->setMaterial(lMaterial);

		pSceneManager->getRootSceneNode()->attachObject(mHandle.get());


		Ogre::ManualObject* lWater = pSceneManager->createManualObject("water");

		float lWaterHeight = 5;
		
		lWater->begin(pMapPrefix + "_water", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		lWater->position(mWidth, lWaterHeight, mHeight);
		lWater->textureCoord(1, 1);
		lWater->position(mWidth, lWaterHeight, 0);
		lWater->textureCoord(1, 0);
		lWater->position(0, lWaterHeight, 0);
		lWater->textureCoord(0, 0);
		lWater->position(0, lWaterHeight, mHeight);
		lWater->textureCoord(0, 1);
		lWater->position(mWidth, lWaterHeight, mHeight);
		lWater->textureCoord(1, 1);
		lWater->position(0, lWaterHeight, 0);
		lWater->textureCoord(0, 0);
		lWater->end();

		pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(lWater);
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Terrain::flattenQuad(Ogre::Vector2 pCenter, size_t pSideLength, float pHeight)
{		
	int lCenterX = pCenter.x;
	int lCenterZ = pCenter.y;

	int lSideX = pSideLength;
	int lSideZ = pSideLength;

	for (int x = -(lSideX/2); x <= (lSideX/2) + 4; ++x) {
		for (int z = -(lSideZ/2); z <= (lSideZ/2) + 4; ++z) {
			mFlatTerrain->setHeightValue((lCenterX + x) / mScaleX, (lCenterZ + z) / mScaleZ,
										 pHeight / cTerrainScaleY);
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

float Terrain::getHeightAt(float pX, float pZ)
{
	if((pX >= 0) && (pX <= (mWidth-1)) && (pZ >= 0) && (pZ <= (mHeight-1)))
		return mFlatTerrain->getHeightAt(pX, pZ);
	else
		return -1.0;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector3 Terrain::getIntersectionFromRay(const Ogre::Ray& pRay)
{
	std::pair< bool, Ogre::Vector3 > lResult;
	lResult = mFlatTerrain->rayIntersects(pRay);

	if(lResult.first)
		return lResult.second;
	else
		return Ogre::Vector3(-1.0,-1.0,-1.0);
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector3 Terrain::getQuadIntersectionFromRay(const Ogre::Ray& pRay, int pSideLength)
{
	int lExtendedSideLength = pSideLength + 3;
	Ogre::Vector3 lIntersectionResult = getIntersectionFromRay(pRay);

	float lHeightMax = 0.0;

	for (int i = (-lExtendedSideLength/2) - 1; i < (lExtendedSideLength/2); i++) {
		for (int j = (-lExtendedSideLength/2) - 1; j < (lExtendedSideLength/2); j++) {
			float lCurrentHeight = 
				getHeightValueAt((lIntersectionResult.x - i) 
					/ mScaleX, (lIntersectionResult.z - j) / mScaleZ);

			if(lCurrentHeight > lHeightMax)
				lHeightMax = lCurrentHeight;
		}
	}

	lIntersectionResult.y = (lHeightMax * cTerrainScaleY) + 2.0f;

	return lIntersectionResult;
}

/*-----------------------------------------------------------------------------------------------*/

float Terrain::getHighestFromQuad(int pX, int pY, int pSideLength, bool pAverage)
{
	int lExtendedSideLength = pSideLength + 3;
	float lHeightMax = 0.0;
	float lHeightMin = 90000.0;

	for (int i = (-lExtendedSideLength/2) - 1; i < (lExtendedSideLength/2); i++) {
		for (int j = (-lExtendedSideLength/2) - 1; j < (lExtendedSideLength/2); j++) {
			float lCurrentHeight = getHeightValueAt((pX - i) / mScaleX, (pY - j) / mScaleZ);

			if (lCurrentHeight > lHeightMax)
				lHeightMax = lCurrentHeight;

			if (lCurrentHeight < lHeightMin)
				lHeightMin = lCurrentHeight;
		}
	}

	if(pAverage)
		return ((lHeightMax+lHeightMax+lHeightMin)/3) * cTerrainScaleY;
	else
		return lHeightMax * cTerrainScaleY;

}

/*-----------------------------------------------------------------------------------------------*/

void Terrain::evaluateParams(void)
{
	int lHeightmapHeight = 0;
	int lHeightmapWidth = 0;

	boost::shared_ptr<TiXmlDocument> lTerrainConfig;
	TiXmlNode* rootNode;

	lTerrainConfig.reset(new TiXmlDocument((cDataDirPre + "maps/"  + mMapPrefix + 
    "_terrain.xml").c_str()));

	lTerrainConfig->LoadFile(TIXML_ENCODING_UTF8);

	std::vector< std::string > lBorderPoints;

	rootNode = lTerrainConfig->FirstChildElement("terrain_config");
	
	lHeightmapWidth = toNumber<int>(rootNode->ToElement()->Attribute("page_width"));
	lHeightmapHeight = toNumber<int>(rootNode->ToElement()->Attribute("page_height"));
	mWidth = toNumber<int>(rootNode->ToElement()->Attribute("world_x"));
	mHeight = toNumber<int>(rootNode->ToElement()->Attribute("world_z"));

	mScaleX = mWidth / (lHeightmapWidth - 1);
	mScaleZ = mHeight / (lHeightmapHeight - 1);
}

/*-----------------------------------------------------------------------------------------------*/

void Terrain::setHeightValueAt(int pX, int pY, float pHeight)
{
	mFlatTerrain->setHeightValue(pX, pY, pHeight);
}

/*-----------------------------------------------------------------------------------------------*/

float Terrain::getHeightValueAt(int pX, int pY)
{
	if ((pX >= 0) && (pX <= (mWidth/mScaleX)) && (pY >= 0) && (pY <= (mHeight/mScaleZ)))
		return mFlatTerrain->getHeightValue(pX, pY);
	else
		return -1.0f;
}

/*-----------------------------------------------------------------------------------------------*/

void Terrain::update(void)
{
	mFlatTerrain->update();
}