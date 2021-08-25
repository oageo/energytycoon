#include "StdAfx.h"
#include "Bezier.h"				

/*-----------------------------------------------------------------------------------------------*/

BezierCurve2D::BezierCurve2D(std::vector< Ogre::Vector2 > pPoints, double pError, int pSubdivision)
: mSubdivision(pSubdivision),
  mPoints(pPoints)
{
  fitCubic(0, pPoints.size() - 1 , computeLeftTangent(0),
    computeRightTangent(pPoints.size() - 1), pError);
}

/*-----------------------------------------------------------------------------------------------*/

std::vector< Ogre::Vector2 > BezierCurve2D::getPoints(void)
{
  return mCurveDone;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 BezierCurve2D::approximatePoint(float pT, std::vector< Ogre::Vector2 > pCurve)
{
  std::vector< std::vector< Ogre::Vector2 > > lData;

  lData.push_back(pCurve);
  lData.resize(4);
  lData[1].resize(4);
  lData[2].resize(4);
  lData[3].resize(4);

  for (size_t j = 1; j < 4; j++) {	
    for (size_t i = 0; i < (lData[0].size() - j); i++) {
      // subdivision with factor t
      lData[j][i]  = ((1.0 - pT) * lData[j-1][i]) + pT * lData[j-1][i+1]; 
    }
  }

  return lData[3][0];
}

/*-----------------------------------------------------------------------------------------------*/

void BezierCurve2D::approximateCurve(std::vector< Ogre::Vector2 > pCurve)
{
  std::vector< Ogre::Vector2 > lPointsOnBez;

  for (int i = 0; i <= mSubdivision; i++) {
    lPointsOnBez.push_back(approximatePoint((float)i / (float)mSubdivision, pCurve));
  }

  for (size_t i = 0; i < lPointsOnBez.size(); i++) {
    if (i > 0) {
      if (lPointsOnBez[i] != lPointsOnBez[i-1])
        mCurveDone.push_back(lPointsOnBez[i]);
      else
        int z = 7;
    } else if (mCurveDone.size() == 0) {
      mCurveDone.push_back(lPointsOnBez[i]);
    }
  }
}

/*-----------------------------------------------------------------------------------------------*/

void BezierCurve2D::fitCubic(int pFirst, int pLast, Ogre::Vector2 pHat1, Ogre::Vector2 pHat2,
                             double pError)
{
  std::vector< double > lU; // parameter values for point
  std::vector< double > lUPrime;	// improved parameter values

  double lMaxError; // maximum fitting error
  int	lSplitPoint; // point to split point set at
  int	lMaxIterations = 4; // max times to try iterating

  double lIterationError = pError * pError;
  int	lNumberPoints = pLast - pFirst + 1;

  // use heuristic if region only has two points in it
  if (lNumberPoints == 2) {
    double lDistance = (mPoints[pLast] - mPoints[pFirst]).length() / 3.0;

    mControlPoints.resize(4);

    mControlPoints[0] = mPoints[pFirst];
    mControlPoints[3] = mPoints[pLast];
    mControlPoints[1] = mControlPoints[0] + (pHat1 * lDistance);
    mControlPoints[2] = mControlPoints[3] + (pHat2 * lDistance);

    approximateCurve(mControlPoints);

    return;
  }

  // parameterize points, and attempt to fit curve
  lU = chordLengthParameterize(pFirst, pLast);
  mControlPoints = generateBezier(pFirst, pLast, lU, pHat1, pHat2);

  // find max deviation of points to fitted curve
  lMaxError = computeMaxError(pFirst, pLast, lU, lSplitPoint);

  if (lMaxError < pError) {
    approximateCurve(mControlPoints);
    return;
  }

  // if error not too large, try some reparameterization
  // and iteration
  if(lMaxError < lIterationError) {
    for(int i = 0; i < lMaxIterations; i++) {
      lUPrime = reparameterize(pFirst, pLast, lU);
      mControlPoints = generateBezier(pFirst, pLast, lUPrime, pHat1, pHat2);
      lMaxError = computeMaxError(pFirst, pLast, lUPrime, lSplitPoint);

      if (lMaxError < pError) {
        approximateCurve(mControlPoints);
        return;
      }

      lU = lUPrime;
    }
  }

  // fitting failed - split at max error point and fit recursively
  Ogre::Vector2 lHatCenter = computeCenterTangent(lSplitPoint);
  fitCubic(pFirst, lSplitPoint, pHat1, lHatCenter, pError);
  lHatCenter = lHatCenter * -1;
  fitCubic(lSplitPoint, pLast, lHatCenter, pHat2, pError);
}

/*-----------------------------------------------------------------------------------------------*/

std::vector< Ogre::Vector2 > BezierCurve2D::generateBezier(int pFirst, int pLast,
                                                           std::vector< double > pUPrime,
                                                           Ogre::Vector2 pHat1,
                                                           Ogre::Vector2 pHat2)
{
  Ogre::Vector2 lA[cMaxPoints][2]; // precomputed rhs for eqn
  double lC[2][2]; // matrix C		
  double lX[2]; // matrix X	

  Ogre::Vector2 lTemp;

  mControlPoints.resize(4);
  int lNumberPoints = pLast - pFirst + 1;

  // compute the A's
  for(int i = 0; i < lNumberPoints; i++) {
    Ogre::Vector2	lV1, lV2;
    lV1 = pHat1;
    lV2 = pHat2;
    lV1 = lV1 * b1(pUPrime[i]);
    lV2 = lV2 * b2(pUPrime[i]);
    lA[i][0] = lV1;
    lA[i][1] = lV2;
  }

  // create the C and X matrices	
  lC[0][0] = 0.0;
  lC[0][1] = 0.0;
  lC[1][0] = 0.0;
  lC[1][1] = 0.0;
  lX[0]    = 0.0;
  lX[1]    = 0.0;

  for(int j = 0; j < lNumberPoints; j++) {
    lC[0][0] += lA[j][0].dotProduct(lA[j][0]);
    lC[0][1] += lA[j][0].dotProduct(lA[j][1]);
    lC[1][0] = lC[0][1];
    lC[1][1] += lA[j][1].dotProduct(lA[j][1]);

    lTemp = mPoints[pFirst + j] - ((mPoints[pFirst] * b0(pUPrime[j]))
      + ((mPoints[pFirst] * b1(pUPrime[j]))
      + ((mPoints[pLast] * b2(pUPrime[j]))
      + (mPoints[pLast] * b3(pUPrime[j])))));


    lX[0] += lA[j][0].dotProduct(lTemp); 
    lX[1] += lA[j][1].dotProduct(lTemp);
  }

  // compute the determinants of C and X	
  double lDetC0C1 = lC[0][0] * lC[1][1] - lC[1][0] * lC[0][1];
  double lDetC0X  = lC[0][0] * lX[1]    - lC[1][0] * lX[0];
  double lDetXC1  = lX[0]    * lC[1][1] - lX[1]    * lC[0][1];

  // finally, derive alpha values	
  double lAlphaL = (lDetC0C1 == 0) ? 0.0 : lDetXC1 / lDetC0C1;
  double lAlphaR = (lDetC0C1 == 0) ? 0.0 : lDetC0X / lDetC0C1;

  double lSegmentLength = (mPoints[pLast] - mPoints[pFirst]).length();
  double lEpsilon = 1.0e-6 * lSegmentLength;

  if(lAlphaL < lEpsilon || lAlphaR < lEpsilon) {
    // fall back on standard formula, and subdivide further if needed
    double lDistance = lSegmentLength / 3.0;
    mControlPoints[0] = mPoints[pFirst];
    mControlPoints[3] = mPoints[pLast];
    mControlPoints[1] = mControlPoints[0] + (pHat1 * lDistance);
    mControlPoints[2] = mControlPoints[3] + (pHat2 * lDistance);

    return mControlPoints;
  }

  mControlPoints[0] = mPoints[pFirst];
  mControlPoints[3] = mPoints[pLast];
  mControlPoints[1] = mControlPoints[0] + (pHat1 * lAlphaL);
  mControlPoints[2] = mControlPoints[3] + (pHat2 * lAlphaR);

  return mControlPoints;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector< double > BezierCurve2D::reparameterize(int pFirst, int pLast,
                                                    std::vector< double > pU)
{
  int lNumberPoints = pLast - pFirst + 1;	
  std::vector< double > lUPrime; // new parameter values	

  lUPrime.resize(lNumberPoints);

  for(int i = pFirst; i <= pLast; i++)
    lUPrime[i - pFirst] = newtonRaphsonRootFind(mControlPoints, mPoints[i], pU[i - pFirst]);

  return lUPrime;
}

/*-----------------------------------------------------------------------------------------------*/

double BezierCurve2D::newtonRaphsonRootFind(std::vector< Ogre::Vector2 > pQ, Ogre::Vector2 pP,
                                            double pU)
{
  std::vector< Ogre::Vector2 > lQ1, lQ2;	// Q' and Q''

  lQ1.resize(3);
  lQ2.resize(2);

  Ogre::Vector2	lQU, lQ1U, lQ2U; // u evaluated at Q, Q', & Q''

  // compute Q(u)
  lQU = bezierII(3, pQ, pU);

  // generate control vertices for Q'
  for(int i = 0; i <= 2; i++) 
  {
    lQ1[i].x = (pQ[i + 1].x - pQ[i].x) * 3.0;
    lQ1[i].y = (pQ[i + 1].y - pQ[i].y) * 3.0;
  }

  // generate control vertices for Q''

  for(int j = 0; j <= 1; j++) 
  {
    lQ2[j].x = (lQ1[j + 1].x - lQ1[j].x) * 2.0;
    lQ2[j].y = (lQ1[j + 1].y - lQ1[j].y) * 2.0;
  }

  // compute Q'(u) and Q''(u)	
  lQ1U = bezierII(2, lQ1, pU);
  lQ2U = bezierII(1, lQ2, pU);

  // compute f(u)/f'(u) 
  double lNumerator = (lQU.x - pP.x) * (lQ1U.x) + (lQU.y - pP.y) * (lQ1U.y);
  double lDenominator = (lQ1U.x) * (lQ1U.x) + (lQ1U.y) * (lQ1U.y) +
    (lQU.x - pP.x) * (lQ2U.x) + (lQU.y - pP.y) * (lQ2U.y);

  if(lDenominator == 0.0f)
    return pU;

  // u = u - f(u)/f'(u) 
  return (pU - (lNumerator / lDenominator));
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 BezierCurve2D::bezierII(int pDegree, std::vector< Ogre::Vector2 > pV, double pT)
{
  Ogre::Vector2 lQ;
  std::vector< Ogre::Vector2 > lVTemp = pV;	

  for(int i = 1; i <= pDegree; i++) {	
    for(int j = 0; j <= pDegree - i; j++) {
      lVTemp[j].x = (1.0 - pT) * lVTemp[j].x + pT * lVTemp[j + 1].x;
      lVTemp[j].y = (1.0 - pT) * lVTemp[j].y + pT * lVTemp[j + 1].y;
    }
  }

  lQ = lVTemp[0];

  return lQ;
}

/*-----------------------------------------------------------------------------------------------*/

double BezierCurve2D::b0(double pU)
{
  double lTemp = 1.0 - pU;
  return (lTemp * lTemp * lTemp);
}

/*-----------------------------------------------------------------------------------------------*/

double BezierCurve2D::b1(double pU)
{
  double lTemp = 1.0 - pU;
  return (3 * pU * (lTemp * lTemp));
}

/*-----------------------------------------------------------------------------------------------*/

double BezierCurve2D::b2(double pU)
{
  double lTemp = 1.0 - pU;
  return (3 * pU * pU * lTemp);
}

/*-----------------------------------------------------------------------------------------------*/

double BezierCurve2D::b3(double pU)
{
  return (pU * pU * pU);
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 BezierCurve2D::computeLeftTangent(int pEnd)
{
  Ogre::Vector2	lHat1;
  lHat1 = mPoints[pEnd + 1] - mPoints[pEnd];
  lHat1.normalise();

  return lHat1;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 BezierCurve2D::computeRightTangent(int pEnd)
{
  Ogre::Vector2	lHat2;
  lHat2 = mPoints[pEnd - 1] - mPoints[pEnd];
  lHat2.normalise();

  return lHat2;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 BezierCurve2D::computeCenterTangent(int pCenter)
{
  Ogre::Vector2	lV1, lV2, lHatCenter;

  lV1 = mPoints[pCenter - 1] - mPoints[pCenter];
  lV2 = mPoints[pCenter] -  mPoints[pCenter + 1];
  lHatCenter.x = (lV1.x + lV2.x) / 2.0;
  lHatCenter.y = (lV1.y + lV2.y) / 2.0;
  lHatCenter.normalise();

  return lHatCenter;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector< double > BezierCurve2D::chordLengthParameterize(int pFirst, int pLast)
{
  std::vector< double > lU; // parameterization

  lU.resize(pLast - pFirst + 1);

  lU[0] = 0.0;

  for(int i = pFirst + 1; i <= pLast; i++) 
    lU[i - pFirst] = lU[i - pFirst - 1] + (mPoints[i]- mPoints[i-1]).length();

  for(int j = pFirst + 1; j <= pLast; j++) 
    lU[j - pFirst] = lU[j - pFirst] / lU[pLast - pFirst];

  return lU;
}

/*-----------------------------------------------------------------------------------------------*/

double BezierCurve2D::computeMaxError(int pFirst, int pLast, std::vector< double > u,
                                      int &pSplitPoint)
{
  double lMaxDistance = 0.0;	
  double lCurrentDistance;
  Ogre::Vector2	lPointOnCurve;
  Ogre::Vector2	lPointToCurve;

  pSplitPoint = (pLast - pFirst + 1) / 2;

  for(int i = pFirst + 1; i < pLast; i++) {
    lPointOnCurve = bezierII(3, mControlPoints, u[i - pFirst]);
    lPointToCurve = lPointOnCurve - mPoints[i];

    lCurrentDistance = lPointToCurve.squaredLength();

    if(lCurrentDistance >= lMaxDistance){
      lMaxDistance = lCurrentDistance;
      pSplitPoint = i;
    }
  }

  return lMaxDistance;
}
