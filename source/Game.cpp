#include "StdAfx.h"
#include "Game.h"
#include "StringsLoc.h"
#include "Event.h"
#include "Powerplant.h"
#include "Pole.h"
#include "Timer.h"
#include "PowerLine.h"
#include "CompanyBuilding.h"
#include "ResourceBuilding.h"
#include "Audio.h"
#include "StaticGameObject.h"
#include "City.h"
#include "Terrain.h"
#include "Map.h"
#include "PanCamera.h"
#include "GUI.h"
#include "Event.h"
#include "GameTime.h"
#include "Collidable.h"
#include "Pole.h"
#include "PowerLine.h"

/*-----------------------------------------------------------------------------------------------*/

const bool cDemo = false; //!< Set to true to build demo binary

/*-----------------------------------------------------------------------------------------------*/

Game::Game(Ogre::RenderWindow* pRenderWindow, Ogre::SceneManager* pSceneManager,
           Ogre::Root* pRoot, bool pRestart)
           : mWindow(pRenderWindow),
           mSceneManager(pSceneManager),
           mSandbox(false),
           mLastFrameTime(0.0),
           mInputManager(0),
           mMouse(0),
           mKeyboard(0),
           mRoot(pRoot),
           mDismantle(false),
           mGamestate(eGUIExclusiveMode),
           mBuildable(false),
           mOverPoleConnectableNode(false),
           mRightButtonDown(false),
           mSmoothOutPan(false),
           mDemolishPartSys(0),
           mRunDemolishPart(false),
           mDemolishPartTime(0.0),
           mDemolishPartNode(0),
           mCoalCounter(0),
           mGasCounter(0),
           mUraniumCounter(0),
           mRun(true),
           mBuildLock(0),
           mDemoTime(0.0f),
           mDemoEventRaised(false)
{
  mPanCamera.reset(new PanCamera(mSceneManager, mWindow));

  startInputSystem();

  windowResized(mWindow);
  Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

  mGUI.reset(new GUI(mWindow, mRoot->getRenderSystem(), pRoot, pRestart));
  
  mGUI->setDemo(cDemo);

  MyGUI::PointerManager::getInstance().setPosition(MyGUI::IntPoint(mWindow->getWidth() / 2,
    mWindow->getHeight() / 2));

  REGISTER_CALLBACK(eGamestateChangeBuild, Game::enableBuildMode);
  REGISTER_CALLBACK(eGamestateChangeDemolish, Game::enableDemolishMode);
  REGISTER_CALLBACK(eGamestateChangeView, Game::enableSelectMode);
  REGISTER_CALLBACK(eGamestateChangeGUI, Game::enableGUIMode);
  REGISTER_CALLBACK(eDismantleGame, Game::dismantle);
  REGISTER_CALLBACK(eSerializeGame, Game::serialize);
  REGISTER_CALLBACK(eLoadGame, Game::load);
  REGISTER_CALLBACK(eShutdownGame, Game::unload);

  mLastMousePos.x = mWindow->getWidth() / 2;
  mLastMousePos.y = mWindow->getHeight() / 2;
  mLastMouseSize.x = mWindow->getWidth();
  mLastMouseSize.y = mWindow->getHeight();

  // Fresh start -> straight to main menu
  if (!pRestart) {
    Audio::setMenu();
    mGUI->switchToMainMenu();
    mGUI->setGameRunning(false);
  }

  // Shadow settings
  mSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);

  double lShadowBrightness = GameConfig::getDouble("ShadowBrightness");

  mSceneManager->setShadowColour(Ogre::ColourValue(lShadowBrightness,
    lShadowBrightness, lShadowBrightness));

  setupDemolishParticles();
}

/*-----------------------------------------------------------------------------------------------*/

void Game::loadMap(std::string pMapPrefix)
{
  mGameTime.reset(new GameTime());

  EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(22));
  EventHandler::raiseEvent(eSetLoadingStatus, new EventArg<std::string > (
    StrLoc::get()->LoadingTerrain()));

  mMap.reset(new Map(pMapPrefix, mSceneManager));

  EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(65));

  mPanCamera->setMapSize(mMap->getWidth(), mMap->getHeight());

  mBuildRequestNode = mSceneManager->getRootSceneNode()
    ->createChildSceneNode("build_request", Ogre::Vector3(0.0, 0.0, 0.0));

  double lBuildRequestScale = GameConfig::getDouble("BuildRequestNodeScale");

  mBuildRequestNode->scale(lBuildRequestScale, lBuildRequestScale,
    lBuildRequestScale);
}

/*-----------------------------------------------------------------------------------------------*/

void Game::load(EventData* pData)
{
  EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(0));

  mRaySceneQuery = mSceneManager->createRayQuery(Ogre::Ray());

  StartData lStartData = static_cast<EventArg<StartData>*> (pData)->mData1;
  mSandbox = lStartData.mSandbox;

  EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(10));

  if ((lStartData.mSerialized) 
    || (getStartDataFromMission(lStartData.mMission,
      StrLoc::get()->MyGUILanguage())[2] != "default")) {
      boost::shared_ptr<TiXmlDocument> lSavegame;

      if (lStartData.mSerialized) {
				
				std::string lUserHomeDirectory(getenv("HOME"));
				std::string lSaveGameDirectory = lUserHomeDirectory + "/.energytycoon/";
        lSavegame.reset(new TiXmlDocument((lSaveGameDirectory + "saved_games/"
          + lStartData.mSerializedFile + ".xml").c_str()));
      } else {
        lSavegame.reset( new TiXmlDocument((cDataDirPre + "saved_games/"
          + getStartDataFromMission(lStartData.mMission,
          StrLoc::get()->MyGUILanguage())[2]).c_str()));
      }

      lSavegame->LoadFile(TIXML_ENCODING_UTF8);

      TiXmlNode* lRootNode;

      lRootNode = lSavegame->FirstChildElement("savegame");

      std::string lMapPrefix = lRootNode->ToElement()->Attribute("map");
      std::string lSandbox = lRootNode->ToElement()->Attribute("sandbox");

      mMission = lRootNode->ToElement()->Attribute("mission");
      mSandbox = (lSandbox == "yes") ? true : false;

      EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(20));

      loadMap(lMapPrefix);

      EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(70));
      EventHandler::raiseEvent(eSetLoadingStatus, new EventArg<std::string > (
        StrLoc::get()->LoadingCompanyData()));

      mCompany.reset(new Company(mMission, mSandbox, mMap->getAllCities(), mMap));

      mGUI->setCompany(mCompany);

      mGUI->setTutorial(mMission.find("tutorial") != std::string::npos);

      mCompany->deserializeFromXMLElement(lRootNode, mMap, mSceneManager,
        mCompany, mGUI);
      mGameTime->deserializeFromXMLElement(lRootNode);

      EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(80));
  } else {
    mMission = lStartData.mMission;
    loadMap(getStartDataFromMission(lStartData.mMission,
      StrLoc::get()->MyGUILanguage())[1]);
			
    EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(70));

    mCompany.reset(new Company(lStartData.mMission, lStartData.mSandbox,
      mMap->getAllCities(), mMap));

    mGUI->setCompany(mCompany);

    mGUI->setTutorial(mMission.find("tutorial") != std::string::npos);


    EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(90));
		
  }

  EventHandler::raiseEvent(eSetLoadingProgress, new EventArg<int>(100));

  mGUI->setMission(mMission);
  mGUI->finishLoading();
  mGUI->setMinimap(mMap->getName() + "_minimap.png");

  Audio::setGame();

  if (lStartData.mNew && !(mMission.find("tutorial") != std::string::npos))
    mGUI->askForCompanyName();

  EventHandler::raiseEvent(eResetSpeedIndicator,
    new EventArg<int>(mGameTime->getSpeedInt()));
}

/*-----------------------------------------------------------------------------------------------*/

void Game::setupDemolishParticles(void)
{
  mDemolishPartNode = mSceneManager->getRootSceneNode()->
    createChildSceneNode("part_demolish");
  mDemolishPartSys = mSceneManager->createParticleSystem("demolish_sys",
    "DemolishPartSys");
  mDemolishPartNode->attachObject(mDemolishPartSys);
  mDemolishPartNode->setVisible(false);
}

/*-----------------------------------------------------------------------------------------------*/

void Game::unload(EventData* pData)
{
  EventHandler::raiseEvent(eGamestateChangeGUI);
  mGUI->setNoRunningGame();
  mGUI->switchToMainMenu();
}

/*-----------------------------------------------------------------------------------------------*/

void Game::dismantle(EventData* pData)
{
  mDismantle = true;
  mStartData = static_cast<EventArg<StartData>*> (pData)->mData1;
}

/*-----------------------------------------------------------------------------------------------*/

void Game::serialize(EventData* pData)
{
  std::string lFilename = static_cast<EventArg<std::string>*> (pData)->mData1;

  boost::shared_ptr<TiXmlDocument> lSaveGame(new TiXmlDocument());

  TiXmlDeclaration * lDeclaration = new TiXmlDeclaration("1.0", "", "");
  lSaveGame->LinkEndChild(lDeclaration);

  TiXmlElement * lSaveElement = new TiXmlElement("savegame");
  lSaveElement->SetAttribute("map", mMap->getName().c_str());
  lSaveElement->SetAttribute("mission", mMission.c_str());
  lSaveElement->SetAttribute("sandbox", mSandbox ? "yes" : "no");
  lSaveGame->LinkEndChild(lSaveElement);

  mGameTime->serializeIntoXMLElement(lSaveElement);
  mCompany->serializeIntoXMLElement(lSaveElement, mGUI);
#ifdef WIN32
  lSaveGame->SaveFile(cDataDirPre + "saved_games\\" + lFilename + ".xml");
#else
std::string lUserHomeDirectory(getenv("HOME"));
 std::string lSaveGameDirectory = lUserHomeDirectory + "/.energytycoon/";
  lSaveGame->SaveFile((lSaveGameDirectory + "saved_games/" + lFilename + ".xml")
    .c_str());
#endif
}

/*-----------------------------------------------------------------------------------------------*/

void Game::windowClosed(Ogre::RenderWindow* pRenderWindow)
{
  if (pRenderWindow == mWindow) {
    if (mInputManager) {
      mInputManager->destroyInputObject(mMouse);
      mInputManager->destroyInputObject(mKeyboard);

      OIS::InputManager::destroyInputSystem(mInputManager);
      mInputManager = 0;
    }

    this->~Game();
    exit(0);
  }
}

/*-----------------------------------------------------------------------------------------------*/

void Game::windowFocusChange(Ogre::RenderWindow* pRenderWindow)
{
  bool lActive = pRenderWindow->isActive();

  if (!lActive) {
    Audio::pause(true);
    mRun = false;
  } else {
    Audio::pause(false);
    mRun = true;
  }
}

/*-----------------------------------------------------------------------------------------------*/

bool Game::frameRenderingQueued(const Ogre::FrameEvent& pEvent)
{
  if (mWindow->isClosed())
    return false;

  return true;
}

/*-----------------------------------------------------------------------------------------------*/

bool Game::frameEnded(const Ogre::FrameEvent& pEvent)
{
  if (mDismantle) {
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    EventHandler::raiseEvent(eRestartGame, new EventArg<StartData > (mStartData));
    this->~Game();
  }

  if (!mRun)
#ifdef WIN32
    Sleep(50);
#else
    usleep(1000 * 50);
#endif

  return true;
}

/*-----------------------------------------------------------------------------------------------*/

bool Game::mouseMoved(const OIS::MouseEvent &pEvent)
{
  if (!pEvent.state.buttonDown(OIS::MB_Middle))
    mGUI->getMyGUI()->injectMouseMove(pEvent);

  if(cDemo && (mDemoTime > cDemoSeconds))
    return true;

  updateLastMouseData(pEvent);

  if (mBuildLock > 0)
    return true;

  if ((mGamestate == eSelectMode) || (mGamestate == eBuildMode)
    || (mGamestate == eDemolishMode)) {

    mGUI->updateTooltip();

    float lSelectorSize = 0.0f;
    boost::shared_ptr<Collidable> lColliding;

    handleMouseMovedMapView(pEvent);

    if (mGamestate == eBuildMode) {
      lSelectorSize = cStructureBorderLength[mBuildType];

      mBuildRequestNode->setVisible(true);
      mBuildRequestNode->setPosition(
        getQuadHighestTerrainIntersection(lSelectorSize)
        + Ogre::Vector3(0.0, cFloorDistance, 0.0));
    }
    else if ((mGamestate == eSelectMode) || (mGamestate == eDemolishMode)) {
      lSelectorSize = cDefaultColliderSize;
    }

    bool lWaterCheck = false;
    bool lRiverCheck = false;
    bool lPoleCheck = false;

    if ((mGamestate == eBuildMode) && (mBuildType == ePPWindOffshore))
      lWaterCheck = true;

    if ((mGamestate == eBuildMode) && (mBuildType == ePPWater))
      lRiverCheck = true;

    if ((mGamestate == eBuildMode) && ((mBuildType == eDIPoleSmall)
      || (mBuildType == eDIPoleLarge)))
      lPoleCheck = true;

    if (((mGamestate == eDemolishMode) || (mGamestate == eBuildMode))
      && (!mGUI->isMouseOverGUI())
      && (mMap->collides(rectFromPoint3(getMouseTerrainIntersection(),
      lSelectorSize), lColliding,
      false, lWaterCheck, lRiverCheck, lPoleCheck))) {
      if (mGamestate == eDemolishMode) {
        if (lColliding->isSelectable()) {
          if ((lColliding->getType() == ePowerplant)
            || (lColliding->getType() == eResource)) {
            boost::dynamic_pointer_cast<Highlightable > (lColliding)->highlight(true);
            mHighlighted = boost::dynamic_pointer_cast<Highlightable > (lColliding);
          }
        } else {
          if (lColliding->getType() == ePole) {
            boost::dynamic_pointer_cast<Highlightable > (lColliding)->highlight(true);
            mHighlighted = boost::dynamic_pointer_cast<Highlightable > (lColliding);
          }
        }
      }
      else if (mGamestate == eBuildMode) {
        if ((mBuildType == eDIPoleLarge) || (mBuildType == eDIPoleSmall))  {
          if (((lColliding->getType() == ePowerplant) // highlight avail. nodes
            || (lColliding->getType() == eCity)
            || (lColliding->getType() == eTransfer)
            || (lColliding->getType() == ePole))) {
            bool lHighlight = true;
            if (mPoleNodes.size() > 0) {
              if (lColliding == mPoleNodes[mPoleNodes.size() - 1])
                lHighlight = false;

              if ((mPoleNodes[mPoleNodes.size() - 1]->getPosition()
                - lColliding->getPosition()).length() > cMaxLineLength) {
                lHighlight = false;
              }
            }

            if (lHighlight) {
              MyGUI::PointerManager::getInstance().setPointer("netstart", 0);

              if (mHighlighted)
                mHighlighted->highlight(false);

              boost::dynamic_pointer_cast<Highlightable > (lColliding)
                ->highlight(true);
              mHighlighted = boost::dynamic_pointer_cast<Highlightable > (lColliding);

              dynamic_cast<Ogre::Entity*> (mBuildRequestNode
                ->getAttachedObject("build_request"))
                ->setVisible(false);

              mOverPoleConnectableNode = true;
              mOverPoleNode = lColliding;

              if (mPoleNodes.size() > 0)  {
                if (mPowerLineSuggestion)
                  mPowerLineSuggestion->removeFromSceneManager();

                mPowerLineSuggestion.reset(new PowerLine(
                  mPoleNodes[mPoleNodes.size() - 1],
                  mPoleNodes[mPoleNodes.size() - 1], mMap->getTerrain(), mMap, true,
                  Ogre::Vector2(mOverPoleNode->getPosition().x,
                  mOverPoleNode->getPosition().y)));

                mPowerLineSuggestion->addToSceneManager(mSceneManager);
                mBuildable = true;
              }
            } else {
              if (mPowerLineSuggestion) {
                mPowerLineSuggestion->resetSuggestionPosition(lColliding
                  ->getPosition());
                mPowerLineSuggestion->changeNoBuildColour();
              }

              dynamic_cast<Ogre::Entity*> (mBuildRequestNode
                ->getAttachedObject("build_request"))
                ->setMaterialName("collides");
              mBuildable = false;
            }
          } else {
            if (mPowerLineSuggestion) {
              mPowerLineSuggestion->resetSuggestionPosition(lColliding
                ->getPosition());
              mPowerLineSuggestion->changeNoBuildColour();
            }

            dynamic_cast<Ogre::Entity*> (mBuildRequestNode
              ->getAttachedObject("build_request"))
              ->setMaterialName("collides");
            mBuildable = false;
          }
        } else {
          dynamic_cast<Ogre::Entity*> (mBuildRequestNode
            ->getAttachedObject("build_request"))
            ->setMaterialName("collides");
          mBuildable = false;
        }
      }
    }
    else if ((mGamestate == eBuildMode) && ((mBuildType == eRECoalLarge)
      || (mBuildType == eRECoalSmall)
      || (mBuildType == eREGasSmall) || (mBuildType == eREGasLarge)
      || (mBuildType == eREUraniumSmall) || (mBuildType == eREUraniumLarge))
      && (!mGUI->isMouseOverGUI())) {
      if(mMap->collides(rectFromPoint3(getMouseTerrainIntersection(),
        lSelectorSize), lColliding, true)) {
        if (lColliding->getType() == eStatic) {
          boost::shared_ptr<StaticGameObject> lResourceCheck
            = boost::dynamic_pointer_cast<StaticGameObject > (lColliding);

          if ((lResourceCheck->getSubType() == eReCoal)
            && ((mBuildType == eRECoalLarge) || (mBuildType == eRECoalSmall)))  {
            dynamic_cast<Ogre::Entity*> (mBuildRequestNode->
              getAttachedObject("build_request"))->setVisible(true);
            dynamic_cast<Ogre::Entity*> (mBuildRequestNode->
              getAttachedObject("build_request"))->setMaterialName("fits");
            mBuildable = true;
          }
          else if ((lResourceCheck->getSubType() == eReGas)
            && ((mBuildType == eREGasLarge) || (mBuildType == eREGasSmall))) {
            dynamic_cast<Ogre::Entity*> (mBuildRequestNode->
              getAttachedObject("build_request"))->setVisible(true);
            dynamic_cast<Ogre::Entity*> (mBuildRequestNode->
              getAttachedObject("build_request"))->setMaterialName("fits");
            mBuildable = true;
          }
          else if ((lResourceCheck->getSubType() == eReUranium)
            && ((mBuildType == eREUraniumSmall) || (mBuildType == eREUraniumLarge))) {
            dynamic_cast<Ogre::Entity*> (mBuildRequestNode->
              getAttachedObject("build_request"))->setVisible(true);
            dynamic_cast<Ogre::Entity*> (mBuildRequestNode->
              getAttachedObject("build_request"))->setMaterialName("fits");
            mBuildable = true;
          } else {
            dynamic_cast<Ogre::Entity*> (mBuildRequestNode->
              getAttachedObject("build_request"))->setMaterialName("collides");
            mBuildable = false;
          }
        }
      } else {
        dynamic_cast<Ogre::Entity*> (mBuildRequestNode->
          getAttachedObject("build_request"))->setMaterialName("collides");
        mBuildable = false;
      }
    } else {
      mMouseOver.reset();

      if (mGamestate != eDemolishMode) {
        //MyGUI::PointerManager::getInstance().setDefaultPointer();
      } else  {
        if (mHighlighted)
          mHighlighted->highlight(false);
      }

      if (mGamestate == eBuildMode) {
        if (mHighlighted)
          mHighlighted->highlight(false);

        Ogre::Entity* lBuildEntity = dynamic_cast<Ogre::Entity*> (
          mBuildRequestNode->getAttachedObject("build_request"));

        mOverPoleConnectableNode = false;

        if ((mBuildType == eDIPoleLarge) || (mBuildType == eDIPoleSmall)) {
          if (mPoleNodes.size() > 0) {
            if (mPowerLineSuggestion)
              mPowerLineSuggestion->removeFromSceneManager();

            mPowerLineSuggestion.reset(new PowerLine(mPoleNodes[mPoleNodes.size() - 1],
              mPoleNodes[mPoleNodes.size() - 1], mMap->getTerrain(), mMap, true,
              Ogre::Vector2(lBuildEntity->getParentSceneNode()->getPosition().x,
              lBuildEntity->getParentSceneNode()->getPosition().z)));

            mPowerLineSuggestion->addToSceneManager(mSceneManager);
          }
        }

        lBuildEntity->setVisible(true);
        lBuildEntity->setMaterialName("fits");
        mBuildable = true;

        if ((mBuildType == eRECoalLarge) || (mBuildType == eRECoalSmall)
          || (mBuildType == eREGasSmall) || (mBuildType == eREGasLarge)
          || (mBuildType == eREUraniumSmall) || (mBuildType == eREUraniumLarge)) {
          lBuildEntity->setMaterialName("collides");
          mBuildable = false;
        }

        if (mPoleNodes.size() > 0) {
          if (((mPoleNodes[mPoleNodes.size() - 1]->getPosition() -
            Ogre::Vector2(lBuildEntity->getParentSceneNode()->getPosition().x,
            lBuildEntity->getParentSceneNode()->getPosition().z)).length()
            > cMaxLineLength)
            || (mCompany->getPowerNet()->lineColliding(mPowerLineSuggestion))
            || (intersectsBuildRequestLine())) {
            if (mPowerLineSuggestion)
              mPowerLineSuggestion->changeNoBuildColour();

            lBuildEntity->setMaterialName("collides");
            mBuildable = false;
          }
        }
      }
    }
  }

  return true;
}

/*-----------------------------------------------------------------------------------------------*/

bool Game::mousePressed(const OIS::MouseEvent &pEvent, OIS::MouseButtonID pID)
{
  mGUI->getMyGUI()->injectMousePress(pEvent, pID);
  updateLastMouseData(pEvent);

  if(cDemo && (mDemoTime > cDemoSeconds))
    return true;

  if (mBuildLock > 0)
    return true;

  if (mGamestate != eGUIExclusiveMode) {
    mGUI->setPanIcon(false, pEvent.state.X.abs, pEvent.state.Y.abs, false, true);

    if ((pID == OIS::MB_Right) || (pID == OIS::MB_Middle)) {
      if (mPowerLineSuggestion)
        mPowerLineSuggestion->removeFromSceneManager();

      if (mHighlighted)
        mHighlighted->highlight(false);

      //mCompany->getPowerNet()->addBuildNodes(mPoleNodes, mBuildLines);

      mPoleNodes.clear();
      mBuildLines.clear();
      destroyBuildEntity();

      if (pID == OIS::MB_Right)
        mRightButtonDown = true;

      mGamestate = eSelectMode;
    }
  }

  if ((mGamestate == eSelectMode) || (mGamestate == eDemolishMode))  {
    boost::shared_ptr<Collidable> lColliding;

    if (mMap->collides(rectFromPoint3(getMouseTerrainIntersection(), 6.0), lColliding)) {
      if (mBuildLock < 0)  {
        if ((lColliding->isSelectable() || (lColliding->getType() == ePole))
          && !mGUI->isMouseOverGUI()
          && (pID == OIS::MB_Left)) {
          if ((mGamestate == eSelectMode) && (lColliding->getType() != ePole)) {
            mMap->showSelector(true, cSelectorScale[lColliding->getSubtype()]);
            mMap->moveSelector(lColliding->getPosition());
            mGUI->showInfo(lColliding);
          }
          else if (mGamestate == eDemolishMode) {
            demolishCompanyObject(lColliding);
            mMap->repaintMinimap(eDefault);
            mMap->showSelector(false);
            EventHandler::raiseEvent(eUpdateMinimap);
          }
        }
        else if (!mGUI->isMouseOverGUI() && (pID == OIS::MB_Left)) {
          mGUI->unshowInfo();
          mMap->showSelector(false);
        }
      }
    }
    else if ((!mGUI->isMouseOverGUI())
      && (pID != OIS::MB_Middle)
      && (pID != OIS::MB_Right)) {
      mGUI->unshowInfo();
      mMap->showSelector(false);
    }

    if (pID == OIS::MB_Right)
      mGamestate = eSelectMode;
  }
  else if ((mGamestate == eBuildMode)
    && (pID == OIS::MB_Left) && !mGUI->isMouseOverGUI()) {
    if ((mBuildType == eDIPoleLarge) || (mBuildType == eDIPoleSmall)) {
      if (mBuildable) {
        if (mCompany->buyObject(mBuildType, mOverPoleConnectableNode)) {
          if (mOverPoleConnectableNode) {
            std::vector<boost::shared_ptr<Collidable> > lTempNodes;
            std::vector<boost::shared_ptr<PowerLine> > lTempLines;

            lTempNodes.push_back(mOverPoleNode);

            mCompany->getPowerNet()->addBuildNodes(lTempNodes, lTempLines);

            mPoleNodes.push_back(mOverPoleNode);
          } else { // build pole
            boost::shared_ptr<Pole> lPole(new Pole(Ogre::Vector2(
              getMouseTerrainIntersection().x,
              getMouseTerrainIntersection().z), mBuildType));
            lPole->addToSceneManager(mSceneManager, mMap->getTerrain());
            mPoleNodes.push_back(lPole);

            std::vector<boost::shared_ptr<Collidable> > lTempNodes;
            std::vector<boost::shared_ptr<PowerLine> > lTempLines;

            lTempNodes.push_back(lPole);

            mCompany->getPowerNet()->addBuildNodes(lTempNodes, lTempLines);
            mMap->addCollidable(lPole, true);

            mGUI->popMessage("#FF0000-" + toString(GameConfig::getInt(
              GameConfig::cCOTS(mBuildType), cCOVCost) / 1000) + "kEUR", 1.3,
              mLastMousePos.x, mLastMousePos.y - 25);
          }

          if (mPoleNodes.size() > 1) {
            std::vector<boost::shared_ptr<Collidable> > lTempNodes;
            std::vector<boost::shared_ptr<PowerLine> > lTempLines;

            boost::shared_ptr<PowerLine> lTemp(new PowerLine(
              mPoleNodes[mPoleNodes.size() - 2],
              mPoleNodes[mPoleNodes.size() - 1], mMap->getTerrain(), mMap));
            lTemp->addToSceneManager(mSceneManager);
            mBuildLines.push_back(lTemp);

            lTempNodes.push_back(mPoleNodes[mPoleNodes.size() - 2]);
            lTempNodes.push_back(mPoleNodes[mPoleNodes.size() - 1]);

            lTempLines.push_back(mBuildLines[mBuildLines.size() - 1]);

            bool lRemoveLine = mCompany->getPowerNet()->
              addBuildNodes(lTempNodes, lTempLines);

            if (lRemoveLine)
              lTemp->removeFromSceneManager();
          }
        } else {
          EventHandler::raiseEvent(eMessageBoxOK,
            new EventArg<std::string > (StrLoc::get()->Building(),
            StrLoc::get()->NoMoney()));

          if (mPowerLineSuggestion)
            mPowerLineSuggestion->removeFromSceneManager();

          mCompany->getPowerNet()->addBuildNodes(mPoleNodes, mBuildLines);

          mPoleNodes.clear();
          mBuildLines.clear();
          destroyBuildEntity();

          MyGUI::PointerManager::getInstance().setDefaultPointer();

          mGamestate = eSelectMode;
        }
      }
    } else {
      if (mBuildable == true) {
        buildCompanyObject();
        destroyBuildEntity();

        mBuildLock = 5; // lock collidable checks for 5 frames

        mGamestate = eSelectMode;
      } else {
        TickerMessage lMessage;

        if (mBuildType == ePPWindOffshore)
          lMessage.mMessage = StrLoc::get()->NoBuildOffshore();
        else if ((mBuildType == eRECoalLarge) || (mBuildType == eRECoalSmall))
          lMessage.mMessage = StrLoc::get()->NoBuildCoal();
        else if ((mBuildType == eREGasLarge) || (mBuildType == eREGasSmall))
          lMessage.mMessage = StrLoc::get()->NoBuildGas();
        else if ((mBuildType == eREUraniumLarge) || (mBuildType == eREUraniumSmall))
          lMessage.mMessage = StrLoc::get()->NoBuildUranium();
        else
          lMessage.mMessage = StrLoc::get()->NoBuild();


        lMessage.mUrgent = false;
        lMessage.mPointOfInterest = Ogre::Vector2(mBuildRequestNode->
          getPosition().x, mBuildRequestNode->getPosition().z);

        EventHandler::raiseEvent(eTickerMessage,
          new EventArg<TickerMessage > (lMessage));
      }
    }
  }

  return true;
}

/*-----------------------------------------------------------------------------------------------*/

void Game::destroyBuildEntity(void)
{
  if (mBuildRequestNode->numAttachedObjects() > 0) {
    Ogre::Entity* lTempEntity = dynamic_cast<Ogre::Entity*> (mBuildRequestNode->
      getAttachedObject("build_request"));

    mBuildRequestNode->detachAllObjects();
    mBuildRequestNode->getCreator()->destroyEntity(lTempEntity);
  }
}

/*-----------------------------------------------------------------------------------------------*/

void Game::runDemolishPart(float pElapsedTime)
{
  if (mRunDemolishPart)
  {
    mDemolishPartSys->getEmitter(0)->setEmissionRate(150);
    mDemolishPartSys->fastForward(1.0);
    mDemolishPartNode->setVisible(true);
    mDemolishPartSys->getEmitter(0)->setEmissionRate(0);

    mRunDemolishPart = false;
  }
}

/*-----------------------------------------------------------------------------------------------*/

void Game::smoothOutPan(float pElapsedTime)
{
  mLastPanDelta *= (1 - (pElapsedTime * 10));

  if (mLastPanDelta.length() < 0.01)
    mSmoothOutPan = false;

  if (mLastPanDelta.x > 0)
    mPanCamera->moveLeft(mLastPanDelta.x / 9000.0f * pElapsedTime * 100);
  else
    mPanCamera->moveRight(-mLastPanDelta.x / 9000.0f * pElapsedTime * 100);

  if (mLastPanDelta.y > 0)
    mPanCamera->moveUp(mLastPanDelta.y / 9000.0f * pElapsedTime * 100);
  else
    mPanCamera->moveDown(-mLastPanDelta.y / 9000.0f * pElapsedTime * 100);
}

/*-----------------------------------------------------------------------------------------------*/

bool Game::mouseReleased(const OIS::MouseEvent &pEvent, OIS::MouseButtonID pID)
{
  if(cDemo && (mDemoTime > cDemoSeconds))
  {}
  else
  {
    if (mGamestate != eGUIExclusiveMode) {
      MyGUI::PointerManager::getInstance().setVisible(true);
      MyGUI::PointerManager::getInstance().setDefaultPointer();

      if ((pID == OIS::MB_Right) || (pID == OIS::MB_Middle))  {
        if (pID == OIS::MB_Middle) {
          OIS::MouseState &lMutableMouseState
            = const_cast<OIS::MouseState &> (mMouse->getMouseState());
          lMutableMouseState.X.abs = mGUI->getLastPanPosition().x;
          lMutableMouseState.Y.abs = mGUI->getLastPanPosition().y;
        }

        if (pID == OIS::MB_Right) {
          mRightButtonDown = false;
          mLastPanDelta = mGUI->getLastPanPosition() - mLastMousePos;
          mSmoothOutPan = true;
        }

        mGUI->setPanIcon(false, 0, 0, false);
      }
    }
  }

  mGUI->getMyGUI()->injectMouseRelease(pEvent, pID);
  updateLastMouseData(pEvent);

  return true;
}

/*-----------------------------------------------------------------------------------------------*/

bool Game::keyPressed(const OIS::KeyEvent &pEvent)
{
  mGUI->getMyGUI()->injectKeyPress(pEvent);
  return true;
}

/*-----------------------------------------------------------------------------------------------*/

bool Game::keyReleased(const OIS::KeyEvent &pEvent)
{
  if (mGamestate != eGUIExclusiveMode) {
    if (pEvent.key == OIS::KC_B) {
      std::string lFilename = mWindow->writeContentsToTimestampedFile("screen_", ".jpg");
      mCompany->dump("dump_" + lFilename.substr(0, lFilename.size() - 4) + ".txt");

      TickerMessage lMessage;
      lMessage.mMessage = "Screenshot " + lFilename + StrLoc::get()->ScreenSaved();

      EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage > (lMessage));
    }
    else if (pEvent.key == OIS::KC_Q) {
      if(!cDemo) {
        TickerMessage lMessage;
        lMessage.mMessage = StrLoc::get()->GameSaved();

        EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage > (lMessage));
        EventHandler::raiseEvent(eSerializeGame, new EventArg<std::string > ("quicksave"));
      }
    }
  }

  mGUI->getMyGUI()->injectKeyRelease(pEvent);
  return true;
}

/*-----------------------------------------------------------------------------------------------*/

bool Game::frameStarted(const Ogre::FrameEvent &pEvent)
{
  mGUI->delaySwitchToMainMenu();

  mLastFrameTime = pEvent.timeSinceLastFrame;

  if (mBuildLock >= 0) // prevent ogre png loader crash
    --mBuildLock;

  if (!mRun)
    mLastFrameTime = 0;

  mGUI->getMyGUI()->injectFrameEntered(mLastFrameTime);

  captureInput();

  if ((mGamestate == eSelectMode) || (mGamestate == eBuildMode)
    || (mGamestate == eDemolishMode)) {

    if(cDemo){
      mDemoTime += mLastFrameTime;
      mGUI->updateDemoTime(mDemoTime);
    }

    if(cDemo && (mDemoTime > cDemoSeconds)) {
      if(!mDemoEventRaised) {
        mDemoEventRaised = true;
        MyGUI::PointerManager::getInstance().setVisible(true);
        MyGUI::PointerManager::getInstance().setDefaultPointer();
        mGUI->showDemoTimeUp(0);
      }
    }

    updateHoverText(mLastFrameTime);
    mGUI->updateGameTime(mGameTime->getTime(), mGameTime->getSpeed());
    mGUI->updateMinimapWindow(getViewTerrainIntersetction().x,
      getViewTerrainIntersetction().y,
      mPanCamera->getHeight(), mMap->getWidth(), mMap->getHeight());

    mGUI->updateButtonsDown(mLastFrameTime);
    mGUI->updateWeather(mLastFrameTime);
    mGUI->updatePopMessages(mLastFrameTime);
    mCompany->updateAnimations(mLastFrameTime);
    mGameTime->update(mLastFrameTime);
    mMap->updateAirplanes(mLastFrameTime);
    mMap->updateCars(mLastFrameTime);
    mMap->updateShips(mLastFrameTime);

    handleMapViewKeys(pEvent);
    runDemolishPart(mLastFrameTime);

    if (mRightButtonDown)  {
      mGUI->setPanIcon(true, mLastMousePos.x, mLastMousePos.y, false);

      float lX = mGUI->getLastPanPosition().x - mLastMousePos.x;
      float lY = mGUI->getLastPanPosition().y - mLastMousePos.y;


      if (lX > 0)
        mPanCamera->moveLeft(lX / 9000.0f * mLastFrameTime * 100);
      else
        mPanCamera->moveRight(-lX / 9000.0f * mLastFrameTime * 100);

      if (lY > 0)
        mPanCamera->moveUp(lY / 9000.0f * mLastFrameTime * 100);
      else
        mPanCamera->moveDown(-lY / 9000.0f * mLastFrameTime * 100);
    }

    if (mSmoothOutPan)
      smoothOutPan(mLastFrameTime);

    mPanCamera->update(mLastFrameTime);
    GameTimer::updateTimers(mLastFrameTime);
  }

  return true;
}

/*-----------------------------------------------------------------------------------------------*/

void Game::handleMouseMovedMapView(const OIS::MouseEvent &pEvent)
{
  MyGUI::PointerManager::getInstance().setVisible(true);
  MyGUI::PointerManager::getInstance().setDefaultPointer();

  if (mGamestate == eDemolishMode)
    MyGUI::PointerManager::getInstance().setPointer("demolish", 0);

  if (pEvent.state.buttonDown(OIS::MB_Middle)) {
    mGUI->setPanIcon(true, pEvent.state.X.abs, pEvent.state.Y.abs, true);
    MyGUI::PointerManager::getInstance().setVisible(false);
    mPanCamera->rotateRight((float) pEvent.state.X.rel / cMouseRotationSteps / 2.0);
  }

  if (!pEvent.state.buttonDown(OIS::MB_Right) && !pEvent.state.buttonDown(
    OIS::MB_Middle)) {
    mGUI->setPanIcon(false, 0, 0, false);
  }

  if (pEvent.state.Z.rel != 0)
    if (!mGUI->isMouseOverGUI())
      mPanCamera->addZoom((float) mMouse->getMouseState().Z.rel / cMouseWheelZoomSteps);
}

/*-----------------------------------------------------------------------------------------------*/

void Game::handleMapViewKeys(const Ogre::FrameEvent &pEvent)
{
  if(cDemo && (mDemoTime > cDemoSeconds))
    return;

  if (!mWindow->isClosed()) {
    if (mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
      if (mBuildRequestNode->isInSceneGraph())
        mBuildRequestNode->setVisible(false);
      mGamestate = eGUIExclusiveMode;
      mGUI->switchToMainMenu();
    }

    if (mKeyboard->isKeyDown(OIS::KC_UP) ||
      (mMouse->getMouseState().Y.abs < 1)) {
      if (!mMouse->getMouseState().buttonDown(OIS::MB_Middle)) {
        mPanCamera->moveUp(pEvent.timeSinceLastFrame);
      }
    }

    if (mKeyboard->isKeyDown(OIS::KC_DOWN) ||
      (mMouse->getMouseState().Y.abs > (int) mWindow->getHeight() - 1)) {
      if (!mMouse->getMouseState().buttonDown(OIS::MB_Middle)) {
        mPanCamera->moveDown(pEvent.timeSinceLastFrame);
      }
    }

    if (mKeyboard->isKeyDown(OIS::KC_LEFT) ||
      (mMouse->getMouseState().X.abs < 1)) {
      if (!mMouse->getMouseState().buttonDown(OIS::MB_Middle)) {
        mPanCamera->moveLeft(pEvent.timeSinceLastFrame);
      }
    }

    if (mKeyboard->isKeyDown(OIS::KC_RIGHT) ||
      (mMouse->getMouseState().X.abs > (int) mWindow->getWidth() - 1)) {
      if (!mMouse->getMouseState().buttonDown(OIS::MB_Middle)) {
        mPanCamera->moveRight(pEvent.timeSinceLastFrame);
      }
    }

    if (mKeyboard->isKeyDown(OIS::KC_W))
      mPanCamera->zoomIn(pEvent.timeSinceLastFrame);

    if (mKeyboard->isKeyDown(OIS::KC_S))
      mPanCamera->zoomOut(pEvent.timeSinceLastFrame);

    if (mKeyboard->isKeyDown(OIS::KC_A))
      mPanCamera->rotateLeft(pEvent.timeSinceLastFrame);

    if (mKeyboard->isKeyDown(OIS::KC_D))
      mPanCamera->rotateRight(pEvent.timeSinceLastFrame);
  }
}

/*-----------------------------------------------------------------------------------------------*/

void Game::buildCompanyObject(void)
{
  if (mCompany->buyObject(mBuildType, false))  {
    if ((mBuildType == ePPNuclearSmall) || (mBuildType == ePPNuclearLarge)
      || (mBuildType == ePPWindSmall) || (mBuildType == ePPWindLarge)
      || (mBuildType == ePPGasSmall) || (mBuildType == ePPGasLarge)
      || (mBuildType == ePPWindOffshore) || (mBuildType == ePPWater)
      || (mBuildType == ePPBio) || (mBuildType == ePPSolarSmall)
      || (mBuildType == ePPSolarLarge) || (mBuildType == ePPSolarUpdraft)
      || (mBuildType == ePPNuclearFusion) || (mBuildType == ePPCoalLarge)
      || (mBuildType == ePPCoalSmall))  {
      int lYear = 0;
      int lTemp1 = 0;
      int lTemp2 = 0;

      mGameTime->getTime(lYear, lTemp1, lTemp2);

      boost::shared_ptr<Powerplant> lTemp(new Powerplant(Ogre::Vector2(
        getQuadHighestTerrainIntersection(cStructureBorderLength[mBuildType]).x,
        getQuadHighestTerrainIntersection(cStructureBorderLength[mBuildType]).z),
        mBuildType,
        mMap->findNearestCity(Ogre::Vector2(getMouseTerrainIntersection().x,
        getMouseTerrainIntersection().z)), mCompany, lYear));
      lTemp->addToSceneManager(mSceneManager, mMap->getTerrain());

      std::vector<boost::shared_ptr<Collidable> > lTempNodes;
      std::vector<boost::shared_ptr<PowerLine> > lTempLines;

      lTempNodes.push_back(lTemp);

      mCompany->getPowerNet()->addBuildNodes(lTempNodes, lTempLines);
      mMap->addCollidable(lTemp, true);
    }
    else if ((mBuildType == eREUraniumLarge) || (mBuildType == eREUraniumSmall)
      || (mBuildType == eRECoalLarge) || (mBuildType == eRECoalSmall)
      || (mBuildType == eREGasLarge) || (mBuildType == eREGasSmall)) {
      boost::shared_ptr<ResourceBuilding> lTemp;

      if ((mBuildType == eREUraniumLarge) || (mBuildType == eREUraniumSmall))  {
        mUraniumCounter++;
        lTemp.reset(new ResourceBuilding(Ogre::Vector2(
          getQuadHighestTerrainIntersection(cStructureBorderLength[mBuildType]).x,
          getQuadHighestTerrainIntersection(cStructureBorderLength[mBuildType]).z),
          mBuildType, mUraniumCounter, mCompany));
      }
      else if ((mBuildType == eRECoalLarge) || (mBuildType == eRECoalSmall)) {
        mCoalCounter++;
        lTemp.reset(new ResourceBuilding(Ogre::Vector2(
          getQuadHighestTerrainIntersection(cStructureBorderLength[mBuildType]).x,
          getQuadHighestTerrainIntersection(cStructureBorderLength[mBuildType]).z),
          mBuildType, mCoalCounter, mCompany));
      }
      else if ((mBuildType == eREGasLarge) || (mBuildType == eREGasSmall)) {
        mGasCounter++;
        lTemp.reset(new ResourceBuilding(Ogre::Vector2(
          getQuadHighestTerrainIntersection(cStructureBorderLength[mBuildType]).x,
          getQuadHighestTerrainIntersection(cStructureBorderLength[mBuildType]).z),
          mBuildType, mGasCounter, mCompany));
      }

      lTemp->addToSceneManager(mSceneManager, mMap->getTerrain());

      mCompany->addResourceBuilding(lTemp);
      mMap->addCollidable(lTemp, true);
    }
    else if ((mBuildType == eCOHeadquarters)
      || (mBuildType == eCOResearch)
      || (mBuildType == eCOPublicRelations))  {
      boost::shared_ptr<CompanyBuilding> lTemp(new CompanyBuilding(Ogre::Vector2(
        getQuadHighestTerrainIntersection(cStructureBorderLength[mBuildType]).x,
        getQuadHighestTerrainIntersection(cStructureBorderLength[mBuildType]).z),
        mBuildType));
      lTemp->addToSceneManager(mSceneManager, mMap->getTerrain());

      mCompany->addCompanyBuilding(lTemp);
      mMap->addCollidable(lTemp, true);
    }

    mGUI->popMessage("#FF0000-" + toString(GameConfig::getInt(
      GameConfig::cCOTS(mBuildType), cCOVCost) / 1000) + "kEUR", 1.3,
      mLastMousePos.x, mLastMousePos.y - 25);
  } else {
    EventHandler::raiseEvent(
      eMessageBoxOK, new EventArg<std::string > (StrLoc::get()->Building(),
      StrLoc::get()->NoMoney()));
  }
}

/*-----------------------------------------------------------------------------------------------*/

void Game::demolishCompanyObject(boost::shared_ptr<Collidable> pObject)
{
  mRunDemolishPart = true;

  if ((pObject->getType() == ePowerplant) || (pObject->getType() == ePole)
    || (pObject->getType() == eResource)) {
    Ogre::Vector3 lStructurePos = Ogre::Vector3(pObject->getPosition().x,
      mMap->getTerrain()->getHighestFromQuad(
      pObject->getPosition().x, pObject->getPosition().y,
      cStructureBorderLength[pObject->getType()]), pObject->getPosition().y);

    mDemolishPartNode->setPosition(lStructurePos);

    Ogre::AxisAlignedBox lPartBox = Ogre::AxisAlignedBox(
      Ogre::Vector3(-1.0 * cStructureBorderLength[pObject->getType()] / 16.0, 0,
      -1.0 * cStructureBorderLength[pObject->getType()] / 16.0),
      Ogre::Vector3(cStructureBorderLength[pObject->getType()] / 16.0, 3,
      +cStructureBorderLength[pObject->getType()] / 16.0));


    mDemolishPartSys->setBoundsAutoUpdated(false);
    mDemolishPartSys->setBounds(lPartBox);
		
    if (pObject->getType() == ePowerplant) {
      mDemolishPartNode->setScale(0.26, 0.26, 0.26);

      boost::dynamic_pointer_cast<Powerplant > (pObject)->removeFromSceneManager();
      mMap->removeCollidable(pObject->getID());
      mCompany->getPowerNet()->removeNode(pObject);
      eCompanyObjectType pSubtype 
        = boost::dynamic_pointer_cast<Powerplant > (pObject)->getSubtype();
      mCompany->reimburse(GameConfig::getInt(GameConfig::cCOTS(pSubtype), cCOVCost) / 2);
      mGUI->popMessage("#00FF00+" 
        + toString(GameConfig::getInt(GameConfig::cCOTS(mBuildType), cCOVCost) / 1000 / 2) 
        + "kEUR", 1.3, mLastMousePos.x, mLastMousePos.y - 25);
    }
    else if (pObject->getType() == ePole) {
      mDemolishPartNode->setScale(0.1, 0.1, 0.1);

      boost::dynamic_pointer_cast<Pole > (pObject)->removeFromSceneManager();
      mMap->removeCollidable(pObject->getID());
      mCompany->getPowerNet()->removeNode(pObject);
      eCompanyObjectType pSubtype = boost::dynamic_pointer_cast<Pole > (pObject)->getSubtype();
      mCompany->reimburse(GameConfig::getInt(GameConfig::cCOTS(pSubtype), cCOVCost) / 2);
      mGUI->popMessage("#00FF00+" 
        + toString(GameConfig::getInt(GameConfig::cCOTS(mBuildType), cCOVCost) / 1000 / 2) 
        + "kEUR", 1.3, mLastMousePos.x, mLastMousePos.y - 25);
    }
    else if (pObject->getType() == eResource) {
      mDemolishPartNode->setScale(0.26, 0.26, 0.26);

      boost::dynamic_pointer_cast<ResourceBuilding > (pObject)->removeFromSceneManager();
      mMap->removeCollidable(pObject->getID());
      mCompany->removeResourceBuilding(pObject);
      eCompanyObjectType pSubtype 
        = boost::dynamic_pointer_cast<ResourceBuilding > (pObject)->getSubtype();
      mCompany->reimburse(GameConfig::getInt(GameConfig::cCOTS(pSubtype), cCOVCost) / 2);
      mGUI->popMessage("#00FF00+" 
        + toString(GameConfig::getInt(GameConfig::cCOTS(mBuildType), cCOVCost) / 1000 / 2) 
        + "kEUR", 1.3, mLastMousePos.x, mLastMousePos.y - 25);
    }
  }
}

/*-----------------------------------------------------------------------------------------------*/

void Game::enableBuildMode(EventData* pData)
{
  destroyBuildEntity();

  mGamestate = eBuildMode;

  Ogre::Entity* lTempEntity = mSceneManager->createEntity("build_request",
    cMeshFilename[static_cast<EventArg<int>*> (pData)->mData1]);

  lTempEntity->setRenderQueueGroup(99);

  mBuildRequestNode->attachObject(lTempEntity);

  mBuildRequestNode->setScale(
    cStructureScale[static_cast<EventArg<int>*> (pData)->mData1]);

  mBuildType = (eCompanyObjectType)static_cast<EventArg<int>*> (pData)->mData1;

  mBuildRequestNode->setVisible(false);

  mGUI->unshowInfo();
  mMap->showSelector(false);
}

/*-----------------------------------------------------------------------------------------------*/

void Game::enableDemolishMode(EventData* pData)
{
  mGamestate = eDemolishMode;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector3 Game::getMouseTerrainIntersection(void)
{
  Ogre::Ray lMouseRay =
    mPanCamera->getCam()->getCameraToViewportRay(
    float(mLastMousePos.x) / float(mLastMouseSize.x),
    float(mLastMousePos.y) / float(mLastMouseSize.y));

  return mMap->getTerrain()->getIntersectionFromRay(lMouseRay);
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector3 Game::getQuadHighestTerrainIntersection(int pSideLength)
{
  Ogre::Ray lMouseRay =
    mPanCamera->getCam()->getCameraToViewportRay(
    float(mLastMousePos.x) / float(mLastMouseSize.x),
    float(mLastMousePos.y) / float(mLastMouseSize.y));

  return mMap->getTerrain()->getQuadIntersectionFromRay(lMouseRay, pSideLength);
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 Game::getViewTerrainIntersetction(void)
{
  Ogre::Ray lViewRay =
    mPanCamera->getCam()->getCameraToViewportRay(
    float(mLastMouseSize.x / 2) / float(mLastMouseSize.x),
    float(mLastMouseSize.y / 2) / float(mLastMouseSize.y));

  return Ogre::Vector2(mMap->getTerrain()->getIntersectionFromRay(lViewRay).x,
    mMap->getTerrain()->getIntersectionFromRay(lViewRay).z);
}

/*-----------------------------------------------------------------------------------------------*/

void Game::updateLastMouseData(const OIS::MouseEvent &pEvent)
{
  mLastMousePos.x = pEvent.state.X.abs;
  mLastMousePos.y = pEvent.state.Y.abs;

  mLastMouseSize.x = pEvent.state.width;
  mLastMouseSize.y = pEvent.state.height;
}

/*-----------------------------------------------------------------------------------------------*/

void Game::startInputSystem(void)
{
  OIS::ParamList lParameterList;

  size_t lWindowHandle = 0;
  std::ostringstream lWindowHandleString;

  mWindow->getCustomAttribute("WINDOW", &lWindowHandle);
  lWindowHandleString << lWindowHandle;

  lParameterList.insert(std::make_pair(std::string("WINDOW"), lWindowHandleString.str()));

  mInputManager = OIS::InputManager::createInputSystem(lParameterList);

  mKeyboard = static_cast<OIS::Keyboard*>
    (mInputManager->createInputObject(OIS::OISKeyboard, true));

  mMouse = static_cast<OIS::Mouse*>
    (mInputManager->createInputObject(OIS::OISMouse, true));

  mMouse->setEventCallback(this);
  mKeyboard->setEventCallback(this);

  OIS::MouseState &lMutableMouseState = const_cast<OIS::MouseState &> (
    mMouse->getMouseState());
  lMutableMouseState.X.abs = mWindow->getWidth() / 2;
  lMutableMouseState.Y.abs = mWindow->getHeight() / 2;
}

/*-----------------------------------------------------------------------------------------------*/

void Game::windowResized(Ogre::RenderWindow* pRenderWindow)
{
  size_t lWidth, lHeight, lDepth;
  int lLeft, lTop;

  pRenderWindow->getMetrics(lWidth, lHeight, lDepth, lLeft, lTop);
  const OIS::MouseState &lMouseState = mMouse->getMouseState();

  lMouseState.width = lWidth;
  lMouseState.height = lHeight;
}

/*-----------------------------------------------------------------------------------------------*/

void Game::captureInput(void)
{
  if (mMouse)  {
    if (!mWindow->isClosed())
      mMouse->capture();
  }
  if (mKeyboard) {
    if (!mWindow->isClosed())
      mKeyboard->capture();
  }
}

/*-----------------------------------------------------------------------------------------------*/

void Game::enableSelectMode(EventData* pData)
{
  if (mGamestate == eBuildMode)
    destroyBuildEntity();

  mGamestate = eSelectMode;
}

/*-----------------------------------------------------------------------------------------------*/

void Game::enableGUIMode(EventData* pData)
{
  if (mGamestate == eBuildMode)
    destroyBuildEntity();

  mGamestate = eGUIExclusiveMode;
}

/*-----------------------------------------------------------------------------------------------*/

bool Game::intersectsBuildRequestLine(void)
{
  for (size_t i = 0; i < mBuildLines.size(); i++) {
    if (intersectLines(mPowerLineSuggestion->getFrom(),
      mPowerLineSuggestion->getSuggestionPosition(),
      mBuildLines[i]->getFrom(), mBuildLines[i]->getTo())) {
      return true;
    }
  }

  return false;
}

/*-----------------------------------------------------------------------------------------------*/

Ogre::Vector2 Game::getScreenCoordInfoPanel(Ogre::MovableObject* pEntity)
{
  if (pEntity) {
    const Ogre::AxisAlignedBox& lBoundingBox = pEntity->getWorldBoundingBox(true);
    Ogre::Matrix4 lMatrix = mPanCamera->getCam()->getViewMatrix();

    Ogre::Vector3 lCenter = lBoundingBox.getCenter();
    Ogre::Vector3 lScreen;

    lScreen = lMatrix * lCenter;

    float x = lScreen.x / lScreen.z + 0.5;
    float y = lScreen.y / lScreen.z + 0.5;

    int mScreenWidth = mWindow->getWidth();
    int mScreenHeight = mWindow->getHeight();

    x = x * mScreenWidth;
    y = y * mScreenHeight;

    int lXPos = mScreenWidth - x;
    int lYPos = y - 60;

    if(mScreenWidth > 1280) {
      // ugly fix for correct positioning on widescreen formats
      if(((lXPos+128) < (mScreenWidth/2)) && (lXPos > -512)) {
        int lLeftRelX = (mScreenWidth/2) - (lXPos+128);
        float lTightenFactor = (float)lLeftRelX / ((float)mScreenWidth/2.0);
        lXPos += (lTightenFactor)*((float)mScreenWidth/5.0);
      }
      else if((lXPos+128) < (mScreenWidth + 1024))  {
        int lRightRelX = (lXPos+128) - (mScreenWidth/2);
        float lTightenFactor = (float)lRightRelX / ((float)mScreenWidth/2.0);
        lXPos -= (lTightenFactor)*((float)mScreenWidth/5.0);
      }
    }
    return Ogre::Vector2(lXPos, lYPos);
  } else {
    return Ogre::Vector2(0, 0);
  }
}

/*-----------------------------------------------------------------------------------------------*/

void Game::updateHoverText(float pElapsedTime)
{
  if (mBuildLock > 0)
    return;

  std::vector<InfoPanel> lPanelsToShow;
  std::vector<boost::shared_ptr<City> > lAllCities = mMap->getAllCities();

  for (size_t i = 0; i < lAllCities.size(); i++) {
    Ogre::Vector2 lScreenCoord = getScreenCoordInfoPanel(lAllCities[i]->getMainEntityObject());

    if ((lScreenCoord.x > -256) && (lScreenCoord.y > 0)
      && (lScreenCoord.x < mWindow->getWidth())
      && (lScreenCoord.y < mWindow->getHeight()))  {
      InfoPanel lNewPanel;
      lNewPanel.mType = eCity;
      lNewPanel.mName = lAllCities[i]->getName();
      lNewPanel.mPosition = lScreenCoord;

      if (lAllCities[i]->getCurrentSupply() > 0)
        lNewPanel.mConnected = true;
      else
        lNewPanel.mConnected = false;

      lPanelsToShow.push_back(lNewPanel);
    }
  }

  std::vector<boost::shared_ptr<Powerplant> > lAllPowerplants = mCompany->getAllPowerplants();

  for (size_t j = 0; j < lAllPowerplants.size(); j++) {
    Ogre::Vector2 lScreenCoord = getScreenCoordInfoPanel(
      lAllPowerplants[j]->getMainEntityObject());

    if ((lScreenCoord.x > -256) && (lScreenCoord.y > 0)
      && (lScreenCoord.x < mWindow->getWidth())
      && (lScreenCoord.y < mWindow->getHeight())) {
      InfoPanel lNewPanel;
      lNewPanel.mType = ePowerplant;
      lNewPanel.mName = lAllPowerplants[j]->getName();
      lNewPanel.mPosition = lScreenCoord;
      lNewPanel.mConnected = true;

      lPanelsToShow.push_back(lNewPanel);
    }
  }

  std::vector<boost::shared_ptr<ResourceBuilding> > lAllResourceBuildings
    = mCompany->getResourceBuildings();

  for (size_t a = 0; a < lAllResourceBuildings.size(); a++) {
    Ogre::Vector2 lScreenCoord = getScreenCoordInfoPanel(
      lAllResourceBuildings[a]->getMainEntityObject());

    if ((lScreenCoord.x > -256) && (lScreenCoord.y > 0)
      && (lScreenCoord.x < mWindow->getWidth())
      && (lScreenCoord.y < mWindow->getHeight()))  {
      InfoPanel lNewPanel;
      lNewPanel.mType = eResource;
      lNewPanel.mName = lAllResourceBuildings[a]->getName();
      lNewPanel.mPosition = lScreenCoord;
      lNewPanel.mConnected = true;

      lPanelsToShow.push_back(lNewPanel);
    }
  }

  std::vector<boost::shared_ptr<CompanyBuilding> > lAllCompanyBuildings
    = mCompany->getCompanyBuildings();

  for (size_t b = 0; b < lAllCompanyBuildings.size(); b++)  {
    Ogre::Vector2 lScreenCoord = getScreenCoordInfoPanel(
      lAllCompanyBuildings[b]->getMainEntityObject());

    if ((lScreenCoord.x > -256) && (lScreenCoord.y > 0)
      && (lScreenCoord.x < mWindow->getWidth())
      && (lScreenCoord.y < mWindow->getHeight())) {
      InfoPanel lNewPanel;
      lNewPanel.mType = eCompany;

      if (lAllCompanyBuildings[b]->getSubtype() == eCOHeadquarters)
        lNewPanel.mName = StrLoc::get()->TooltipHeadquarters();
      else if (lAllCompanyBuildings[b]->getSubtype() == eCOResearch)
        lNewPanel.mName = StrLoc::get()->TooltipResearchBuilding();
      else if (lAllCompanyBuildings[b]->getSubtype() == eCOPublicRelations)
        lNewPanel.mName = StrLoc::get()->TooltipPR();

      lNewPanel.mPosition = lScreenCoord;
      lNewPanel.mConnected = true;

      lPanelsToShow.push_back(lNewPanel);
    }
  }

  if (mGamestate == eBuildMode)  {
    if ((mBuildType == eRECoalLarge) || (mBuildType == eRECoalSmall)
      || (mBuildType == eREGasSmall) || (mBuildType == eREGasLarge)
      || (mBuildType == eREUraniumSmall) || (mBuildType == eREUraniumLarge)) {
      std::vector<boost::shared_ptr<StaticGameObject> > lAllResources = mMap->getResources();

      for (size_t k = 0; k < lAllResources.size(); k++)
        lAllResources[k]->setVisible(false);

      if ((mBuildType == eRECoalLarge) || (mBuildType == eRECoalSmall))  {
        for (size_t l = 0; l < lAllResources.size(); l++) {
          if (lAllResources[l]->getSubType() == eReCoal) {
            lAllResources[l]->setVisible(true);
            lAllResources[l]->rotate(pElapsedTime);
          }
        }
      }
      else if ((mBuildType == eREGasSmall) || (mBuildType == eREGasLarge))  {
        for (size_t l = 0; l < lAllResources.size(); l++) {
          if (lAllResources[l]->getSubType() == eReGas) {
            lAllResources[l]->setVisible(true);
            lAllResources[l]->rotate(pElapsedTime);
          }
        }
      }
      else if ((mBuildType == eREUraniumSmall) || (mBuildType == eREUraniumLarge)) {
        for (size_t l = 0; l < lAllResources.size(); l++) {
          if (lAllResources[l]->getSubType() == eReUranium) {
            lAllResources[l]->setVisible(true);
            lAllResources[l]->rotate(pElapsedTime);
          }
        }
      }
    }
  } else {
    std::vector<boost::shared_ptr<StaticGameObject> > lAllResources = mMap->getResources();

    for (size_t k = 0; k < lAllResources.size(); k++)
      lAllResources[k]->setVisible(false);
  }

  mGUI->updateInfoPanels(lPanelsToShow);
}
