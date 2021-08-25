#ifndef COMPANY_H
#define COMPANY_H

/*-----------------------------------------------------------------------------------------------*/

#include "Shared.h"

/*-----------------------------------------------------------------------------------------------*/

class GUI;
class Map;
class Chart;
class CompanyBuilding;
class ResourceBuilding;
class GameObject;
class PowerNet;
class Powerplant;
class City;
class Collidable;
class EventData;

/*-----------------------------------------------------------------------------------------------*/

const long cDefaultStartMoney = 3000000; //!< Default value
const int cDefaultUraniumStart = 10000; //!< Default value
const int cDefaultCoalStart = 20000; //!< Default value
const int cDefaultGasStart = 20000; //!< Default value

/*-----------------------------------------------------------------------------------------------*/

//! Resource type
enum eResourceType { eCoal, eGas, eUranium, eNone };

//! Possible mission conditions
enum eMissionCondition { eFreePlay, eCustomerCount, eMoney};

//! Training options
enum eTraining { eNoTraining, eNormalTraining, eExtensiveTraining };

//! Support options
enum eSupport { eNoSupport, eNormalSupport, eExtensiveSupport };

//! Safety options
enum eSafety { eNoSafety, eNormalSafety, eExtensiveSafety };

//! Contract time options
enum eContract { eNoTime, e12, e24 };

//! Ad type
enum eAdType { eInternet, eNewspaper, eRadio, eTV };

//! Special action type
enum eSpecialAction { eBribe, eWar, ePrice, eEmployee, eTaxfraud, eWaste };

//! Image affector type
enum eImageAffectorType { ePermanent, eTemporary };

/*-----------------------------------------------------------------------------------------------*/

//! Get string for resource type
static std::string resourceString(eResourceType pType, bool pCap = false)
{
	if(pType == eCoal)
		return pCap ? StrLoc::get()->Coal() : StrLoc::get()->CoalS();
	else if(pType == eGas)
		return pCap ? StrLoc::get()->Gas() : StrLoc::get()->GasS();
	else if(pType == eUranium)
		return pCap ? StrLoc::get()->Uranium() : StrLoc::get()->UraniumS();
	else
		return "";
}

/*-----------------------------------------------------------------------------------------------*/

//! Get resource unit string
static std::string getResourceUnit(eResourceType pType)
{
	if(pType == eCoal)
		return StrLoc::get()->TooltipKTons();
	else if(pType == eGas)
		return StrLoc::get()->TooltipKCubicFeet();
	else if(pType == eUranium)
		return StrLoc::get()->TooltipRods();
	else
		return "";
}

/*-----------------------------------------------------------------------------------------------*/

//! Financial data for one month/year(for finances window)
class FinancialArchive {
public:

	FinancialArchive(void) : 
	  mLoanPayments(0), mTrainingSafety(0), mMaintenance(0), mAdvertising(0),
	  mOperation(0), mEnvironmental(0), mResearch(0), mBuilding(0),
	  mSpecialExpenses(0), mTrade(0), mCustomerIncome(0), mTotal(0)
	{}

    void updateTotal(void)
	{
		mTotal =  mTrainingSafety + mMaintenance + mAdvertising +
			mOperation + mEnvironmental + mResearch + mBuilding +
			mSpecialExpenses + mTrade + mCustomerIncome;
	}


	int mLoanPayments;
	int mTrainingSafety;
	int mMaintenance;
	int mAdvertising;
	int mOperation;
	int mEnvironmental;
	int mResearch;
	int mBuilding;
	int mSpecialExpenses;
	int mTrade;
	int mCustomerIncome;
	int mTotal;
};

/*-----------------------------------------------------------------------------------------------*/

//! A loan taken by the player
class Loan {
public:

	std::string formatted(void);


	int mAmountLeft;
	int mMonthlyRate;
	int mMonthLeft;
};

/*-----------------------------------------------------------------------------------------------*/

//! An ad campaign
class AdCampaign {
public:

	eAdType mType;
	int mWeeksLeft;
};

/*-----------------------------------------------------------------------------------------------*/

//! A special action
class SpecialAction {
public:

	eSpecialAction mType;
	int mTradesLeft;
	int mMonthLeft;
	bool mDone;
};

/*-----------------------------------------------------------------------------------------------*/

//! A trading offer
class TradingOffer {
public:

	//! Get formatted offer string
	std::string formatted(void);

	//! Get formatted running contract string
	std::string formattedCurrent(void);


	std::string mTradingPartner; //!< Name of the partner
	eResourceType mResource; //!< What resource
	int mAmount; //!< Resource amount
	bool mUnique; //!< Single transaction or several weeks?
	int mWeeks; //!< Duration
	bool mWorldMarket; //!< World market price or fixed price?
	int mDiscount; //!< Discount on world market price
	int mFixedPrice; //!< Fixed price for all weeks
	bool mPermanent; //!< Permanent contract?
};

/*-----------------------------------------------------------------------------------------------*/

//! A trading partner (defined in the mission XML file)
class TradingPartner {
public:
	Ogre::UTFString mName;
	int mPrice;
	bool mAvailable;
};

/*-----------------------------------------------------------------------------------------------*/

//! All researchable options
class ResearchSet {
public:

	//! Initalise
	ResearchSet();

	bool mResearched[21]; //!< Is it researched
	bool mStarted[21]; //!< Has the research been started
	int mPrice[21]; //!< Price of the research
	std::string mName[21];
	int mWeeksLeft[21]; //!< Weeks left (currently researching)
	int mWeeks[21]; //!< Weeks required
	int mPrerequesit[21]; //!< Needed before this in research tree
};

/*-----------------------------------------------------------------------------------------------*/

//! A Mission goal
class MissionGoal {
public:
	std::string mText;
	bool mDone;
	eMissionCondition mType;
	int mValueNeeded;
	int mValueCurrent;
};

/*-----------------------------------------------------------------------------------------------*/

//! Image affector
class ImageAffector {
public:
	ImageAffector(eImageAffectorType pType, double pInfluence, int pDuration, int pAssociatedID = 0)
		: mType(pType), mInfluence(pInfluence), mDaysLeft(pDuration), mAssociatedID(pAssociatedID)
	{}

	eImageAffectorType mType;
	bool mDone;
	double mInfluence;
	int mDaysLeft;
	int mAssociatedID;
};

/*-----------------------------------------------------------------------------------------------*/

//! Visible values on finances chart
class FinanceChartSet {
public:
	FinanceChartSet() :
	  mExpenses(true), mIncome(true), mFunds(true), mNetWorth(true)
	  {}

	  bool mExpenses;
	  bool mIncome;
	  bool mFunds;
	  bool mNetWorth;
};

/*-----------------------------------------------------------------------------------------------*/

//! Representing the players company
class Company {
public:

	//! Create it
	Company(std::string pMission, bool pSandbox, std::vector<boost::shared_ptr<City> > pCities,
    boost::shared_ptr<Map> pMap);

	//! Return money
	int getMoney(void);

	//! Return storage amount
	int getUranium(void) { return mUranium; }

	//! Return storage amount
	int getCoal(void) { return mCoal; }

	//! Return storage amount
	int getGas(void) { return mGas; }

	//! Return world market price
	int getUraniumPrice(void) { return mUraniumPrice; }

	//! Return world market price
	int getCoalPrice(void) { return mCoalPrice; }

	//! Return world market price
	int getGasPrice(void) { return mGasPrice; }

	//! Buy a structure or fail with ticker message
	bool buyObject(eCompanyObjectType pType, bool pFree);

	//! Return power net
	boost::shared_ptr<PowerNet> getPowerNet(void);

	//! Return all trading offers
	std::vector<TradingOffer> getTradingOffers(void) { return mTradingOffers; }

	//! Return all current obligations
	std::vector<TradingOffer> getTradingObligations(void) { return mExecutingOffers; }

	//! cancel obligation
	void cancelTradingObligation(int pIndex) 
  { mExecutingOffers.erase(mExecutingOffers.begin() + pIndex); }

	//! Add a trading offer
	bool acceptTradingOffer(std::string pName);
	
	//! Get all trading partners from current mission
	std::vector<TradingPartner> getTradingPartners(void) { return mTradingPartners; }

	//! Sign trade agreement
	bool buyTradingPartner(std::string pName);

	//! Get all mission goals
	std::vector<MissionGoal> getMissionGoals(void) { return mMissionGoals; }

	//! set money amount
	void setMoney(int pMoney) { mMoney = pMoney; }

	//! Get formatted mission status
	std::vector<std::string> getFormattedMissionStatus(void);

	//! Serialize
	void serializeIntoXMLElement(TiXmlElement* pParentElement, boost::shared_ptr<GUI> pGUI);
	
	//! Load
	void deserializeFromXMLElement(TiXmlNode* pParentNode, boost::shared_ptr<Map> pMap,
								   Ogre::SceneManager* pSceneManager, boost::shared_ptr<Company> pCompany,
								   boost::shared_ptr<GUI> pGUI);

	//! Add a resource gathering building
	void addResourceBuilding(boost::shared_ptr<ResourceBuilding> pBuilding);

	//! Add a company building
	void addCompanyBuilding(boost::shared_ptr<CompanyBuilding> pBuilding);

	//! Get all current research data
	std::string getCurrentResearch(int &oPercentageCompleted);

	//! Get all company buildings
	std::vector<boost::shared_ptr<CompanyBuilding> > getCompanyBuildings(void)
  { return mCompanyBuildings; }

	//! Get all resource buildings
	std::vector<boost::shared_ptr<ResourceBuilding> > getResourceBuildings(void) 
  { return mResourceBuildings; }

	//! Get financial data
	std::vector<FinancialArchive> getFinancialData(void);

	//! Get sales data
	int getImage(void) { return mGlobalImage; }

	//! Get sales data
	int getAdvertising(void) { return mGlobalAdvertising; }

	//! Get sales data
	int getMaintenance(void) { return mGlobalMaintenance; }

	//! Get sales data
	int getPrice(void) { return mGlobalPrice; }

	//! Set sales data
	void setPrice(int pNewPrice) { mGlobalPrice = pNewPrice; }

	//! Set sales data
	void setAdvertising(int pNewAdvertising) { mGlobalAdvertising = pNewAdvertising; }

	//! Set sales data
	void setMaintenance(int pNewMaintenance) { mGlobalMaintenance = pNewMaintenance; }

	//! Get customer count
	int getCustomers(void) { return mCompleteCustomers; }

	//! Change resource values
	void adjustResource(eResourceType pType, int pAmount);

	//! Get all cities connected with the players powerplants
	std::vector<boost::shared_ptr<City> > getConnectedCities(bool pForceUpdate);

	//! Get company name
	std::string getName(void) { return mName; }

	//! Get formatted customer opinions
	std::vector<std::string> getCustomerOpinions(bool pAllCities = true, int pCityIndex = 0);

	//! Get formatted customer opinions from single city
	std::vector<std::string> getCityOpinions(int pCityIndex);

	//! Update all animations
	void updateAnimations(float pElapsedTime);

	//! Take out loan or fail with message box
	bool takeOutLoan(Loan pLoan);

	//! Get all available loans
	std::vector<Loan> getLoans(void);

	//! Make an unscheduled repayment
	bool unscheduledRepayment(int pLoanIndex, int pAmount);

	//! Get all powerplants in the players power-net
	std::vector<boost::shared_ptr<Powerplant> > getAllPowerplants(void);

	//! Get setting
	eTraining getTrainingLevel(void) { return mTraining; }

	//! Get setting
	eSupport getSupportLevel(void) { return mCustomerSupport; }

	//! Get setting
	eSafety getSafetyLevel(void) { return mSafety; }

	//! Get setting
	eContract getContractTime(void) { return mContractTime; }

	//! Global maintenance setting
	size_t getSupplyMaintenance(void) { return mSupplyMaintenance; }

	//! Global env support setting
	size_t getEnvSupport(void) { return mEnvSupport; }

	//! Set finance window settings
	void setTrainingLevel(eTraining pTraining) { mTraining = pTraining; }

	//! Set finance window settings
	void setSupportLevel(eSupport pCustomerSupport) { mCustomerSupport = pCustomerSupport; }

	//! Set finance window settings
	void setSafetyLevel(eSafety pSafety) { mSafety = pSafety; }

	//! Set finance window settings
	void setContractTime(eContract pContractTime) { mContractTime = pContractTime; }

	//! Set finance window settings
	void setSupplyMaintenance(size_t pSupplyMaintenance) 
  { mSupplyMaintenance = pSupplyMaintenance; }

	//! Set finance window settings
	void setEnvSupport(size_t pEnvSupport) { mEnvSupport = pEnvSupport; }

	//! Check if company buildings have been built
	bool isResearchBuilt(void) { return mResearchBuilt; }

	//! Check if company buildings have been built
	bool isHeadquartersBuilt(void) { return mHeadquartersBuilt; }

	//! Check if company buildings have been built
	bool isPRBuilt(void) { return mPRBuilt; }

	//! Start an ad campaign
	bool runAdCampaign(eAdType pType);

	//! Run campaigns
	void handleAdCampaigns(void);

	//! Start research project
	bool startResearch(int pIndex);

	//! Run research projects
	void updateResearch(void);

	//! Return research set
	ResearchSet getResearchSet(void) { return mResearchSet; }

	//! Start special action
	bool runSpecialAction(eSpecialAction pType);

	//! Check if special action is already executing
	bool isSpecialActionRunning(eSpecialAction pType);

	//! how many trades from special action left
	int getTaxFreeTradesLeft(void);

	//! Run special actions
	void handleSpecialActions(void);

	//! Demolish a resource building
	void removeResourceBuilding(boost::shared_ptr<Collidable> pObject);

	//! Return resource stock trends
	std::vector<int> getResourceTrends(void);

	//! Dump some debug data
	void dump(std::string pFilename);

	//! User decided to keep playing after mission done
	void setKeepPlaying(void) { mKeepPlaying = true; }

	//! Get keep playing state
	bool getKeepPlaying(void) { return mKeepPlaying; }

  //! Is research reachable in tree
	bool isAvailableInTree(size_t pResearchIndex);

  //! Is the players company creditworthy
	bool isCreditworthy(int pAmount, std::string& pWhyNot);

  //! Set company name
	void setName(std::string pName) { mName = pName; }

  //! Reimburse for demolished building
	void reimburse(int pAmount) { mMoney+= pAmount; }

private:

	//! Handle possible accidents (weekly)
	void handleAccidents(void);
	
	//! Handle all running costs (weekly)
	void handleRunningCosts(void);

	//! Handle all running loans (monthly)
	void handleLoans(void);

	//! Check for mission goals (weekly)
	void handleMissionGoals(void);

	//! Update mission status in "Mission goals" window (weekly)
	void updateMissionStatus(void);

	//! Update financial chart
	void updateFinanceChart(EventData* pData);

	//! Handle customers
	void handleCustomers(void);

	//! Handle currently executing offers
	void handleExecutingOffers(void);

	//! Handle sales
	void handleSales(void);

	//! Cancel a contract (with ticker message)
	void contractCanceled(bool pMoney, std::string pPartner, std::string pBuySell,
    std::string pResource);

	//! Alter resource amount
	void changeResource(EventData* pData);

	//! Day passed event handler
	void dayPassed(EventData* pData);

	//! Week passed event handler
	void weekPassed(EventData* pData);

	//! Month passed event handler
	void monthPassed(EventData* pData);

	//! Year passed event handler
	void yearPassed(EventData* pData);

	//! Read mission params from XML
	void setupMissionParams(std::string pMission);

	//! Create new random trade offers
	void createNewTradeOffers(void);


	bool mSandbox; //!< Sandbox mode?
	int mMoney; //!< €€€, $$$
	int mUranium; //!< rods
	int mCoal; //!< tons
	int mGas; //!< thousand cubic meters
	int mGasPrice, mCoalPrice, mUraniumPrice; //!< World market prices
	boost::shared_ptr<PowerNet> mPowerNet; //!< The power net(s)
	std::vector<boost::shared_ptr<ResourceBuilding> > mResourceBuildings; //!< All the resource buildings
	std::vector<boost::shared_ptr<CompanyBuilding> > mCompanyBuildings; //!< All the company buildings
	std::vector<boost::shared_ptr<City> > mCities;
	std::vector<TradingOffer> mTradingOffers; //!< Offers available
	std::vector<TradingOffer> mExecutingOffers; //!< currently running offers
	std::vector<TradingPartner> mTradingPartners; //!< Available trading partners
	std::vector<MissionGoal> mMissionGoals; //!< Current mission goals
	std::vector<Loan> mCurrentLoans; //!< Active loans
	FinancialArchive mCurrentMonth, mLastMonth, mCurrentYear, mLastYear; //!< Financial data for "Finances" window
	boost::shared_ptr<Chart> mTradeChart; //!< World market price chart
	boost::shared_ptr<Chart> mImageChart; //!< Company image and customer count chart
	boost::shared_ptr<Chart> mFinanceChart; //!< Finances chart
	double mGlobalImage; //!< Global company image (0-100)
	int mGlobalPrice; //!< Gloabl price (in ct/kWh)
	int mGlobalAdvertising; //!< Global ad budget per city
	int mGlobalMaintenance; //!< Global maintenance ratio (0-100%)
	int mCompleteCustomers; //!< Complete customer count
	int mAdBoost; //!< Current ad boost from campaigns
	std::vector<boost::shared_ptr<City> > mConnectedCities; //!< All cities connected to the players powerplants
	std::string mName; //!< The companys name
	int mIncome; //!< Finance chart valuse
	int mExpenses;
	int mNetWorth;
	eTraining mTraining; //!< Sales/Safety/Training settings
	eSupport mCustomerSupport;
	eSafety mSafety;
	eContract mContractTime;
	size_t mSupplyMaintenance;
	size_t mEnvSupport;
	int mTaxFreeTrades; //!< Special actions running data
	bool mWarActive;
	bool mPriceActive;
	bool mWasteActive;
	bool mBribeActive;
	bool mNoTrainingActive;
	bool mHeadquartersBuilt, mResearchBuilt, mPRBuilt; //!< Company buildings built?
	std::vector<AdCampaign> mCampaigns; //!< Currently running ad campaigns
	std::vector<SpecialAction> mSpecialActions; //!< Currentyl running special actions
	ResearchSet mResearchSet; //!< All the research data
	bool mBankruptcyWarningIssued; //!< Has a bankruptcy warning been issued last year?
	int mGasLastWeek, mGasTrend;
	int mCoalLastWeek, mCoalTrend;
	int mUraniumLastWeek, mUraniumTrend;
	bool mKeepPlaying;
	boost::shared_ptr<Map> mMap;
};

/*-----------------------------------------------------------------------------------------------*/

#endif // COMPANY_H
