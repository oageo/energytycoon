#ifndef COLLIDABLE_H
#define COLLIDABLE_H

/*-----------------------------------------------------------------------------------------------*/

#include "GameObject.h"

/*-----------------------------------------------------------------------------------------------*/

//! Collidable interface
class Collidable : public GameObject {
public:

	//! Should it be removed if something is built on it
	virtual bool isAutoremove(void) = 0;

	//! Get ground rects
	virtual std::vector<RotatedRect> getGroundRects() = 0;

	//! Get main entity (for hover text bounding box)
	virtual Ogre::MovableObject* getMainEntityObject() = 0;
};

/*-----------------------------------------------------------------------------------------------*/

#endif // COLLIDABLE_H
