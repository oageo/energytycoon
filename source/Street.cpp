#include "StdAfx.h"
#include "Street.h"
#include "Bezier.h"
#include "City.h"
#include "Terrain.h"

/*-----------------------------------------------------------------------------------------------*/

Street::Street(int pX1, int pY1, int pX2, int pY2)
: mX1(pX1),
  mY1(pY1),
  mX2(pX2),
  mY2(pY2),
  mCarDriving(false),
  mCarFadingIn(false),
  mCarFadingOut(false)
{
}

/*-----------------------------------------------------------------------------------------------*/

void Street::upDown(std::vector<Ogre::Vector2> &pPoints, int pX1, int pX2, int pY1, int pY2)
{
  int lOutH = 15;
  int lOut = 21;

  pPoints.push_back(Ogre::Vector2(pX1 - 1.1, pY1-lOutH));
  pPoints.push_back(Ogre::Vector2(pX1 - 1.1, pY1-lOut));
  pPoints.push_back(Ogre::Vector2(pX2 + 0.3,pY2+lOut));
  pPoints.push_back(Ogre::Vector2(pX2 + 0.3,pY2+lOutH));
}

/*-----------------------------------------------------------------------------------------------*/

void Street::downUp(std::vector<Ogre::Vector2> &pPoints, int pX1, int pX2, int pY1, int pY2)
{
  int lOutH = 15;
  int lOut = 21;

  pPoints.push_back(Ogre::Vector2(pX1 + 0.3,pY1+lOutH));
  pPoints.push_back(Ogre::Vector2(pX1 + 0.3,pY1+lOut));
  pPoints.push_back(Ogre::Vector2(pX2 - 1.1,pY2-lOut));
  pPoints.push_back(Ogre::Vector2(pX2 - 1.1,pY2-lOutH));
}

/*-----------------------------------------------------------------------------------------------*/

void Street::leftRight(std::vector<Ogre::Vector2> &pPoints, int pX1, int pX2, int pY1, int pY2)
{
  int lOutH = 17;
  int lOut = 23;

  pPoints.push_back(Ogre::Vector2(pX1-lOutH+0.5,pY1+3.25));
  pPoints.push_back(Ogre::Vector2(pX1-lOut+0.5,pY1+3.25));
  pPoints.push_back(Ogre::Vector2(pX2+lOut,pY2+0.9));
  pPoints.push_back(Ogre::Vector2(pX2+lOutH,pY2+0.9));
}

/*-----------------------------------------------------------------------------------------------*/

void Street::rightLeft(std::vector<Ogre::Vector2> &pPoints, int pX1, int pX2, int pY1, int pY2)
{//
  int lOutH = 17;
  int lOut = 23;

  pPoints.push_back(Ogre::Vector2(pX1+lOutH,pY1+1.1));
  pPoints.push_back(Ogre::Vector2(pX1+lOut,pY1+1.1));
  pPoints.push_back(Ogre::Vector2(pX2-lOut,pY2+3.25));
  pPoints.push_back(Ogre::Vector2(pX2-lOutH,pY2+3.25));
}

/*-----------------------------------------------------------------------------------------------*/

void Street::generateAndAddToSceneManager(Ogre::SceneManager* pSceneManager,
                                          boost::shared_ptr< Terrain > pTerrain)
{
  mSceneManager = pSceneManager;

  std::stringstream lTemp;
  std::string lName;
  lTemp << mX1 << mY1 << mX2 << mY2;
  lName = lTemp.str() + "bezier";

  // points for bezier curve
  std::vector< Ogre::Vector2 > lPoints;

  // first cubic bezier def.-points offset from city
  int lOutH = 17;
  int lOut = 22;

  // determine city street node to attach to (up, left, right, bottom)
  if (mX2 >= mX1) {
    if((mY2 > mY1) && ((mY2 - mY1) < (mX2 - mX1)))
      rightLeft(lPoints, mX1, mX2, mY1, mY2);
    else if(mY2 >= mY1)
      downUp(lPoints, mX1, mX2, mY1, mY2);

    if((mY2 < mY1) && ((mY1 - mY2) < (mX2 - mX1)))
      rightLeft(lPoints, mX1, mX2, mY1, mY2);
    else if(mY2 <= mY1)
      upDown(lPoints, mX1, mX2, mY1, mY2);
  } else {
    if((mY2 > mY1) && ((mY2 - mY1) < (mX1 - mX2)))
      leftRight(lPoints, mX1, mX2, mY1, mY2);
    else if(mY2 >= mY1)
      downUp(lPoints, mX1, mX2, mY1, mY2);

    if((mY2 < mY1) && ((mY1 - mY2) < (mX1 - mX2)))
      leftRight(lPoints, mX1, mX2, mY1, mY2);
    else if(mY2 <= mY1)
      upDown(lPoints, mX1, mX2, mY1, mY2);
  }


  // create bezier curve
  BezierCurve2D lCurve(lPoints, 3.0, 20);

  std::vector<Ogre::Vector2> lBezierPoints = lCurve.getPoints();
  mBezierPoints = lBezierPoints;

  Ogre::ManualObject* manual = pSceneManager->createManualObject(lName);
  manual->begin("roadtex", Ogre::RenderOperation::OT_TRIANGLE_LIST);

  float lX,lY;
  int j;
  int lSubdivLength = lBezierPoints.size();

  std::vector< float > lLengthAtIndex;
  float lCompleteLength = 0.0;

  lLengthAtIndex.push_back(0.0);

  for (int l = 1; l < lSubdivLength; l ++) {
    lCompleteLength += (lBezierPoints[l-1]-lBezierPoints[l]).length();
    lLengthAtIndex.push_back(lCompleteLength/4);
  }

  int lHeightQuadCarPath = 20;

  for (int i = 0; i < lSubdivLength - 1; i++) {
    int lHeightQuadFirst = 20;
    int lHeightQuadSecond = 20;

    if (i == 0)
      lHeightQuadFirst = 5;
    else if (i == (lSubdivLength-2))
      lHeightQuadSecond = 5;

    Ogre::Vector2 lCol[4];

    Ogre::Vector2 lTangentNorm = (lBezierPoints[i+1] - lBezierPoints[i]);
    lTangentNorm.normalise();

    Ogre::Vector2 lTangentNormNext;

    if (i < lSubdivLength - 2)
      lTangentNormNext = (lBezierPoints[i+2] - lBezierPoints[i+1]);
    else
      lTangentNormNext = lTangentNorm;

    lTangentNormNext.normalise();

    Ogre::Vector2 lTangentVertNorm = lTangentNorm.perpendicular();
    lTangentVertNorm.normalise();

    Ogre::Vector2 lTangentVertNormNext = lTangentNormNext.perpendicular();
    lTangentVertNormNext.normalise();

    lTangentVertNormNext *= 1.3;
    lTangentVertNorm *= 1.3;

    if ((lTangentVertNormNext.x == 0) && (lTangentVertNormNext.y == 0))
      lTangentVertNormNext = lTangentVertNorm;

    j = i*10;

    mCarPath.push_back(Ogre::Vector3(lBezierPoints[i].x,
      pTerrain->getHighestFromQuad(lBezierPoints[i].x,
      lBezierPoints[i].y,lHeightQuadCarPath) + 0.6, lBezierPoints[i].y));

    lX = lBezierPoints[i].x + lTangentVertNorm.x;
    lY = lBezierPoints[i].y + lTangentVertNorm.y;

    lCol[0].x = lX;
    lCol[0].y = lY;

    manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,lHeightQuadFirst)
      + 0.2, lY);
    manual->textureCoord(1, lLengthAtIndex[i]);


    lX = lBezierPoints[i+1].x + lTangentVertNormNext.x;
    lY = lBezierPoints[i+1].y + lTangentVertNormNext.y;

    lCol[1].x = lX;
    lCol[1].y = lY;

    manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,lHeightQuadSecond)
      + 0.2, lY);
    manual->textureCoord(1,lLengthAtIndex[i+1]);


    lX = lBezierPoints[i].x - lTangentVertNorm.x;
    lY = lBezierPoints[i].y - lTangentVertNorm.y;

    lCol[2].x = lX;
    lCol[2].y = lY;

    manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,lHeightQuadFirst)
      + 0.2, lY);
    manual->textureCoord(0,lLengthAtIndex[i]);

    // second tri

    lX = lBezierPoints[i].x - lTangentVertNorm.x;
    lY = lBezierPoints[i].y - lTangentVertNorm.y;

    manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,lHeightQuadFirst)
      + 0.2, lY);
    manual->textureCoord(0,lLengthAtIndex[i]);


    lX = lBezierPoints[i+1].x + lTangentVertNormNext.x;
    lY = lBezierPoints[i+1].y + lTangentVertNormNext.y;

    manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,lHeightQuadSecond)
      + 0.2, lY);
    manual->textureCoord(1,lLengthAtIndex[i+1]);


    lX = lBezierPoints[i+1].x - lTangentVertNormNext.x;
    lY = lBezierPoints[i+1].y - lTangentVertNormNext.y;

    lCol[3].x = lX;
    lCol[3].y = lY;

    manual->position(lX, pTerrain->getHighestFromQuad(lX,lY,lHeightQuadSecond)
      + 0.2, lY);
    manual->textureCoord(0,lLengthAtIndex[i+1]);

    mCollidableRects.push_back(alignPoints(lCol[3],lCol[1],lCol[2],lCol[0]));

    lHeightQuadCarPath = lHeightQuadSecond;
  }

  mCarPath.push_back(Ogre::Vector3(lBezierPoints[lBezierPoints.size() - 1].x,
    pTerrain->getHighestFromQuad(lBezierPoints[lBezierPoints.size() - 1].x,
    lBezierPoints[lBezierPoints.size() - 1].y,
    lHeightQuadCarPath) + 0.6,
    lBezierPoints[lBezierPoints.size() - 1].y));

  manual->end();

  manual->setCastShadows(false);

  manual->setRenderQueueGroup(97);

  pSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

  // Fill in lines list
  for (size_t k = 1; k < lBezierPoints.size(); k++) {
    mLineList.push_back(lBezierPoints[k-1]);
    mLineList.push_back(lBezierPoints[k]);
  }

  loadCars();
}

/*-----------------------------------------------------------------------------------------------*/

RotatedRect Street::alignPoints(Ogre::Vector2 p1, Ogre::Vector2 p2, Ogre::Vector2 p3,
                                Ogre::Vector2 p4)
{
  return RotatedRect(p1, p2, p3, p4);
}

/*-----------------------------------------------------------------------------------------------*/

bool Street::isAutoremove(void)
{
  return false;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector< RotatedRect > Street::getGroundRects()
{
  return mCollidableRects;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 Street::getPosition(void)
{
  return Ogre::Vector2(-1.0,-1.0);
}

/*-----------------------------------------------------------------------------------------------*/

bool Street::isSelectable(void)
{
  return false;
}

/*-----------------------------------------------------------------------------------------------*/

eGameObjectType Street::getType(void)
{
  return eStreet;
}

/*-----------------------------------------------------------------------------------------------*/

void Street::updateCars(float pElapsedTime)
{
  if (mCarDriving) {
    mCar->getSubEntity(0)->getMaterial()->getTechnique(0)
      ->getPass(0)->setAmbient(Ogre::ColourValue(1.0,1.0,1.0,1.0));
    mCar->getSubEntity(0)->getMaterial()->getTechnique(0)
      ->getPass(0)->setDiffuse(Ogre::ColourValue(1.0,1.0,1.0,1.0));
    mCar->getSubEntity(0)->getMaterial()->getTechnique(0)
      ->getPass(0)->setSceneBlending(Ogre::SBF_ONE, Ogre::SBF_ZERO);
    mCar->getSubEntity(0)->getMaterial()->getTechnique(0)
      ->getPass(0)->setDepthWriteEnabled(true);

    Ogre::Vector3 lDirection = mCurrentCarPath.normalisedCopy();

    mCarNode->translate(lDirection * pElapsedTime * 10);
    mCurrentLengthDone += (lDirection * pElapsedTime * 10).length();

    if (mCurrentLengthDone > mCurrentCarPath.length()) {
      if ((mPathIndex + 1) < ((int)mCarPath.size())) {
        mCarNode->setPosition(mCarPath[mPathIndex]);
        mCurrentCarPath = mCarPath[mPathIndex + 1] - mCarPath[mPathIndex];
        Ogre::Vector2 lDirection = mBezierPoints[mPathIndex + 1]
        - mBezierPoints[mPathIndex];
        mCarNode->resetOrientation();
        mCarNode->setDirection(Ogre::Vector3(lDirection.x, 0, lDirection.y));
        mCurrentLengthDone = 0.0;
        mPathIndex++;
      } else {
        mCarFadingOut = true;
        mCarDriving = false;
        mCarFader = 0.0;
      }
    }
  }
  else if (mCarFadingIn) {
    mWait -= pElapsedTime;

    if (mWait < 0) {
      mCarNode->setVisible(true);
      mCarFader += pElapsedTime*2;

      mCar->getSubEntity(0)->getMaterial()->getTechnique(0)
        ->getPass(0)->setAmbient(Ogre::ColourValue(1.0,1.0,1.0,mCarFader));
      mCar->getSubEntity(0)->getMaterial()->getTechnique(0)
        ->getPass(0)->setDiffuse(Ogre::ColourValue(1.0,1.0,1.0,mCarFader));
      mCar->getSubEntity(0)->getMaterial()->getTechnique(0)
        ->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
      mCar->getSubEntity(0)->getMaterial()->getTechnique(0)
        ->getPass(0)->setDepthWriteEnabled(false);

      if (mCarFader > 0.95) {
        mCarFadingIn = false;
        mCarDriving = true;
      }
    }
  }
  else if (mCarFadingOut) {
    mCarFader += pElapsedTime*2;

    mCar->getSubEntity(0)->getMaterial()->getTechnique(0)
      ->getPass(0)->setAmbient(Ogre::ColourValue(1.0,1.0,1.0,1.0 - mCarFader));
    mCar->getSubEntity(0)->getMaterial()->getTechnique(0)
      ->getPass(0)->setDiffuse(Ogre::ColourValue(1.0,1.0,1.0,1.0 - mCarFader));
    mCar->getSubEntity(0)->getMaterial()->getTechnique(0)
      ->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    mCar->getSubEntity(0)->getMaterial()->getTechnique(0)
      ->getPass(0)->setDepthWriteEnabled(false);

    if (mCarFader > 0.95)  {
      mCarFadingOut = false;
      mCarNode->setVisible(false);
    }
  } else {
    mCarNode->detachAllObjects();
    mSceneManager->destroyEntity(mCar);
    Ogre::MaterialManager::getSingleton().remove(mCarMaterial->getHandle());
    mSceneManager->destroySceneNode(mCarNode);

    loadCars();

    mCurrentCarPath = mCarPath[1] - mCarPath[0];
    mCurrentLengthDone = 0.0;
    mPathIndex = 1;
    mCarFader = 0.0;
    mWait = rand() % 25;

    mCarNode->setPosition(mCarPath[0]);
    Ogre::Vector2 lDirection = mBezierPoints[1] - mBezierPoints[0];
    mCarNode->resetOrientation();
    mCarNode->setDirection(Ogre::Vector3(lDirection.x, 0, lDirection.y));
    mCarNode->setScale(Ogre::Vector3(0.030,0.030,0.030));

    mCarFadingIn = true;
  }
}

/*-----------------------------------------------------------------------------------------------*/

void Street::loadCars(void)
{
  int lTruck = rand() % 8;

  if (lTruck == 0) {
    mCar = mSceneManager->createEntity("Car" + getID(), "truck1.mesh");
    mCarMaterial = Ogre::MaterialManager::getSingleton().create(
      "CarMat" + getID(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    mCarMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(
      "truck1CompleteMap.tga");

  } else {
    mCar = mSceneManager->createEntity("Car" + getID(), "car1.mesh");
    mCarMaterial = Ogre::MaterialManager::getSingleton().create(
      "CarMat" + getID(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    std::string lTextureString = "car" + toString((rand() % 4) + 1) + "CompleteMap.tga";
    mCarMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(lTextureString);
  }

  mCar->setMaterial(mCarMaterial);

  mCarNode = mSceneManager->getRootSceneNode()->createChildSceneNode("CarNode" + getID());
  mCarNode->attachObject(mCar);
  mCar->setRenderQueueGroup(98);
  mCarNode->setVisible(false);
}