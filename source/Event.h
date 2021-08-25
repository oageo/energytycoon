#ifndef EVENT_H
#define EVENT_H

/*-----------------------------------------------------------------------------------------------*/

//! Game events
enum eEventType
{
  eGameSpeedSlower, eGameSpeedFaster, eGameSpeedPause, eResetSpeedIndicator,
  eGamestateChangeBuild, eGamestateChangeView, eGamestateChangeGUI, eGamestateChangeDemolish,
  eGraphicOptionsChange,
  eShowMainMenu,
  eUpdateMinimap, eMinimapModeChange,
  eDayPassed, eWeekPassed, eMonthPassed, eYearPassed,
  eMoveCamera, eCameraZoom, eCameraRotate,
  eTickerMessage,
  eMessageBoxOK,
  eChangeResource, eResourceGone, eResourceGain,
  eUpdatePowerNet, eUpdatePowerNetInfo,
  eSerializeGame, eLoadGame, eRestartGame, eDismantleGame, eShutdownGame,
  eUpdateTradeWindow, eUpdateObligations, eUpdateFinanceWindow,
  eHideSelector,
  eUpdateFinanceChart, eUpdateLoans,
  eUpdateResearchableButtons, eUpdateCOBuildingButtons,
  eShowNewspaper,
  eSetLoadingProgress, eSetLoadingStatus,
  eBankruptcyWarning, eBankruptcy,
  eMissionDone, eUpdateMissionGoals,
  eUpdateSpecialActions, eUpdateResourceTrend, eUpdateGUICompanyData,
  eNewWeather,
  eDemoTimeUp
};

/*-----------------------------------------------------------------------------------------------*/

//! Event arg superclass
class EventData 
{
};

/*-----------------------------------------------------------------------------------------------*/

//! Holds the event data
template <typename T> class EventArg : public EventData 
{
public:

  EventArg(T pData1, T pData2, T pData3) 
    : mData1(pData1), mData2(pData2), mData3(pData3) {}
  EventArg(T pData1, T pData2) 
    : mData1(pData1), mData2(pData2) {}
  EventArg(T pData1) 
    : mData1(pData1) {}
  EventArg() { }

  T mData1;
  T mData2;
  T mData3;
};

/*-----------------------------------------------------------------------------------------------*/

//! Handles all the events
class EventHandler  {
public:

  //! Register a callback for a specific event
  static std::multimap<const eEventType, boost::function<void (EventData*)> >::iterator
    registerCallback(eEventType pType, boost::function<void (EventData*) > pFunction)
  {
    return mCallbacks.insert(std::pair<const eEventType,
      boost::function<void (EventData*)> >(pType, pFunction));
  }

  //! Remove callback
  static void removeCallback(std::multimap<const eEventType,
    boost::function<void (EventData*)> >::iterator pIt)
  {
    mCallbacks.erase(pIt);
  }

  //! Raise an event
  static void raiseEvent(eEventType pType, EventData* pData = 0)
  {
    for (std::multimap<const eEventType, boost::function<void (EventData*)> >::iterator it
      = mCallbacks.begin(); it != mCallbacks.end(); it++) {
      if (((*it).first == pType) && ((*it).first != eRestartGame))
        (*it).second(pData);
      // eRestartGame event handler invalidates the iterator -> break after calling
      else if (((*it).first == pType) && ((*it).first == eRestartGame))  {
        (*it).second(pData);
        break;
      }
    }
    // Cleanup event arguments
    delete pData;
    pData = 0;
  }

  //! Clear all callback assignments
  static void clear(void)
  {
    mCallbacks.clear();
  }

private:

  static std::multimap<const eEventType,
    boost::function<void (EventData*)> > mCallbacks; //!< All the callback assignments
};

//! Derive from this and add callbacks to mRegisteredCallbacks if callback assignments
//! should be deleted when the object goes out of scope
class EventReceiver
{
public:

  ~EventReceiver(void)
  {
    for (size_t i = 0; i < mRegisteredCallbacks.size(); i++)
      EventHandler::removeCallback(mRegisteredCallbacks[i]);
  }

protected:

  std::vector<std::multimap<const eEventType,
    boost::function<void (EventData*)> >::iterator> mRegisteredCallbacks; //!< Callbacks in this instance
};

//! Shortcut for registering callbacks
#define REGISTER_CALLBACK(x, y) EventHandler::registerCallback(x, std::bind1st(std::mem_fun(&y), this));

/*-----------------------------------------------------------------------------------------------*/

#endif // EVENT_H
