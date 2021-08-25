#include "StdAfx.h"
#include "PowerLine.h"
#include "Pole.h"
#include "PowerNet.h"
#include "Map.h"
#include "Terrain.h"

/*-----------------------------------------------------------------------------------------------*/

PowerLine::PowerLine(boost::shared_ptr< Collidable > pFrom, boost::shared_ptr< Collidable > pTo,
					 boost::shared_ptr< Terrain > pTerrain, boost::shared_ptr<Map > pMap, bool pSuggestion,
					 Ogre::Vector2 pSuggestionPosition, bool pInvisible)
: mFrom(pFrom),
  mTo(pTo),
  mSuggestion(pSuggestion),
  mTerrain(pTerrain),
	mSuggestionPosition(pSuggestionPosition),
  mInvisible(pInvisible),
  mMap(pMap),
  mTreesRemoved(false)
{
}

/*-----------------------------------------------------------------------------------------------*/

PowerLine::~PowerLine(void)
{
	if (mNode->isInSceneGraph())
		removeFromSceneManager();
}

/*-----------------------------------------------------------------------------------------------*/

void PowerLine::addToSceneManager(Ogre::SceneManager* pSceneManager)
{
	mManualObject =  pSceneManager->createManualObject("line" + getID()); 

	Ogre::ColourValue lColour = Ogre::ColourValue(0.1,0.1,0.1,0.05);

	if(mSuggestion)
		lColour = Ogre::ColourValue(0.0,0.6,0.0,0.5);

	mNode = pSceneManager->getRootSceneNode()
		->createChildSceneNode("line_node" + getID()); 

	mManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST); 

	mManualObject->position(mFrom->getPosition().x, 25, mFrom->getPosition().y);
	mManualObject->colour(lColour);

	if (mSuggestion) {
		mManualObject->position(mSuggestionPosition.x, 25, mSuggestionPosition.y); 
		mManualObject->colour(lColour);
	} else {
		mManualObject->position(mTo->getPosition().x, 25, mTo->getPosition().y); 
		mManualObject->colour(lColour);
	}

	mManualObject->end();
	mManualObject->setRenderQueueGroup(99);

	mNode->attachObject(mManualObject);
}

/*-----------------------------------------------------------------------------------------------*/

void PowerLine::changeNoBuildColour(void)
{
	mManualObject->clear();

	mManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST); 

	Ogre::ColourValue lColour = Ogre::ColourValue(0.6,0.0,0.0,0.5);

	mManualObject->position(mFrom->getPosition().x, 25, mFrom->getPosition().y);
	mManualObject->colour(lColour);

	if(mSuggestion) {
		mManualObject->position(mSuggestionPosition.x, 25, mSuggestionPosition.y); 
		mManualObject->colour(lColour);
	} else {
		mManualObject->position(mTo->getPosition().x, 25, mTo->getPosition().y); 
		mManualObject->colour(lColour);
	}

	mManualObject->end();
	mManualObject->setRenderQueueGroup(99);
}

/*-----------------------------------------------------------------------------------------------*/

void PowerLine::resetSuggestionPosition(Ogre::Vector2 pPosition)
{
	if ((pPosition.x != -1.0) && (pPosition.y != -1.0))
		mSuggestionPosition = pPosition;

	mManualObject->clear();

	Ogre::ColourValue lColour = Ogre::ColourValue(0.1,0.1,0.1,1.0);

	if (mSuggestion)
		lColour = Ogre::ColourValue(0.0,0.6,0.0,0.5);

	mManualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST); 

	mManualObject->position(mFrom->getPosition().x, 25, mFrom->getPosition().y);
	mManualObject->colour(lColour);

	if(mSuggestion) {
		mManualObject->position(mSuggestionPosition.x, 25, mSuggestionPosition.y); 
		mManualObject->colour(lColour);
	} else {
		mManualObject->position(mTo->getPosition().x, 25, mTo->getPosition().y); 
		mManualObject->colour(lColour);
	}

	mManualObject->end(); 
	mManualObject->setRenderQueueGroup(99);
}

/*-----------------------------------------------------------------------------------------------*/

void PowerLine::addHangingLineToManual(Ogre::Vector3 pFrom, Ogre::Vector3 pTo,
                                       Ogre::ManualObject* pObject)
{
	Ogre::Vector3 lDirection = pTo - pFrom;

	float lHangingDepth = 150.0 / lDirection.length();

	Ogre::Vector3 lPos = pFrom;
	mManualObject->position(lPos);

	float lSubdivisions = 10.0f;

	for (int i = 1; i < (int)lSubdivisions; i++) {
		float lWay = (float)i / lSubdivisions;
		lPos = pFrom + (lDirection * lWay);
		lPos.y += ((((float)i - (lSubdivisions / 2.0))
			* ((float)i - (lSubdivisions / 2.0))) / lHangingDepth) 
				- ((lSubdivisions / 2.0)*(lSubdivisions / 2.0)/lHangingDepth);
		mManualObject->position(lPos);
		mManualObject->position(lPos);
	}

	lPos = pFrom + (lDirection * 1);
	mManualObject->position(lPos);
}

/*-----------------------------------------------------------------------------------------------*/

void PowerLine::adjust(boost::shared_ptr< PowerNetVertex > pFrom,
                       boost::shared_ptr< PowerNetVertex > pTo, bool pFromIntersection)
{
	Ogre::Vector2 pFromPos = pFrom->getPosition();
	Ogre::Vector2 pToPos = pTo->getPosition();
	Ogre::Vector2 lNull = Ogre::Vector2(0.0,0.0);

	float lHeightFrom = mTerrain->getHeightAt(pFromPos.x,pFromPos.y);
	float lHeightTo = mTerrain->getHeightAt(pToPos.x,pToPos.y);

	if ((lHeightTo < 0.12) || (lHeightFrom < 0.12)) {
		Ogre::Vector2 lPerp = (pFromPos - pToPos).perpendicular().normalisedCopy()*0.5;

		mManualObject->clear();

		mManualObject->begin("kabel_manual", Ogre::RenderOperation::OT_TRIANGLE_LIST);

		mManualObject->position(pToPos.x - lPerp.x, lHeightTo + 1, pToPos.y - lPerp.y);
		mManualObject->textureCoord(1,1);
		mManualObject->position(pToPos.x + lPerp.x, lHeightTo + 1, pToPos.y + lPerp.y);
		mManualObject->textureCoord(0,1);
		mManualObject->position(pFromPos.x + lPerp.x, lHeightFrom + 1, pFromPos.y + lPerp.y);
		mManualObject->textureCoord(0,0);

		mManualObject->position(pFromPos.x - lPerp.x, lHeightFrom + 1, pFromPos.y - lPerp.y);
		mManualObject->textureCoord(1,0);
		mManualObject->position(pToPos.x - lPerp.x, lHeightTo + 1, pToPos.y - lPerp.y);
		mManualObject->textureCoord(1,1);
		mManualObject->position(pFromPos.x + lPerp.x, lHeightFrom + 1, pFromPos.y + lPerp.y);
		mManualObject->textureCoord(0,0);

		mManualObject->end();
		mManualObject->setRenderQueueGroup(0);
	} else {
		bool lIntersecting =  false;

		if ((pFrom->getLineAttachingPoints(pToPos, true).size() == 4) 
			&& ((pTo->getLineAttachingPoints(pFromPos, true).size() == 4))) {
			Ogre::Vector2 lFrom1 = Ogre::Vector2(pFrom->getLineAttachingPoints(pToPos, true)[0].x,
				pFrom->getLineAttachingPoints(pToPos, true)[0].z);
			Ogre::Vector2 lTo1 = Ogre::Vector2(pTo->getLineAttachingPoints(pFromPos, true)[0].x,
				pTo->getLineAttachingPoints(pFromPos, true)[0].z);
			Ogre::Vector2 lFrom2 = Ogre::Vector2(pFrom->getLineAttachingPoints(pToPos, true)[1].x,
				pFrom->getLineAttachingPoints(pToPos, true)[1].z);
			Ogre::Vector2 lTo2 = Ogre::Vector2(pTo->getLineAttachingPoints(pFromPos, true)[1].x,
				pTo->getLineAttachingPoints(pFromPos, true)[1].z);

			if(intersectLines(lFrom1, lTo1, lFrom2, lTo2))
				lIntersecting = true;

			mManualObject->clear();
			mManualObject->begin("powerline", Ogre::RenderOperation::OT_LINE_LIST);

			if (lIntersecting) {
				addHangingLineToManual(pFrom->getLineAttachingPoints(pToPos)[0], 
										pTo->getLineAttachingPoints(pFromPos)[2],
										mManualObject);

				addHangingLineToManual(pFrom->getLineAttachingPoints(pToPos)[1],
										pTo->getLineAttachingPoints(pFromPos)[3],
										mManualObject);

				addHangingLineToManual(pFrom->getLineAttachingPoints(pToPos)[2],
										pTo->getLineAttachingPoints(pFromPos)[0],
										mManualObject);

				addHangingLineToManual(pFrom->getLineAttachingPoints(pToPos)[3], 
										pTo->getLineAttachingPoints(pFromPos)[1],
										mManualObject);
			} else {
				addHangingLineToManual(pFrom->getLineAttachingPoints(pToPos)[0], 
										pTo->getLineAttachingPoints(pFromPos)[0],
										mManualObject);

				addHangingLineToManual(pFrom->getLineAttachingPoints(pToPos)[1],
										pTo->getLineAttachingPoints(pFromPos)[1],
										mManualObject);

				addHangingLineToManual(pFrom->getLineAttachingPoints(pToPos)[2],
										pTo->getLineAttachingPoints(pFromPos)[2],
										mManualObject);

				addHangingLineToManual(pFrom->getLineAttachingPoints(pToPos)[3], 
										pTo->getLineAttachingPoints(pFromPos)[3],
										mManualObject);
			}

			mManualObject->end();
			mManualObject->setRenderQueueGroup(99);
		} else {
			if (intersectLines(Ogre::Vector2(pFrom->getLineAttachingPoints(pToPos, true)[0].x,
				  pFrom->getLineAttachingPoints(pToPos, true)[0].z),
				  Ogre::Vector2(pTo->getLineAttachingPoints(pFromPos, true)[0].x,
				  pTo->getLineAttachingPoints(pFromPos, true)[0].z),
				  Ogre::Vector2(pFrom->getLineAttachingPoints(pToPos, true)[1].x,
				  pFrom->getLineAttachingPoints(pToPos, true)[1].z),
				  Ogre::Vector2(pTo->getLineAttachingPoints(pFromPos, true)[1].x,
				  pTo->getLineAttachingPoints(pFromPos, true)[1].z))) {
				lIntersecting = true;
			}

			mManualObject->clear();
			mManualObject->begin("powerline", Ogre::RenderOperation::OT_LINE_LIST);

			if (lIntersecting) {
				addHangingLineToManual(pFrom->getLineAttachingPoints(pToPos)[0], 
					pTo->getLineAttachingPoints(pFromPos)[1], mManualObject);

				addHangingLineToManual(pFrom->getLineAttachingPoints(pToPos)[1],
					pTo->getLineAttachingPoints(pFromPos)[0], mManualObject);
			} else {
				addHangingLineToManual(pFrom->getLineAttachingPoints(pToPos)[0], 
					pTo->getLineAttachingPoints(pFromPos)[0], mManualObject);

				addHangingLineToManual(pFrom->getLineAttachingPoints(pToPos)[1],
					pTo->getLineAttachingPoints(pFromPos)[1], mManualObject);
			}

			mManualObject->end();
			mManualObject->setRenderQueueGroup(99);
		}

		if (!mTreesRemoved) {
			// remove trees
			boost::shared_ptr <LineCollidable > lTreeRemover(new LineCollidable(RotatedRect(
				Ogre::Vector2(pFrom->getLineAttachingPoints(pToPos)[0].x,
        pFrom->getLineAttachingPoints(pToPos)[0].z),
				Ogre::Vector2(pTo->getLineAttachingPoints(pFromPos)[0].x,
        pTo->getLineAttachingPoints(pFromPos)[0].z),
				Ogre::Vector2(pFrom->getLineAttachingPoints(pToPos)[1].x,
        pFrom->getLineAttachingPoints(pToPos)[1].z),
				Ogre::Vector2(pTo->getLineAttachingPoints(pFromPos)[1].x,
        pTo->getLineAttachingPoints(pFromPos)[1].z))));

			mMap->addCollidable(lTreeRemover, true);
			mMap->removeCollidable(lTreeRemover->getID());

			mTreesRemoved = true;
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void PowerLine::removeFromSceneManager(void)
{
	mNode->detachAllObjects();
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 PowerLine::getPosition(void)
{
	return Ogre::Vector2(-1.0,-1.0);
}

/*-----------------------------------------------------------------------------------------------*/

bool PowerLine::isSelectable(void)
{
	return false;
}

/*-----------------------------------------------------------------------------------------------*/

eGameObjectType PowerLine::getType(void)
{
	return eLine;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 PowerLine::getFrom(void)
{
	return mFrom->getPosition();
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 PowerLine::getTo(void)
{
	return mTo->getPosition();
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 PowerLine::getSuggestionPosition(void)
{
	return mSuggestionPosition;
}