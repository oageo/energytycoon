#ifndef BEZIER_H
#define BEZIER_H

/*-----------------------------------------------------------------------------------------------*/

const double cDefaultBezierError = 4.0; //!< Maximum deviation from given points
const int cDefaultBezierSubdivision = 50; //!< Number of subdivisions
const int cMaxPoints = 1000; //!< Maximum number of points

/*-----------------------------------------------------------------------------------------------*/

//! Create a 2D Bezier curve from a set of points
//! Given a std::vector of points it will create a Bezier curve along those points
class BezierCurve2D {
public:

  //! Create the curve
  BezierCurve2D(std::vector< Ogre::Vector2 > pPoints, double pError = cDefaultBezierError,
                int pSubdivision = cDefaultBezierSubdivision);

  //! Returns all points of the created curve
  std::vector< Ogre::Vector2 > getPoints(void);

private:

  //! Approximates the complete curve from all control points
  void approximateCurve(std::vector< Ogre::Vector2 > pCurve);

  //! Approximate a single point from a cubic control set
  Ogre::Vector2 approximatePoint(float pT, std::vector< Ogre::Vector2 > pCurve);

  //! Fit a bezier curve to a (sub)set of digitized points
  void fitCubic(int pFirst, int pLast, Ogre::Vector2 pHat1, Ogre::Vector2 pHat2, double pError);

  //! Given set of points and their parameterization, try to find
  //! a better parameterization
  std::vector< double > reparameterize(int pFirst, int pLast, std::vector< double > pU);

  //! Use Newton-Raphson iteration to find better root
  double newtonRaphsonRootFind(std::vector< Ogre::Vector2 > pQ, Ogre::Vector2 pP, double pU);

  //! Evaluate a bezier curve at a particular parameter value
  Ogre::Vector2 bezierII(int pDegree, std::vector< Ogre::Vector2 > pV, double pT);

  // Bezier multipliers
  double b0(double pU);
  double b1(double pU);
  double b2(double pU);
  double b3(double pU);

  //! Approximate unit tangents at left endpoint of the curve
  Ogre::Vector2	computeLeftTangent(int pEnd);

  //! Approximate unit tangent at right endpoint of the curve
  Ogre::Vector2	computeRightTangent(int pEnd);

  //! Approximate unit tangent at center of the curve
  Ogre::Vector2	computeCenterTangent(int pCenter);

  //! Find the maximum squared distance of digitized points
  //! to fitted curve.
  double computeMaxError(int pFirst, int pLast, std::vector< double > pU, int &pSplitPoint);

  //! Assign parameter values to digitized points 
  //! using relative distances between points.
  std::vector< double > chordLengthParameterize(int pFirst, int pLast);

  //! Use least-squares method to find Bezier control points for region
  std::vector< Ogre::Vector2 > generateBezier(int pFirst, int pLast, std::vector<double> pUPrime,
                                              Ogre::Vector2 pHat1, Ogre::Vector2 pHat2);


  int mSubdivision; //!< Number of points the user wants to have
  std::vector< Ogre::Vector2 > mCurveDone; //!< Complete curve
  std::vector< Ogre::Vector2 > mPoints; //!< Points given by user
  std::vector< Ogre::Vector2 > mControlPoints; //!< Control points generated
};

/*-----------------------------------------------------------------------------------------------*/

#endif // BEZIER_H
