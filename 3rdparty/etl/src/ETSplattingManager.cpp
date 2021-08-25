#include "ETSplattingManager.h"
#include "ETSplattingLayout.h"
#include <functional>
#include <iostream>

using namespace std;
using namespace Ogre;


namespace ET
{
  SplattingManager::SplattingManager(const String& baseName, const String& group,
      unsigned int texWidth, unsigned int texHeight, PixelFormat format, 
      const Vector2& gridScale, const String& baseTexture)
  : Editable(gridScale, texWidth, texHeight)
  {
    mSplattingLayout = new SplattingLayout(baseName, group, texWidth, texHeight,
      format, gridScale, 1);
    mTexChanMap[baseTexture] = 0;
    mNumUsedChannels = 1;
    mTextures.push_back(baseTexture);
    mDirtyRects.push_back(Rect(0,0,texWidth-1,texHeight-1));
  }

  SplattingManager::~SplattingManager()
  {
    delete mSplattingLayout;
  }


  SplattingLayout* SplattingManager::getSplattingLayout() const
  {
    return mSplattingLayout;
  }


  void SplattingManager::checkChannelsUsed()
  {
    static const Real THRESHOLD = 0.05;

    // check all used channels if they actually contain data
    for (unsigned int i = 0; i < mNumUsedChannels; ++i)
    {
      if (mTextures[i].empty())
        continue;
      Rect& r = mDirtyRects[i];
      // we check all values in the dirty rect if they are > 0
      // we check the rect in circles so that we can potentially
      // make it smaller for later queries.
      bool chanUsed = false;
      cout << "Checking channel " << i << " with dirty rect: ";
      cout << r.x1 << "," << r.y1 << "," << r.x2 << "," << r.y2 << endl;
      while (r.y1 <= r.y2 && r.x1 <= r.x2)
      {
        for (int x = r.x1; x <= r.x2; ++x)
        {
          Real contents1 = mSplattingLayout->getChannelContent(x, r.y1, i);
          Real contents2 = mSplattingLayout->getChannelContent(x, r.y2, i);
          if (contents1 > THRESHOLD || contents2 > THRESHOLD)
          {
            cout << "Channel used at x = " << x << ", y = (" << r.y1 <<","<<r.y2<<"): ";
            cout << contents1 << ", " << contents2 << "\n";
            chanUsed = true;
            break;
          }
        }
        if (chanUsed)
          break;
        for (int y = r.y1+1; y < r.y2; ++y)
        {
          Real contents1 = mSplattingLayout->getChannelContent(r.x1, y, i);
          Real contents2 = mSplattingLayout->getChannelContent(r.x2, y, i);
          if (contents1 > THRESHOLD || contents2 > THRESHOLD)
          {
            cout << "Channel used at x = (" << r.x1<<","<<r.x2 << ", y = " << y<<"): ";
            cout << contents1 << ", " << contents2 << "\n";
            chanUsed = true;
            break;
          }
        }
        if (chanUsed)
          break;
        ++r.x1;
        ++r.y1;
        --r.x2;
        --r.y2;
      }

      if (!chanUsed)
      {
        cout << "Channel is not in use." << endl;
        // channel is not used, so mark it as free
        mTexChanMap.erase(mTextures[i]);
        mTextures[i].clear();
        mDirtyRects[i] = Rect(mGridWidth, mGridHeight, 0, 0);
        if (i == mNumUsedChannels-1)
        {
          cout << "Channel is last in list, delete it." << endl;
          // last channel, simply delete it
          mTextures.erase(mTextures.end()-1);
          mDirtyRects.erase(mDirtyRects.end()-1);
          --mNumUsedChannels;
          mSplattingLayout->setNumChannels(mNumUsedChannels);
        }
      }
    }
  }


  void SplattingManager::addTexture(const String& texture)
  {
    // first, see if there is an empty spot in the already occupied
    // channels.
    unsigned int numMaps = mSplattingLayout->getNumSplattingMaps();
    checkChannelsUsed();
    bool inserted = false;
    bool updateMaps = false;
    for (size_t i = 0; i < mTextures.size(); ++i)
    {
      if (mTextures[i].empty())
      {
        mTextures[i] = texture;
        mTexChanMap[texture] = i;
        cout << "Reusing channel " << i << "\n";
        inserted = true;
      }
    }
    
    if (!inserted)
    {
      // no free slot: simply add it to the end
      mTextures.push_back(texture);
      mDirtyRects.push_back(Rect(mGridWidth, mGridHeight, 0, 0));
      mTexChanMap[texture] = mNumUsedChannels;
      ++mNumUsedChannels;
      mSplattingLayout->setNumChannels(mNumUsedChannels);
      cout << "Added texture in channel " << mNumUsedChannels-1 << "\n";
    }
    updateMaps = numMaps != mSplattingLayout->getNumSplattingMaps();

    // inform listeners about texture list change
    for (vector<SplattingListener*>::iterator l = mListeners.begin(); l != mListeners.end(); ++l)
    {
      (*l)->notifySplattingTextures(mTextures);
      if (updateMaps)
        (*l)->notifySplattingMaps(mSplattingLayout->getSplattingMapNames());
    }
  }


  void SplattingManager::splat(Real x, Real z, const Brush* brush, const String& texture,
      float intensity)
  {
    if (mTexChanMap.find(texture) == mTexChanMap.end())
      addTexture(texture);
    unsigned int chan = mTexChanMap[texture];
    mSplattingLayout->splat(x, z, brush, chan, intensity);
    // update potential area rect for the used texture
    Rect ur = mSplattingLayout->getLastEditedRect();
    Rect& dr = mDirtyRects[chan];
    dr.x1 = min(dr.x1, ur.x1);
    dr.y1 = min(dr.y1, ur.y1);
    dr.x2 = max(dr.x2, ur.x2);
    dr.y2 = max(dr.y2, ur.y2);
  }

  void SplattingManager::splat(const Vector3& pos, const Brush* brush, const String& texture,
      float intensity)
  {
    Vector3 localPos = transformPositionW2L(pos);
    splat(localPos.x, localPos.z, brush, texture, intensity);
  }


  void SplattingManager::addListener(SplattingListener* listener)
  {
    mListeners.push_back(listener);
    listener->notifySplattingTextures(mTextures);
    listener->notifySplattingMaps(mSplattingLayout->getSplattingMapNames());
  }

  void SplattingManager::removeListener(SplattingListener* listener)
  {
    vector<SplattingListener*>::iterator it = find(mListeners.begin(), mListeners.end(), listener);
    if (it != mListeners.end())
      mListeners.erase(it);
  }


  void SplattingManager::update()
  {
    mSplattingLayout->update();
  }
}

