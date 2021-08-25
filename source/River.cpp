#include "StdAfx.h"
#include "River.h"
#include "Bezier.h"
#include "Terrain.h"

/*-----------------------------------------------------------------------------------------------*/

River::River(std::vector< Ogre::Vector2 > pPoints, bool pOceanEnd, bool pSourceStart)
: mPoints(pPoints),
  mOceanEnd(pOceanEnd),
  mSourceStart(pSourceStart)
{
}

/*-----------------------------------------------------------------------------------------------*/

void River::generateAndAddToSceneManager(Ogre::SceneManager* pSceneManager,
										 boost::shared_ptr<Terrain> pTerrain)
{
	std::stringstream lTemp;
	std::string lName;
	lTemp << mPoints[0] << mPoints[1];
	lName = lTemp.str() + "river";

	// create bezier curve
	BezierCurve2D lCurve(mPoints, 1.5, 10);

	std::vector<Ogre::Vector2> lBezierPoints = lCurve.getPoints();

	Ogre::ManualObject* manual = pSceneManager->createManualObject(lName);
	manual->begin("rivertex", Ogre::RenderOperation::OT_TRIANGLE_LIST);

	float lX,lY;
	int j;
	int lSubdivLength = lBezierPoints.size();

	std::vector<float> lLengthAtIndex;
	float lCompleteLength = 0.0;

	lLengthAtIndex.push_back(0.0);

	for (int l = 1; l < lSubdivLength; l ++) {
		lCompleteLength += (lBezierPoints[l-1]-lBezierPoints[l]).length();
		lLengthAtIndex.push_back(lCompleteLength/4);
	}

	Ogre::Vector2 lTangentNormNext;
	Ogre::Vector2 lTangentNorm;
	Ogre::Vector2 lTangentVertNormNext;
	Ogre::Vector2 lTangentVertNorm;

	for (int i = 0; i < lSubdivLength - 1; i++) {
		Ogre::Vector2 lCol[4];

		lTangentNorm = (lBezierPoints[i+1] - lBezierPoints[i]);
		lTangentNorm.normalise();

		if(i < lSubdivLength - 2)
			lTangentNormNext = (lBezierPoints[i+2] - lBezierPoints[i+1]);
		else
			lTangentNormNext = lTangentNorm;

		lTangentNormNext.normalise();

		lTangentVertNorm = lTangentNorm.perpendicular();
		lTangentVertNorm.normalise();

		lTangentVertNormNext = lTangentNormNext.perpendicular();
		lTangentVertNormNext.normalise();

		if((lTangentVertNormNext.x == 0) && (lTangentVertNormNext.y == 0))
			lTangentVertNormNext = lTangentVertNorm;

		if (i < 10) {
			lTangentVertNorm = lTangentVertNorm * (i/2.5);
			lTangentVertNormNext = lTangentVertNormNext * ((i+1)/2.5);
		} else {
			lTangentVertNorm = lTangentVertNorm * 4;
			lTangentVertNormNext = lTangentVertNormNext * 4;
		}

		j = i*10;

		lX = lBezierPoints[i].x + lTangentVertNorm.x;
		lY = lBezierPoints[i].y + lTangentVertNorm.y;

		lCol[0].x = lX;
		lCol[0].y = lY;

		manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,20)+2, lY);
		manual->textureCoord(1, lLengthAtIndex[i]);
		
		lX = lBezierPoints[i+1].x + lTangentVertNormNext.x;
		lY = lBezierPoints[i+1].y + lTangentVertNormNext.y;

		lCol[1].x = lX;
		lCol[1].y = lY;

		manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,20)+2, lY);
		manual->textureCoord(1,lLengthAtIndex[i+1]);

		lX = lBezierPoints[i].x - lTangentVertNorm.x;
		lY = lBezierPoints[i].y - lTangentVertNorm.y;

		lCol[2].x = lX;
		lCol[2].y = lY;

		manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,20)+2, lY);
		manual->textureCoord(0,lLengthAtIndex[i]);

		// next tri

		lX = lBezierPoints[i].x - lTangentVertNorm.x;
		lY = lBezierPoints[i].y - lTangentVertNorm.y;

		manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,20)+2, lY);
		manual->textureCoord(0,lLengthAtIndex[i]);

		lX = lBezierPoints[i+1].x + lTangentVertNormNext.x;
		lY = lBezierPoints[i+1].y + lTangentVertNormNext.y;

		manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,20)+2, lY);
		manual->textureCoord(1,lLengthAtIndex[i+1]);

		lX = lBezierPoints[i+1].x - lTangentVertNormNext.x;
		lY = lBezierPoints[i+1].y - lTangentVertNormNext.y;

		lCol[3].x = lX;
		lCol[3].y = lY;

		manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,20)+2, lY);
		manual->textureCoord(0,lLengthAtIndex[i+1]);

		mCollidableRects.push_back(alignPoints(lCol[3],lCol[1],lCol[2],lCol[0]));
	}

	manual->end();
	pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

	if (mOceanEnd) {
		Ogre::ManualObject* manual = pSceneManager->createManualObject("riverend" + getID());
		manual->begin("riverendtex", Ogre::RenderOperation::OT_TRIANGLE_LIST);

		lX = lBezierPoints[lBezierPoints.size()-1].x 
			- lTangentNormNext.x*0.01 + lTangentVertNormNext.x;
		lY = lBezierPoints[lBezierPoints.size()-1].y 
			- lTangentNormNext.y*0.01 + lTangentVertNormNext.y;

		manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,20)+2, lY);
		manual->textureCoord(1, 0);

		lX = lBezierPoints[lBezierPoints.size()-1].x 
			+ lTangentNormNext.x*15 + lTangentVertNormNext.x;
		lY = lBezierPoints[lBezierPoints.size()-1].y 
			+ lTangentNormNext.y*15 + lTangentVertNormNext.y;

		manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,20)+2, lY);
		manual->textureCoord(1, 1);

		lX = lBezierPoints[lBezierPoints.size()-1].x 
			- lTangentNormNext.x*0.01 -lTangentVertNormNext.x;
		lY = lBezierPoints[lBezierPoints.size()-1].y 
			- lTangentNormNext.y*0.01 - lTangentVertNormNext.y;

		manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,20)+2, lY);
		manual->textureCoord(0, 0);

		// next tri

		lX = lBezierPoints[lBezierPoints.size()-1].x 
			- lTangentNormNext.x*0.01 - lTangentVertNormNext.x;
		lY = lBezierPoints[lBezierPoints.size()-1].y 
			- lTangentNormNext.y*0.01 - lTangentVertNormNext.y;

		manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,20)+2, lY);
		manual->textureCoord(0, 0);


		lX = lBezierPoints[lBezierPoints.size()-1].x 
			+ lTangentNormNext.x*15 + lTangentVertNormNext.x;
		lY = lBezierPoints[lBezierPoints.size()-1].y 
			+ lTangentNormNext.y*15 + lTangentVertNormNext.y;

		manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,20)+2, lY);
		manual->textureCoord(1, 1);


		lX = lBezierPoints[lBezierPoints.size()-1].x 
			+ lTangentNormNext.x*15 - lTangentVertNormNext.x;
		lY = lBezierPoints[lBezierPoints.size()-1].y 
			+ lTangentNormNext.y*15 - lTangentVertNormNext.y;

		manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,20)+2, lY);
		manual->textureCoord(0, 1);

		manual->end();

		manual->setCastShadows(false);

		pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(manual);
	}

	// Fill in lines list
	for (size_t k = 1; k < lBezierPoints.size(); k++) {
		mLineList.push_back(lBezierPoints[k-1]);
		mLineList.push_back(lBezierPoints[k]);
	}
}

/*-----------------------------------------------------------------------------------------------*/

RotatedRect River::alignPoints(Ogre::Vector2 p1, Ogre::Vector2 p2, Ogre::Vector2 p3,
                               Ogre::Vector2 p4)
{
	return RotatedRect(p1, p2, p3, p4);
}
/*-----------------------------------------------------------------------------------------------*/

bool River::isAutoremove(void)
{
	return false;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector< RotatedRect > River::getGroundRects()
{
	return mCollidableRects;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 River::getPosition(void)
{
	return Ogre::Vector2(-1.0,-1.0);
}

/*-----------------------------------------------------------------------------------------------*/

bool River::isSelectable(void)
{
	return false;
}

/*-----------------------------------------------------------------------------------------------*/

eGameObjectType River::getType(void)
{
	return eRiver;
}