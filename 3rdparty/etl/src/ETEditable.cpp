/********************************************************************************
EDITABLE TERRAIN LIBRARY v3 for Ogre

Copyright (c) 2008 Holger Frydrych <frydrych@oddbeat.de>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
********************************************************************************/

#include "ETEditable.h"
#include "ETBrush.h"
#include <cassert>
#include <OgreRectangle.h>

using namespace Ogre;
using namespace std;


namespace ET
{
  EditableListener::~EditableListener()
  {
  }


  Editable::Editable(const Vector2& gridScale, unsigned int gridWidth,
      unsigned int gridHeight)
  : mGridScale(gridScale), mGridWidth(gridWidth), mGridHeight(gridHeight)
  {
  }

  Editable::~Editable()
  {
    for (EditListenerList::iterator it = mEditListeners.begin(); it != mEditListeners.end(); ++it)
      (*it)->notifyDestroyed(this);
  }




  Rect Editable::determineEditRect(Real x, Real z, const Brush* brush)
  {
    Rectangle bounds = brush->getBoundingRect();
    bounds.left += x;
    bounds.right += x;
    bounds.top += z;
    bounds.bottom += z;
    bounds.left = ceil(bounds.left / mGridScale.x);
    bounds.right = floor(bounds.right / mGridScale.x);
    bounds.top = ceil(bounds.top / mGridScale.y);
    bounds.bottom = floor(bounds.bottom / mGridScale.y);
    // get the according integer coordinates
    Rect editRect;
    editRect.x1 = (unsigned int) max<Real>(0, bounds.left);
    editRect.y1 = (unsigned int) max<Real>(0, bounds.top);
    editRect.x2 = (unsigned int) min<Real>(mGridWidth-1, bounds.right);
    editRect.y2 = (unsigned int) min<Real>(mGridHeight-1, bounds.bottom);
    mLastEditedRect = editRect;
    return editRect;
  }


  void Editable::addEditableListener(EditableListener* listener)
  {
    if (find(mEditListeners.begin(), mEditListeners.end(), listener) == mEditListeners.end())
      mEditListeners.push_back(listener);
  }

  void Editable::removeEditableListener(EditableListener* listener)
  {
    EditListenerList::iterator it = find(mEditListeners.begin(), mEditListeners.end(), listener);
    if (it != mEditListeners.end())
      mEditListeners.erase(it);
  }
}

