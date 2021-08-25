#include "StdAfx.h"
#include "Company.h"
#include "GUI.h"
#include "Map.h"
#include "City.h"
#include "Powerplant.h"
#include "PowerNet.h"
#include "GameObject.h"
#include "ResourceBuilding.h"
#include "CompanyBuilding.h"
#include "Chart.h"

/*-----------------------------------------------------------------------------------------------*/

Company::Company(std::string pMission, bool pSandbox,
                 std::vector<boost::shared_ptr<City> > pCities, boost::shared_ptr<Map> pMap)
: mMoney(cDefaultStartMoney),
  mCoal(cDefaultCoalStart),
  mGas(cDefaultGasStart),
	mUranium(cDefaultUraniumStart),
  mSandbox(pSandbox),
  mCities(pCities),
	mGlobalImage(50),
  mGlobalPrice(15),
  mGlobalAdvertising(15),
  mGlobalMaintenance(100),
	mCompleteCustomers(0),
  mName(StrLoc::get()->DefaultCompanyName()),
  mNetWorth(0),
  mExpenses(0),
	mIncome(0),
  mTraining(eNormalTraining),
  mCustomerSupport(eNormalSupport),
	mSafety(eNormalSafety),
  mContractTime(e12),
  mSupplyMaintenance(100),
  mEnvSupport(0),
	mHeadquartersBuilt(false),
  mResearchBuilt(false),
  mPRBuilt(false),
  mTaxFreeTrades(0),
	mWarActive(false),
  mPriceActive(false),
  mWasteActive(false),
  mBribeActive(false),
	mNoTrainingActive(false),
  mBankruptcyWarningIssued(false),
  mGasLastWeek(0), 
	mCoalLastWeek(0),
  mUraniumLastWeek(0),
  mGasTrend(0),
  mCoalTrend(0),
  mUraniumTrend(0),
	mKeepPlaying(false),
  mMap(pMap)
{
	mPowerNet.reset(new PowerNet());

	REGISTER_CALLBACK(eChangeResource, Company::changeResource);
	REGISTER_CALLBACK(eDayPassed, Company::dayPassed);
	REGISTER_CALLBACK(eWeekPassed, Company::weekPassed);
	REGISTER_CALLBACK(eMonthPassed, Company::monthPassed);
	REGISTER_CALLBACK(eYearPassed, Company::yearPassed);
	REGISTER_CALLBACK(eUpdateFinanceChart, Company::updateFinanceChart);
	
	setupMissionParams(pMission);
	
	if(mSandbox)
	{
		mCoal = 50000;
		mGas = 50000;
		mUranium = 50000;
	}

	createNewTradeOffers();

	mTradeChart.reset(new Chart("TradeChart", 290, 133, 512, 256, false));
	mImageChart.reset(new Chart("ImageChart", 338, 185, 512, 256, true));
	mFinanceChart.reset(new Chart("FinanceChart", 560, 294, 1024, 512, true, true, true));

	std::vector<Ogre::ColourValue> lFinanceColours;

	lFinanceColours.push_back(Ogre::ColourValue(1.0, 0.0, 0.0));
	lFinanceColours.push_back(Ogre::ColourValue(0.0, 1.0, 0.0));
	lFinanceColours.push_back(Ogre::ColourValue(1.0, 1.0, 0.0));
	lFinanceColours.push_back(Ogre::ColourValue(0.0, 0.0, 1.0));

	mFinanceChart->setColourSet(lFinanceColours);

	mTradeChart->forceRepaint();
	mImageChart->forceRepaint();
	mFinanceChart->forceRepaint();

	mGasPrice = rand() % 100;
	mCoalPrice = rand() % 100;
	mUraniumPrice = rand() % 100;
}

/*-----------------------------------------------------------------------------------------------*/

void Company::adjustResource(eResourceType pType, int pAmount)
{
	if (pType == eCoal) {
		if ((mCoal == 0) && (pAmount > 0))
			EventHandler::raiseEvent(eResourceGain, new EventArg<int>(eCoal));

		if ((mCoal + pAmount) < 0) {
			mCoal = 0;
			EventHandler::raiseEvent(eResourceGone, new EventArg<int>(eCoal));
			EventHandler::raiseEvent(eMinimapModeChange, new EventArg<int>(-1));

			TickerMessage lMessage;
			lMessage.mMessage = StrLoc::get()->CoalStockDepleted();
			lMessage.mPointOfInterest = Ogre::Vector2(-1.0, -1.0);
			lMessage.mUrgent = true;
			lMessage.mDetail = StrLoc::get()->CoalStockDepletedDetail();
			lMessage.mID = 0;

			EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
		} else {
			mCoal += pAmount;
		}
	}
	else if (pType == eUranium) {
		if((mUranium == 0) && (pAmount > 0))
			EventHandler::raiseEvent(eResourceGain, new EventArg<int>(eUranium));

		if ((mUranium + pAmount) < 0) {
			mUranium = 0;
			EventHandler::raiseEvent(eResourceGone, new EventArg<int>(eUranium));
			EventHandler::raiseEvent(eMinimapModeChange, new EventArg<int>(-1));

			TickerMessage lMessage;
			lMessage.mMessage = StrLoc::get()->UraniumStockDepleted();
			lMessage.mPointOfInterest = Ogre::Vector2(-1.0, -1.0);
			lMessage.mUrgent = true;
			lMessage.mDetail = StrLoc::get()->UraniumStockDepletedDetail();
			lMessage.mID = 0;

			EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
		} else {
			mUranium += pAmount;
		}
	}
	else if (pType == eGas) {
		if((mGas == 0) && (pAmount > 0))
			EventHandler::raiseEvent(eResourceGain, new EventArg<int>(eGas));

		if((mGas + pAmount) < 0) {
			mGas = 0;
			EventHandler::raiseEvent(eResourceGone, new EventArg<int>(eGas));
			EventHandler::raiseEvent(eMinimapModeChange, new EventArg<int>(-1));

			TickerMessage lMessage;
			lMessage.mMessage = StrLoc::get()->GasStockDepleted();
			lMessage.mPointOfInterest = Ogre::Vector2(-1.0, -1.0);
			lMessage.mUrgent = true;
			lMessage.mDetail = StrLoc::get()->GasStockDepletedDetail();
			lMessage.mID = 0;

			EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
		} else {
			mGas += pAmount;
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Company::changeResource(EventData* pData)
{
	if (!mSandbox) {
		eResourceType lType = (eResourceType)static_cast<EventArg<int>*>(pData)->mData1;
		int lAmount = static_cast<EventArg<int>*>(pData)->mData2;

		adjustResource(lType, lAmount);
	}
}

/*-----------------------------------------------------------------------------------------------*/

boost::shared_ptr<PowerNet> Company::getPowerNet(void)
{
	return mPowerNet;
}

/*-----------------------------------------------------------------------------------------------*/

int Company::getMoney(void)
{
	return mMoney;
}

/*-----------------------------------------------------------------------------------------------*/

bool Company::buyObject(eCompanyObjectType pType, bool pFree)
{
	if (!mSandbox) {
		int lCost = GameConfig::getInt(GameConfig::cCOTS(pType), cCOVCost);

		if (pFree)
			lCost = 0;

		if (lCost <= mMoney) {
			mMoney -= lCost;
			mCurrentMonth.mBuilding -= lCost;
			mCurrentYear.mBuilding -= lCost;
			mNetWorth += lCost;
			mExpenses -= lCost;

			if (pType == eCOHeadquarters) {
				mHeadquartersBuilt = true;

				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->HeadquartersBuilt();
				lMessage.mDetail = StrLoc::get()->HeadquartersBuiltDetail();
				lMessage.mPointOfInterest = Ogre::Vector2(-1.0, -1.0);
				lMessage.mUrgent = false;
				lMessage.mID = 0;

				EventHandler::raiseEvent(eTickerMessage, 
					new EventArg<TickerMessage>(lMessage));
				EventHandler::raiseEvent(eUpdateCOBuildingButtons);
			}
			else if (pType == eCOResearch) {
				mResearchBuilt = true;

				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->ResearchBuilt();
				lMessage.mDetail = StrLoc::get()->ResearchBuiltDetail();
				lMessage.mPointOfInterest = Ogre::Vector2(-1.0, -1.0);
				lMessage.mUrgent = false;
				lMessage.mID = 0;

				EventHandler::raiseEvent(eTickerMessage, 
					new EventArg<TickerMessage>(lMessage));
				EventHandler::raiseEvent(eUpdateCOBuildingButtons);
			}
			else if (pType == eCOPublicRelations) {
				mPRBuilt = true;

				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->PRBuilt();
				lMessage.mDetail = StrLoc::get()->PRBuiltDetail();
				lMessage.mPointOfInterest = Ogre::Vector2(-1.0, -1.0);
				lMessage.mUrgent = false;
				lMessage.mID = 0;

				EventHandler::raiseEvent(eTickerMessage, 
					new EventArg<TickerMessage>(lMessage));
				EventHandler::raiseEvent(eUpdateCOBuildingButtons);
			}

			return true;
		} else {
			return false;
		}
	} else {
		if (pType == eCOHeadquarters)
			mHeadquartersBuilt = true;
		else if (pType == eCOResearch)
			mResearchBuilt = true;
		else if (pType == eCOPublicRelations)
			mPRBuilt = true;

		return true;
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Company::setupMissionParams(std::string pMission)
{
	boost::shared_ptr<TiXmlDocument> lMission(new TiXmlDocument((cDataDirPre 
		+ "missions/" + pMission).c_str()));

	lMission->LoadFile(TIXML_ENCODING_UTF8);

	TiXmlNode* rootNode;

	rootNode = lMission->FirstChildElement("mission");

	TiXmlNode* node = rootNode->FirstChildElement("trading_partner");

	while (node) {
		TradingPartner lPartner;
		lPartner.mPrice = toNumber<int>(node->ToElement()->Attribute("price"));

		TiXmlNode* nameNode = node->FirstChildElement("name");
		
		while (nameNode) {
			if (((std::string)nameNode->ToElement()->Attribute("language")) 
				== StrLoc::get()->MyGUILanguage().asUTF8()) {
				lPartner.mName = Ogre::UTFString(nameNode->ToElement()->GetText());
				break;
			}

			nameNode = nameNode->NextSiblingElement("name");
		}

		if (lPartner.mPrice == 0)
			lPartner.mAvailable = true;
		else
			lPartner.mAvailable = false;

		mTradingPartners.push_back(lPartner);
			
		node = node->NextSiblingElement("trading_partner");
	}
	
	node = rootNode->FirstChildElement("mission_goal");

	while (node) {
		MissionGoal lTemp;

		TiXmlNode* textNode = node->FirstChildElement("text");

		while (textNode) {
			if (((std::string)textNode->ToElement()->Attribute("language")) 
				== StrLoc::get()->MyGUILanguage().asUTF8()) {
				lTemp.mText = textNode->ToElement()->GetText();
			}

			textNode = textNode->NextSiblingElement("text");
		}

		lTemp.mDone = false;

		std::string lType = node->ToElement()->Attribute("type");

		if (lType == "freeplay") {
			lTemp.mType = eFreePlay;
			lTemp.mDone = true;
			lTemp.mValueNeeded = 0;
		}
		else if (lType == "money") {
			lTemp.mType = eMoney;
			lTemp.mValueNeeded = toNumber<int>(node->ToElement()->Attribute("value"));
		}
		else if (lType == "customers") {
			lTemp.mType = eCustomerCount;
			lTemp.mValueNeeded = toNumber<int>(node->ToElement()->Attribute("value"));
		}

		lTemp.mValueCurrent = 0;

		mMissionGoals.push_back(lTemp);

		node = node->NextSiblingElement("mission_goal");
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Company::serializeIntoXMLElement(TiXmlElement* pParentElement, boost::shared_ptr<GUI> pGUI)
{
	TiXmlElement* lCompanySerialized = new TiXmlElement("company");
	lCompanySerialized->SetAttribute("money", mMoney);
	lCompanySerialized->SetAttribute("gas", mGas);
	lCompanySerialized->SetAttribute("coal", mCoal);
	lCompanySerialized->SetAttribute("uranium", mUranium);
	lCompanySerialized->SetAttribute("global_price", mGlobalPrice);
	lCompanySerialized->SetAttribute("global_image", mGlobalImage);
	lCompanySerialized->SetAttribute("global_advertising", mGlobalAdvertising);
	lCompanySerialized->SetAttribute("global_maintenance", mGlobalMaintenance);
	lCompanySerialized->SetAttribute("gas_price", mGasPrice);
	lCompanySerialized->SetAttribute("coal_price", mCoalPrice);
	lCompanySerialized->SetAttribute("uranium_price", mUraniumPrice);
	lCompanySerialized->SetAttribute("customers_complete", mCompleteCustomers);
	lCompanySerialized->SetAttribute("contract_time", (int)mContractTime);
	lCompanySerialized->SetAttribute("safety", (int)mSafety);
	lCompanySerialized->SetAttribute("customer_support", (int)mCustomerSupport);
	lCompanySerialized->SetAttribute("training", (int)mTraining);
	lCompanySerialized->SetAttribute("env_support", mEnvSupport);
	lCompanySerialized->SetAttribute("supply_maintenance", mSupplyMaintenance);
	lCompanySerialized->SetAttribute("free_trades", mTaxFreeTrades);
	lCompanySerialized->SetAttribute("net_worth", mNetWorth);
	lCompanySerialized->SetAttribute("company_name", mName.c_str());
	pParentElement->LinkEndChild(lCompanySerialized);

	for (size_t i = 0; i < mCurrentLoans.size(); ++i) {
		TiXmlElement* lLoanSerialized = new TiXmlElement("loan");
		lLoanSerialized->SetAttribute("amount_left", mCurrentLoans[i].mAmountLeft);
		lLoanSerialized->SetAttribute("month_left", mCurrentLoans[i].mMonthLeft);
		lLoanSerialized->SetAttribute("monthly_rate", mCurrentLoans[i].mMonthlyRate);
		pParentElement->LinkEndChild(lLoanSerialized);
	}

	for (size_t j = 0; j < mExecutingOffers.size(); ++j) {
		TiXmlElement* lOfferSerialized = new TiXmlElement("trade_obligation");
		lOfferSerialized->SetAttribute("partner", mExecutingOffers[j].mTradingPartner.c_str());
		lOfferSerialized->SetAttribute("resource", (int)mExecutingOffers[j].mResource);
		lOfferSerialized->SetAttribute("amount", mExecutingOffers[j].mAmount);
		lOfferSerialized->SetAttribute("weeks", mExecutingOffers[j].mWeeks);
		lOfferSerialized->SetAttribute("discount", mExecutingOffers[j].mDiscount);
		lOfferSerialized->SetAttribute("fixed_price", mExecutingOffers[j].mFixedPrice);
		lOfferSerialized->SetAttribute("unique", mExecutingOffers[j].mUnique ? "true" : "false");
		lOfferSerialized->SetAttribute("world_market", mExecutingOffers[j].mWorldMarket ? "true" : "false");
		lOfferSerialized->SetAttribute("permanent", mExecutingOffers[j].mPermanent ? "true" : "false");
		pParentElement->LinkEndChild(lOfferSerialized);
	}

	for (size_t k = 0; k < mTradingPartners.size(); ++k) {
		TiXmlElement* lTradingPartner = new TiXmlElement("trading_partner");
		lTradingPartner->SetAttribute("name", mTradingPartners[k].mName.asUTF8_c_str());
		lTradingPartner->SetAttribute("available", mTradingPartners[k].mAvailable ? "true" : "false");
		pParentElement->LinkEndChild(lTradingPartner);
	}

	for (size_t l = 0; l < mCampaigns.size(); ++l) {
		TiXmlElement* lCampaign = new TiXmlElement("campaign");
		lCampaign->SetAttribute("type", (int)mCampaigns[l].mType);
		lCampaign->SetAttribute("weeks_left", mCampaigns[l].mWeeksLeft);
		pParentElement->LinkEndChild(lCampaign);
	}

	for (size_t m = 0; m < mSpecialActions.size(); ++m) {
		TiXmlElement* lSpecialAction = new TiXmlElement("special_action");
		lSpecialAction->SetAttribute("type", (int)mSpecialActions[m].mType);
		lSpecialAction->SetAttribute("trades_left", mSpecialActions[m].mTradesLeft);
		lSpecialAction->SetAttribute("month_left", mSpecialActions[m].mMonthLeft);
		lSpecialAction->SetAttribute("done", mSpecialActions[m].mDone ? "true" : "false");
		pParentElement->LinkEndChild(lSpecialAction);
	}

	for (size_t n = 0; n < mCompanyBuildings.size(); ++n) {
		TiXmlElement* lCompanyBuilding = new TiXmlElement("company_building");
		lCompanyBuilding->SetAttribute("pos_x", mCompanyBuildings[n]->getPosition().x);
		lCompanyBuilding->SetAttribute("pos_y", mCompanyBuildings[n]->getPosition().y);
		lCompanyBuilding->SetAttribute("type", (int)mCompanyBuildings[n]->getSubtype());
		lCompanyBuilding->SetAttribute("id", mCompanyBuildings[n]->getID().c_str());
		pParentElement->LinkEndChild(lCompanyBuilding);
	}

	for (size_t o = 0; o < mResourceBuildings.size(); ++o) {
		TiXmlElement* lResourceBuilding = new TiXmlElement("resource_building");
		lResourceBuilding->SetAttribute("pos_x", mResourceBuildings[o]->getPosition().x);
		lResourceBuilding->SetAttribute("pos_y", mResourceBuildings[o]->getPosition().y);
		lResourceBuilding->SetAttribute("type", (int)mResourceBuildings[o]->getSubtype());
		lResourceBuilding->SetAttribute("id", mResourceBuildings[o]->getID().c_str());
		lResourceBuilding->SetAttribute("inherit", mResourceBuildings[o]->getInheritSettings() ? true : false);
		lResourceBuilding->SetAttribute("maintenance", mResourceBuildings[o]->getMaintenance());
		lResourceBuilding->SetAttribute("counter", mResourceBuildings[o]->getCounter());
		lResourceBuilding->SetAttribute("damaged", mResourceBuildings[o]->getDamaged() ? true : false);
		lResourceBuilding->SetAttribute("condition", mResourceBuildings[o]->getCondition());
		pParentElement->LinkEndChild(lResourceBuilding);
	}

	TiXmlElement* lCurrentMonth = new TiXmlElement("finances_current_month");
	lCurrentMonth->SetAttribute("advertising", mCurrentMonth.mAdvertising);
	lCurrentMonth->SetAttribute("building", mCurrentMonth.mBuilding);
	lCurrentMonth->SetAttribute("customer_income", mCurrentMonth.mCustomerIncome);
	lCurrentMonth->SetAttribute("environmental", mCurrentMonth.mEnvironmental);
	lCurrentMonth->SetAttribute("loan_payments", mCurrentMonth.mLoanPayments);
	lCurrentMonth->SetAttribute("maintenance", mCurrentMonth.mMaintenance);
	lCurrentMonth->SetAttribute("operation", mCurrentMonth.mOperation);
	lCurrentMonth->SetAttribute("research", mCurrentMonth.mResearch);
	lCurrentMonth->SetAttribute("special_expenses", mCurrentMonth.mSpecialExpenses);
	lCurrentMonth->SetAttribute("trade", mCurrentMonth.mTrade);
	lCurrentMonth->SetAttribute("training_safety", mCurrentMonth.mTrainingSafety);
	pParentElement->LinkEndChild(lCurrentMonth);

	TiXmlElement* lCurrentYear = new TiXmlElement("finances_current_year");
	lCurrentYear->SetAttribute("advertising", mCurrentYear.mAdvertising);
	lCurrentYear->SetAttribute("building", mCurrentYear.mBuilding);
	lCurrentYear->SetAttribute("customer_income", mCurrentYear.mCustomerIncome);
	lCurrentYear->SetAttribute("environmental", mCurrentYear.mEnvironmental);
	lCurrentYear->SetAttribute("loan_payments", mCurrentYear.mLoanPayments);
	lCurrentYear->SetAttribute("maintenance", mCurrentYear.mMaintenance);
	lCurrentYear->SetAttribute("operation", mCurrentYear.mOperation);
	lCurrentYear->SetAttribute("research", mCurrentYear.mResearch);
	lCurrentYear->SetAttribute("special_expenses", mCurrentYear.mSpecialExpenses);
	lCurrentYear->SetAttribute("trade", mCurrentYear.mTrade);
	lCurrentYear->SetAttribute("training_safety", mCurrentYear.mTrainingSafety);
	pParentElement->LinkEndChild(lCurrentYear);

	TiXmlElement* lLastMonth = new TiXmlElement("finances_last_month");
	lLastMonth->SetAttribute("advertising", mLastMonth.mAdvertising);
	lLastMonth->SetAttribute("building", mLastMonth.mBuilding);
	lLastMonth->SetAttribute("customer_income", mLastMonth.mCustomerIncome);
	lLastMonth->SetAttribute("environmental", mLastMonth.mEnvironmental);
	lLastMonth->SetAttribute("loan_payments", mLastMonth.mLoanPayments);
	lLastMonth->SetAttribute("maintenance", mLastMonth.mMaintenance);
	lLastMonth->SetAttribute("operation", mLastMonth.mOperation);
	lLastMonth->SetAttribute("research", mLastMonth.mResearch);
	lLastMonth->SetAttribute("special_expenses", mLastMonth.mSpecialExpenses);
	lLastMonth->SetAttribute("trade", mLastMonth.mTrade);
	lLastMonth->SetAttribute("training_safety", mLastMonth.mTrainingSafety);
	pParentElement->LinkEndChild(lLastMonth);

	TiXmlElement* lLastYear = new TiXmlElement("finances_last_year");
	lLastYear->SetAttribute("advertising", mLastYear.mAdvertising);
	lLastYear->SetAttribute("building", mLastYear.mBuilding);
	lLastYear->SetAttribute("customer_income", mLastYear.mCustomerIncome);
	lLastYear->SetAttribute("environmental", mLastYear.mEnvironmental);
	lLastYear->SetAttribute("loan_payments", mLastYear.mLoanPayments);
	lLastYear->SetAttribute("maintenance", mLastYear.mMaintenance);
	lLastYear->SetAttribute("operation", mLastYear.mOperation);
	lLastYear->SetAttribute("research", mLastYear.mResearch);
	lLastYear->SetAttribute("special_expenses", mLastYear.mSpecialExpenses);
	lLastYear->SetAttribute("trade", mLastYear.mTrade);
	lLastYear->SetAttribute("training_safety", mLastYear.mTrainingSafety);
	pParentElement->LinkEndChild(lLastYear);

	for (size_t p = 0; p < 21; ++p) {
		TiXmlElement* lResearchSet = new TiXmlElement("research_set");

		lResearchSet->SetAttribute("researched", mResearchSet.mResearched[p] ? "true" : "false");
		lResearchSet->SetAttribute("started", mResearchSet.mStarted[p] ? "true" : "false");
		lResearchSet->SetAttribute("price", mResearchSet.mPrice[p]);
		lResearchSet->SetAttribute("name", mResearchSet.mName[p].c_str());
		lResearchSet->SetAttribute("weeks_left", mResearchSet.mWeeksLeft[p]);
		lResearchSet->SetAttribute("weeks_required", mResearchSet.mWeeks[p]);

		pParentElement->LinkEndChild(lResearchSet);
	}

	for (size_t q = 0; q < pGUI->getTickerArchive().size(); ++q) {
		TiXmlElement* lTickerMessage = new TiXmlElement("ticker_message");

		lTickerMessage->SetAttribute("id", pGUI->getTickerArchive()[q].mID);
		lTickerMessage->SetAttribute("poi_x", pGUI->getTickerArchive()[q].mPointOfInterest.x);
		lTickerMessage->SetAttribute("poi_y", pGUI->getTickerArchive()[q].mPointOfInterest.y);
		lTickerMessage->SetAttribute("text", pGUI->getTickerArchive()[q].mMessage.c_str());
		lTickerMessage->SetAttribute("detail", pGUI->getTickerArchive()[q].mDetail.c_str());
		lTickerMessage->SetAttribute("urgent", pGUI->getTickerArchive()[q].mUrgent ? "true" : "false");
		lTickerMessage->SetAttribute("date_time", pGUI->getTickerArchive()[q].mDateTime.c_str());

		pParentElement->LinkEndChild(lTickerMessage);
	}

	mPowerNet->serializeIntoXMLElement(pParentElement);

	TiXmlElement* lTradeChart = new TiXmlElement("trade_chart");
	mTradeChart->serializeIntoXMLElement(lTradeChart);
	pParentElement->LinkEndChild(lTradeChart);

	TiXmlElement* lImageChart = new TiXmlElement("image_chart");
	mImageChart->serializeIntoXMLElement(lImageChart);
	pParentElement->LinkEndChild(lImageChart);

	TiXmlElement* lFinanceChart = new TiXmlElement("finance_chart");
	mFinanceChart->serializeIntoXMLElement(lFinanceChart);
	pParentElement->LinkEndChild(lFinanceChart);
}

/*-----------------------------------------------------------------------------------------------*/

void Company::deserializeFromXMLElement(TiXmlNode* pParentNode, boost::shared_ptr<Map> pMap,
										Ogre::SceneManager* pSceneManager, boost::shared_ptr<Company> pCompany,
										boost::shared_ptr<GUI> pGUI)
{
	TiXmlNode* lCompanyNode = pParentNode->FirstChildElement("company");

	mMoney = toNumber<int>(lCompanyNode->ToElement()->Attribute("money"));
	mGas = toNumber<int>(lCompanyNode->ToElement()->Attribute("gas"));
	mCoal = toNumber<int>(lCompanyNode->ToElement()->Attribute("coal"));
	mUranium = toNumber<int>(lCompanyNode->ToElement()->Attribute("uranium"));
	mGlobalPrice = toNumber<int>(lCompanyNode->ToElement()->Attribute("global_price"));
	mGlobalImage = toNumber<int>(lCompanyNode->ToElement()->Attribute("global_image"));
	mGlobalAdvertising = toNumber<int>(lCompanyNode->ToElement()->Attribute("global_advertising"));
	mGlobalMaintenance = toNumber<int>(lCompanyNode->ToElement()->Attribute("global_maintenance"));
	mGasPrice = toNumber<int>(lCompanyNode->ToElement()->Attribute("gas_price"));
	mCoalPrice = toNumber<int>(lCompanyNode->ToElement()->Attribute("coal_price"));
	mUraniumPrice = toNumber<int>(lCompanyNode->ToElement()->Attribute("uranium_price"));
	mCompleteCustomers = toNumber<int>(lCompanyNode->ToElement()->Attribute("customers_complete"));
	mContractTime = (eContract)toNumber<int>(lCompanyNode->ToElement()->Attribute("contract_time"));
	mSafety = (eSafety)toNumber<int>(lCompanyNode->ToElement()->Attribute("safety"));
	mCustomerSupport = (eSupport)toNumber<int>(lCompanyNode->ToElement()->Attribute("customer_support"));
	mTraining = (eTraining)toNumber<int>(lCompanyNode->ToElement()->Attribute("training"));
	mEnvSupport = toNumber<int>(lCompanyNode->ToElement()->Attribute("env_support"));
	mSupplyMaintenance = toNumber<int>(lCompanyNode->ToElement()->Attribute("supply_maintenance"));
	mTaxFreeTrades = toNumber<int>(lCompanyNode->ToElement()->Attribute("free_trades"));
	mNetWorth = toNumber<int>(lCompanyNode->ToElement()->Attribute("net_worth"));
	mName = lCompanyNode->ToElement()->Attribute("company_name");

	TiXmlNode* lLoanNode = pParentNode->FirstChildElement("loan");

	while (lLoanNode) {
		Loan lTemp;
		lTemp.mMonthlyRate = toNumber<int>(lLoanNode->ToElement()->Attribute("monthly_rate"));
		lTemp.mMonthLeft = toNumber<int>(lLoanNode->ToElement()->Attribute("month_left"));
		lTemp.mAmountLeft = toNumber<int>(lLoanNode->ToElement()->Attribute("amount_left"));
		mCurrentLoans.push_back(lTemp);
		lLoanNode = lLoanNode->NextSiblingElement("loan");
	}

	TiXmlNode* lTradeObligationNode = pParentNode->FirstChildElement("trade_obligation");

	while (lTradeObligationNode) {
		TradingOffer lTemp;
		lTemp.mTradingPartner = lTradeObligationNode->ToElement()->Attribute("partner");
		lTemp.mResource = (eResourceType)toNumber<int>(lTradeObligationNode->ToElement()->Attribute("resource"));
		lTemp.mAmount = toNumber<int>(lTradeObligationNode->ToElement()->Attribute("amount"));
		lTemp.mWeeks = toNumber<int>(lTradeObligationNode->ToElement()->Attribute("weeks"));
		lTemp.mDiscount = toNumber<int>(lTradeObligationNode->ToElement()->Attribute("discount"));
		lTemp.mFixedPrice = toNumber<int>(lTradeObligationNode->ToElement()->Attribute("fixed_price"));
		lTemp.mUnique = (((std::string)lTradeObligationNode->ToElement()->Attribute("unique")) == "false") ? false : true;
		lTemp.mWorldMarket = (((std::string)lTradeObligationNode->ToElement()->Attribute("world_market")) == "false") ? false : true;
		lTemp.mPermanent = (((std::string)lTradeObligationNode->ToElement()->Attribute("permanent")) == "false") ? false : true;
		mExecutingOffers.push_back(lTemp);
		lTradeObligationNode = lTradeObligationNode->NextSiblingElement("trade_obligation");
	}

	TiXmlNode* lTradingPartnerNode = pParentNode->FirstChildElement("trading_partner");

	while (lTradingPartnerNode) {
		std::string lName = lTradingPartnerNode->ToElement()->Attribute("name");
		bool lAvailable = (((std::string)lTradingPartnerNode->ToElement()->Attribute("available")) == "false") ? false : true;

		for(size_t i = 0; i < mTradingPartners.size(); ++i)
			if(mTradingPartners[i].mName == lName)
				mTradingPartners[i].mAvailable = lAvailable;

		lTradingPartnerNode = lTradingPartnerNode->NextSiblingElement("trading_partner");
	}

	TiXmlNode* lAdCampaignNode = pParentNode->FirstChildElement("campaign");

	while (lAdCampaignNode) {
		AdCampaign lTemp;
		lTemp.mType = (eAdType)toNumber<int>(lAdCampaignNode->ToElement()->Attribute("type"));
		lTemp.mWeeksLeft = toNumber<int>(lAdCampaignNode->ToElement()->Attribute("weeks_left"));
		mCampaigns.push_back(lTemp);
		lAdCampaignNode = lAdCampaignNode->NextSiblingElement("campaign");
	}

	TiXmlNode* lSpecialActionNode = pParentNode->FirstChildElement("special_action");

	while (lSpecialActionNode) {
		SpecialAction lTemp;
		lTemp.mType = (eSpecialAction)toNumber<int>(lSpecialActionNode->ToElement()->Attribute("type"));
		lTemp.mMonthLeft = toNumber<int>(lSpecialActionNode->ToElement()->Attribute("month_left"));
		lTemp.mDone = (((std::string)lSpecialActionNode->ToElement()->Attribute("done")) == "false") ? false : true;
		lTemp.mTradesLeft = toNumber<int>(lSpecialActionNode->ToElement()->Attribute("trades_left"));
		mSpecialActions.push_back(lTemp);
		lSpecialActionNode = lSpecialActionNode->NextSiblingElement("special_action");
	}

	TiXmlNode* lCompanyBuildingNode = pParentNode->FirstChildElement("company_building");

	while (lCompanyBuildingNode) {
		Ogre::Vector2 lPosition;
		lPosition.x = toNumber<int>(lCompanyBuildingNode->ToElement()->Attribute("pos_x"));
		lPosition.y = toNumber<int>(lCompanyBuildingNode->ToElement()->Attribute("pos_y"));

		eCompanyObjectType lType = (eCompanyObjectType)toNumber<int>(lCompanyBuildingNode->ToElement()->Attribute("type"));

		boost::shared_ptr<CompanyBuilding> lNewBuilding;
		lNewBuilding.reset(new CompanyBuilding(lPosition, lType));
		lNewBuilding->setID(toNumber<int>(lCompanyBuildingNode->ToElement()->Attribute("id")));

		lNewBuilding->addToSceneManager(pSceneManager, pMap->getTerrain());
		pMap->addCollidable(lNewBuilding, true);

		if (lType == eCOHeadquarters)
			mHeadquartersBuilt = true;
		else if (lType == eCOResearch)
			mResearchBuilt = true;
		else if (lType == eCOPublicRelations)
			mPRBuilt = true;

		mCompanyBuildings.push_back(lNewBuilding);
		lCompanyBuildingNode = lCompanyBuildingNode->NextSiblingElement("company_building");
	}

	TiXmlNode* lResourceBuildingNode = pParentNode->FirstChildElement("resource_building");

	while (lResourceBuildingNode) {
		Ogre::Vector2 lPosition;
		lPosition.x = toNumber<int>(lResourceBuildingNode->ToElement()->Attribute("pos_x"));
		lPosition.y = toNumber<int>(lResourceBuildingNode->ToElement()->Attribute("pos_y"));

		eCompanyObjectType lType = (eCompanyObjectType)toNumber<int>(lResourceBuildingNode->ToElement()->Attribute("type"));
		size_t lCounter = toNumber<int>(lResourceBuildingNode->ToElement()->Attribute("counter"));

		boost::shared_ptr<ResourceBuilding> lNewBuilding;
		lNewBuilding.reset(new ResourceBuilding(lPosition, lType, lCounter, pCompany));
		lNewBuilding->setID(toNumber<int>(lResourceBuildingNode->ToElement()->Attribute("id")));
		lNewBuilding->setInheritSettings((((std::string)lResourceBuildingNode->ToElement()->Attribute("inherit")) == "false") ? false : true);
		lNewBuilding->setMaintenance(toNumber<int>(lResourceBuildingNode->ToElement()->Attribute("maintenance")));

		if (((std::string)lResourceBuildingNode->ToElement()->Attribute("damaged")) == "true")
			lNewBuilding->setDamaged();

		lNewBuilding->setCondition(toNumber<int>(lResourceBuildingNode->ToElement()->Attribute("condition")));

		lNewBuilding->addToSceneManager(pSceneManager, pMap->getTerrain());
		pMap->addCollidable(lNewBuilding, true);

		mResourceBuildings.push_back(lNewBuilding);
		lResourceBuildingNode = lResourceBuildingNode->NextSiblingElement("resource_building");
	}

	TiXmlNode* lCurrentMonthNode = pParentNode->FirstChildElement("finances_current_month");
	mCurrentMonth.mAdvertising = toNumber<int>(lCurrentMonthNode->ToElement()->Attribute("advertising"));
	mCurrentMonth.mBuilding = toNumber<int>(lCurrentMonthNode->ToElement()->Attribute("building"));
	mCurrentMonth.mCustomerIncome = toNumber<int>(lCurrentMonthNode->ToElement()->Attribute("customer_income"));
	mCurrentMonth.mEnvironmental = toNumber<int>(lCurrentMonthNode->ToElement()->Attribute("environmental"));
	mCurrentMonth.mLoanPayments = toNumber<int>(lCurrentMonthNode->ToElement()->Attribute("loan_payments"));
	mCurrentMonth.mMaintenance = toNumber<int>(lCurrentMonthNode->ToElement()->Attribute("maintenance"));
	mCurrentMonth.mOperation = toNumber<int>(lCurrentMonthNode->ToElement()->Attribute("operation"));
	mCurrentMonth.mResearch = toNumber<int>(lCurrentMonthNode->ToElement()->Attribute("research"));
	mCurrentMonth.mSpecialExpenses = toNumber<int>(lCurrentMonthNode->ToElement()->Attribute("special_expenses"));
	mCurrentMonth.mTrade = toNumber<int>(lCurrentMonthNode->ToElement()->Attribute("trade"));
	mCurrentMonth.mTrainingSafety = toNumber<int>(lCurrentMonthNode->ToElement()->Attribute("training_safety"));

	TiXmlNode* lCurrentYearNode = pParentNode->FirstChildElement("finances_current_year");
	mCurrentYear.mAdvertising = toNumber<int>(lCurrentYearNode->ToElement()->Attribute("advertising"));
	mCurrentYear.mBuilding = toNumber<int>(lCurrentYearNode->ToElement()->Attribute("building"));
	mCurrentYear.mCustomerIncome = toNumber<int>(lCurrentYearNode->ToElement()->Attribute("customer_income"));
	mCurrentYear.mEnvironmental = toNumber<int>(lCurrentYearNode->ToElement()->Attribute("environmental"));
	mCurrentYear.mLoanPayments = toNumber<int>(lCurrentYearNode->ToElement()->Attribute("loan_payments"));
	mCurrentYear.mMaintenance = toNumber<int>(lCurrentYearNode->ToElement()->Attribute("maintenance"));
	mCurrentYear.mOperation = toNumber<int>(lCurrentYearNode->ToElement()->Attribute("operation"));
	mCurrentYear.mResearch = toNumber<int>(lCurrentYearNode->ToElement()->Attribute("research"));
	mCurrentYear.mSpecialExpenses = toNumber<int>(lCurrentYearNode->ToElement()->Attribute("special_expenses"));
	mCurrentYear.mTrade = toNumber<int>(lCurrentYearNode->ToElement()->Attribute("trade"));
	mCurrentYear.mTrainingSafety = toNumber<int>(lCurrentYearNode->ToElement()->Attribute("training_safety"));

	TiXmlNode* lLastMonthNode = pParentNode->FirstChildElement("finances_last_month");
	mLastMonth.mAdvertising = toNumber<int>(lLastMonthNode->ToElement()->Attribute("advertising"));
	mLastMonth.mBuilding = toNumber<int>(lLastMonthNode->ToElement()->Attribute("building"));
	mLastMonth.mCustomerIncome = toNumber<int>(lLastMonthNode->ToElement()->Attribute("customer_income"));
	mLastMonth.mEnvironmental = toNumber<int>(lLastMonthNode->ToElement()->Attribute("environmental"));
	mLastMonth.mLoanPayments = toNumber<int>(lLastMonthNode->ToElement()->Attribute("loan_payments"));
	mLastMonth.mMaintenance = toNumber<int>(lLastMonthNode->ToElement()->Attribute("maintenance"));
	mLastMonth.mOperation = toNumber<int>(lLastMonthNode->ToElement()->Attribute("operation"));
	mLastMonth.mResearch = toNumber<int>(lLastMonthNode->ToElement()->Attribute("research"));
	mLastMonth.mSpecialExpenses = toNumber<int>(lLastMonthNode->ToElement()->Attribute("special_expenses"));
	mLastMonth.mTrade = toNumber<int>(lLastMonthNode->ToElement()->Attribute("trade"));
	mLastMonth.mTrainingSafety = toNumber<int>(lLastMonthNode->ToElement()->Attribute("training_safety"));

	TiXmlNode* lLastYearNode = pParentNode->FirstChildElement("finances_last_year");
	mLastYear.mAdvertising = toNumber<int>(lLastYearNode->ToElement()->Attribute("advertising"));
	mLastYear.mBuilding = toNumber<int>(lLastYearNode->ToElement()->Attribute("building"));
	mLastYear.mCustomerIncome = toNumber<int>(lLastYearNode->ToElement()->Attribute("customer_income"));
	mLastYear.mEnvironmental = toNumber<int>(lLastYearNode->ToElement()->Attribute("environmental"));
	mLastYear.mLoanPayments = toNumber<int>(lLastYearNode->ToElement()->Attribute("loan_payments"));
	mLastYear.mMaintenance = toNumber<int>(lLastYearNode->ToElement()->Attribute("maintenance"));
	mLastYear.mOperation = toNumber<int>(lLastYearNode->ToElement()->Attribute("operation"));
	mLastYear.mResearch = toNumber<int>(lLastYearNode->ToElement()->Attribute("research"));
	mLastYear.mSpecialExpenses = toNumber<int>(lLastYearNode->ToElement()->Attribute("special_expenses"));
	mLastYear.mTrade = toNumber<int>(lLastYearNode->ToElement()->Attribute("trade"));
	mLastYear.mTrainingSafety = toNumber<int>(lLastYearNode->ToElement()->Attribute("training_safety"));

	TiXmlNode* lResearchSetNode = pParentNode->FirstChildElement("research_set");

	for (size_t j = 0; j < 21; ++j) {
		mResearchSet.mResearched[j] = ((((std::string)lResearchSetNode->ToElement()->Attribute("researched")) == "false") ? false : true);
		mResearchSet.mStarted[j] = ((((std::string)lResearchSetNode->ToElement()->Attribute("started")) == "false") ? false : true);
		mResearchSet.mPrice[j] = toNumber<int>(lResearchSetNode->ToElement()->Attribute("price"));
		mResearchSet.mName[j] = lResearchSetNode->ToElement()->Attribute("name");
		mResearchSet.mWeeksLeft[j] = toNumber<int>(lResearchSetNode->ToElement()->Attribute("weeks_left"));
		mResearchSet.mWeeks[j] = toNumber<int>(lResearchSetNode->ToElement()->Attribute("weeks_required"));

		lResearchSetNode = lResearchSetNode->NextSiblingElement("research_set");
	}

	std::vector<TickerMessage> lSerializedArchive;

	TiXmlNode* lTickerMessageNode = pParentNode->FirstChildElement("ticker_message");

	while (lTickerMessageNode) {
		TickerMessage lTemp;
		lTemp.mID = toNumber<int>(lTickerMessageNode->ToElement()->Attribute("id"));
		lTemp.mPointOfInterest.x = toNumber<int>(lTickerMessageNode->ToElement()->Attribute("poi_x"));
		lTemp.mPointOfInterest.y = toNumber<int>(lTickerMessageNode->ToElement()->Attribute("poi_y"));
		lTemp.mMessage = lTickerMessageNode->ToElement()->Attribute("text");
		lTemp.mDetail = lTickerMessageNode->ToElement()->Attribute("detail");
		lTemp.mUrgent = (((std::string)lTickerMessageNode->ToElement()->Attribute("urgent")) == "false" ? false : true);
		lTemp.mDateTime = lTickerMessageNode->ToElement()->Attribute("date_time");

		lSerializedArchive.push_back(lTemp);
		lTickerMessageNode = lTickerMessageNode->NextSiblingElement("ticker_message");
	}

	pGUI->setTickerArchive(lSerializedArchive);

	mPowerNet->deserializeFromXMLElement(pParentNode, pMap, pSceneManager, pCompany);

	TiXmlNode* lTradeChart = pParentNode->FirstChildElement("trade_chart");
	mTradeChart->deserializeFromXMLElement(lTradeChart);

	TiXmlNode* lImageChart = pParentNode->FirstChildElement("image_chart");
	mImageChart->deserializeFromXMLElement(lImageChart);

	TiXmlNode* lFinanceChart = pParentNode->FirstChildElement("finance_chart");
	mFinanceChart->deserializeFromXMLElement(lFinanceChart);
}

/*-----------------------------------------------------------------------------------------------*/

void Company::addResourceBuilding(boost::shared_ptr<ResourceBuilding> pBuilding)
{
	mResourceBuildings.push_back(pBuilding);
}

/*-----------------------------------------------------------------------------------------------*/

bool Company::runAdCampaign(eAdType pType)
{
	int lCost = 0;

	if (pType == eInternet)
		lCost = GameConfig::getInt("AdInternetCost");
	else if (pType == eNewspaper)
		lCost = GameConfig::getInt("AdNewspaperCost");
	else if (pType == eRadio)
		lCost = GameConfig::getInt("AdRadioCost");
	else if (pType == eTV)
		lCost = GameConfig::getInt("AdTVCost");

	if (mMoney >= lCost) {
		mMoney -= lCost;
		mCurrentMonth.mAdvertising -= lCost;
		mCurrentYear.mAdvertising -= lCost;

		AdCampaign lNewCampaign;
		lNewCampaign.mType = pType;
		lNewCampaign.mWeeksLeft = GameConfig::getInt("AdLenght");
		mCampaigns.push_back(lNewCampaign);
		return true;
	} else {
		return false;
	}
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<std::string> Company::getCityOpinions(int pCityIndex)
{
	std::vector<std::string> lOpinions;

	if (mGlobalImage > 50.0 && randBool())
		lOpinions.push_back(mName + StrLoc::get()->OpinionIsOK());
	else if (randBool())
		lOpinions.push_back(StrLoc::get()->OpinionDontLike1() + mName 
    + StrLoc::get()->OpinionDontLike2());

	if (mGlobalImage > 85.0 && randBool())
			lOpinions.push_back(mName + StrLoc::get()->OpinionIsGreat());

	if (mGlobalAdvertising > 20 && randBool())
			lOpinions.push_back(StrLoc::get()->OpinionSawAd1() + mName 
      + StrLoc::get()->OpinionSawAd2());

	if (mGlobalPrice > 20 && randBool())
		lOpinions.push_back(StrLoc::get()->OpinionPriceHigh1() + mName 
    + StrLoc::get()->OpinionPriceHigh2());
	else if (randBool())
		lOpinions.push_back(StrLoc::get()->OpinionPriceOK1() + mName 
    + StrLoc::get()->OpinionPriceOK2());

	return lOpinions;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<std::string> Company::getCustomerOpinions(bool pAllCities, int pCityIndex)
{
	std::vector<std::string> lOpinions;

	if (pAllCities) {
		for (size_t i = 0; i < mConnectedCities.size(); i++) {
			std::vector<std::string> lCityOpinions = getCityOpinions(-1);
			lOpinions.insert(lOpinions.end(), lCityOpinions.begin(), lCityOpinions.end());
		}
	} else {
		lOpinions = getCityOpinions(-1);
	}

	return lOpinions;
}

/*-----------------------------------------------------------------------------------------------*/

void Company::dayPassed(EventData* pData)
{
	mCoalPrice += (rand() % 6) * (((rand() % 2) == 0) ? -1 : 1);
	mGasPrice += (rand() % 6) * (((rand() % 2) == 0) ? -1 : 1);
	mUraniumPrice += (rand() % 6) * (((rand() % 2) == 0) ? -1 : 1);

	if (mCoalPrice > GameConfig::getInt("CoalUpperCap"))
		mCoalPrice = GameConfig::getInt("CoalUpperCap");
	if (mGasPrice > GameConfig::getInt("GasUpperCap"))
		mGasPrice = GameConfig::getInt("GasUpperCap");
	if (mUraniumPrice > GameConfig::getInt("UraniumUpperCap"))
		mUraniumPrice = GameConfig::getInt("UraniumUpperCap");

	if (mCoalPrice < GameConfig::getInt("CoalLowerCap"))
		mCoalPrice = GameConfig::getInt("CoalLowerCap");
	if (mGasPrice < GameConfig::getInt("GasLowerCap"))
		mGasPrice = GameConfig::getInt("GasLowerCap");
	if (mUraniumPrice < GameConfig::getInt("UraniumLowerCap"))
		mUraniumPrice = GameConfig::getInt("UraniumLowerCap");

	std::vector<int> lValuesTrade;

	lValuesTrade.push_back(mCoalPrice);
	lValuesTrade.push_back(mGasPrice);
	lValuesTrade.push_back(mUraniumPrice);

	mTradeChart->addValueSet(lValuesTrade);

	EventHandler::raiseEvent(eUpdateGUICompanyData);
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<FinancialArchive> Company::getFinancialData(void)
{
	std::vector<FinancialArchive> lData;

	mCurrentMonth.updateTotal();
	mLastMonth.updateTotal();
	mCurrentYear.updateTotal();
	mLastYear.updateTotal();

	lData.push_back(mCurrentMonth);
	lData.push_back(mLastMonth);
	lData.push_back(mCurrentYear);
	lData.push_back(mLastYear);

	return lData;
}

/*-----------------------------------------------------------------------------------------------*/

void Company::updateFinanceChart(EventData* pData)
{
	FinanceChartSet lSet = static_cast<EventArg<FinanceChartSet>*>(pData)->mData1;

	std::vector<bool> lVisibleSet;

	lVisibleSet.push_back(lSet.mExpenses);
	lVisibleSet.push_back(lSet.mIncome);
	lVisibleSet.push_back(lSet.mFunds);
	lVisibleSet.push_back(lSet.mNetWorth);

	mFinanceChart->setValuesVisible(lVisibleSet);
	mFinanceChart->forceRepaint();
}

/*-----------------------------------------------------------------------------------------------*/

void Company::weekPassed(EventData* pData)
{
	handleAdCampaigns();
	handleCustomers();
	handleExecutingOffers();
	handleSales();
	handleRunningCosts();
	handleAccidents();
	updateResearch();

	EventHandler::raiseEvent(eUpdateFinanceWindow);

	std::vector<int> lValuesFinance;

	lValuesFinance.push_back(mExpenses*8*-1);
	lValuesFinance.push_back(mIncome*8);
	lValuesFinance.push_back(mMoney);
	lValuesFinance.push_back(mNetWorth);

	mFinanceChart->addValueSet(lValuesFinance);

	std::vector<int> lValuesImage;

	lValuesImage.push_back((int)mGlobalImage);
	lValuesImage.push_back(mCompleteCustomers);

	mImageChart->addValueSet(lValuesImage);

	mIncome = 0;
	mExpenses = 0;

	handleMissionGoals();

	if (mGasLastWeek > mGas)
		mGasTrend = 1;
	else if (mGasLastWeek < mGas)
		mGasTrend = -1;
	else
		mGasTrend = 0;

	if (mCoalLastWeek > mCoal)
		mCoalTrend = 1;
	else if (mCoalLastWeek < mCoal)
		mCoalTrend = -1;
	else
		mCoalTrend = 0;

	if (mUraniumLastWeek > mUranium)
		mUraniumTrend = 1;
	else if (mUraniumLastWeek < mUranium)
		mUraniumTrend = -1;
	else
		mUraniumTrend = 0;

	EventHandler::raiseEvent(eUpdateResourceTrend);

	mGasLastWeek = mGas;
	mCoalLastWeek = mCoal;
	mUraniumLastWeek = mUranium;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<int> Company::getResourceTrends(void)
{
	std::vector<int> lTrends;

	lTrends.push_back(mCoalTrend);
	lTrends.push_back(mGasTrend);
	lTrends.push_back(mUraniumTrend);

	return lTrends;
}

/*-----------------------------------------------------------------------------------------------*/

void Company::handleRunningCosts(void)
{
	std::vector<boost::shared_ptr<Powerplant> > lPowerplants = getAllPowerplants();

	int lRunningCosts = 0;
	int lEnvironmentalCosts = 0;
	int lMaintenanceCost = 0;

	int lTrainingCosts = 0;
	int lSafetyCosts = 0;

	if (mTraining == eNoTraining)
		lTrainingCosts += ((lPowerplants.size()+mResourceBuildings.size())
		*GameConfig::getInt("NoTrainingCostFactor"));
	else if (mTraining == eNormalTraining)
		lTrainingCosts += ((lPowerplants.size()+mResourceBuildings.size())
		*GameConfig::getInt("NormalTrainingCostFactor"));
	else if (mTraining == eExtensiveTraining)
		lTrainingCosts += ((lPowerplants.size()+mResourceBuildings.size())
		*GameConfig::getInt("ExtensiveTrainingCostFactor"));

	if (mSafety == eNoSafety)
		lSafetyCosts += ((lPowerplants.size()+mResourceBuildings.size())
		*GameConfig::getInt("NoSafetyCostFactor"));
	else if (mSafety == eNormalSafety)
		lSafetyCosts += ((lPowerplants.size()+mResourceBuildings.size())
		*GameConfig::getInt("NormalSafetyCostFactor"));
	else if (mSafety == eExtensiveSafety)
		lSafetyCosts += ((lPowerplants.size()+mResourceBuildings.size())
		*GameConfig::getInt("ExtensiveSafetyCostFactor"));

	double lMaintenanceCostFactor = GameConfig::getDouble("MaintenanceCostFactor");
	double lRunningCostFactor = GameConfig::getDouble("RunningCostFactor");
	double lEnvCostFactor = GameConfig::getDouble("EnvCostFactor");

	for (size_t i = 0; i < lPowerplants.size(); ++i) {
		lRunningCosts += lPowerplants[i]->getOperatingCosts()*lRunningCostFactor;
		lEnvironmentalCosts += lPowerplants[i]->getEnvironmentalCosts()*lEnvCostFactor;

		if (lPowerplants[i]->getInheritSettings())
			lMaintenanceCost += (lPowerplants[i]->
			getOperatingCosts()*0.5*(mGlobalMaintenance/100))*lMaintenanceCostFactor;
		else
			lMaintenanceCost += (lPowerplants[i]->
			getOperatingCosts()*0.5*((float)lPowerplants[i]->getMaintenance()/100.0))
      *lMaintenanceCostFactor;
	}

	for (size_t j = 0; j < mResourceBuildings.size(); ++j) {
		lRunningCosts += mResourceBuildings[j]->getOperatingCosts()*lRunningCostFactor;
		lEnvironmentalCosts += mResourceBuildings[j]->getEnvironmentalCosts()*lEnvCostFactor;

		if(mResourceBuildings[j]->getInheritSettings())
			lMaintenanceCost += (mResourceBuildings[j]->
			getOperatingCosts()*0.5*(mGlobalMaintenance/100))*lMaintenanceCostFactor;
		else
			lMaintenanceCost += (mResourceBuildings[j]->
			getOperatingCosts()*0.5*((float)mResourceBuildings[j]->
			getMaintenance()/100.0))*lMaintenanceCostFactor;
	}

	lMaintenanceCost += (mPowerNet->getLargePoleCount()*300*
		((float)mSupplyMaintenance/100.0))*lMaintenanceCostFactor;
	lMaintenanceCost += (mPowerNet->getSmallPoleCount()*150*
		((float)mSupplyMaintenance/100.0))*lMaintenanceCostFactor;

	if (mNoTrainingActive)
		lRunningCosts *= GameConfig::getDouble("NoTrainingCostReduction");

	if (mWasteActive)
		lRunningCosts *= GameConfig::getDouble("WasteDumpingCostReduction");

	if (mBribeActive)
		lEnvironmentalCosts *= GameConfig::getDouble("BribeCostReduction");

	if (mResearchSet.mResearched[11])
		lRunningCosts *= GameConfig::getDouble("ResearchedRunningCost");

	if (mResearchSet.mResearched[13])
		lEnvironmentalCosts *= GameConfig::getDouble("CO2ReductionEnvCost");

	mMoney -= lRunningCosts;
	mMoney -= lEnvironmentalCosts;
	mMoney -= lMaintenanceCost;
	mMoney -= (lTrainingCosts+lSafetyCosts);

	mExpenses -= lRunningCosts;
	mExpenses -= lEnvironmentalCosts;
	mExpenses -= lMaintenanceCost;
	mExpenses -= (lTrainingCosts+lSafetyCosts);

	mCurrentMonth.mOperation -= lRunningCosts;
	mCurrentYear.mOperation -= lRunningCosts;

	mCurrentMonth.mEnvironmental -= lEnvironmentalCosts;
	mCurrentYear.mEnvironmental -= lEnvironmentalCosts;

	mCurrentMonth.mMaintenance -= lMaintenanceCost;
	mCurrentYear.mMaintenance -= lMaintenanceCost;

	mCurrentMonth.mTrainingSafety -= (lTrainingCosts+lSafetyCosts);
	mCurrentYear.mTrainingSafety -= (lTrainingCosts+lSafetyCosts);
}

/*-----------------------------------------------------------------------------------------------*/

void Company::handleSales(void)
{
	int lAdvertisingThreshold = GameConfig::getDouble("AdvertisingThreshold");

	if (mResearchSet.mResearched[2])
		lAdvertisingThreshold = GameConfig::getDouble("AdvertisingResearchedThreshold");

	mGlobalImage += ((float)mGlobalAdvertising - lAdvertisingThreshold) / 20.0;

	if (mAdBoost != 0)
		mGlobalImage += mAdBoost;

	int lSafetyFactor = 0;

	if (mTraining == eNoTraining)
		lSafetyFactor += GameConfig::getInt("SafetyFactorNoTraining");
	else if (mTraining == eNormalTraining)
		lSafetyFactor += GameConfig::getInt("SafetyFactorNormalTraining");
	else if (mTraining == eExtensiveTraining)
		lSafetyFactor += GameConfig::getInt("SafetyFactorExtensiveTraining");

	if (mSafety == eNoSafety)
		lSafetyFactor += GameConfig::getInt("SafetyFactorNoSafety");
	else if (mSafety == eNormalSafety)
		lSafetyFactor += GameConfig::getInt("SafetyFactorNormalSafety");
	else if (mSafety == eExtensiveSafety)
		lSafetyFactor += GameConfig::getInt("SafetyFactorExtensiveSafety");

	// compute accidents by safety factor

	if (mCustomerSupport == eNoSupport)
		mGlobalImage += GameConfig::getInt("NoSupportImageInf");
	else if (mCustomerSupport == eExtensiveSupport)
		mGlobalImage += GameConfig::getInt("ExtensvieSupportImageInf");

	if (mContractTime == eNoTime)
		mGlobalImage += GameConfig::getInt("NoTimeImageInf");
	else if (mContractTime == e24)
		mGlobalImage += GameConfig::getInt("24TimeImageInf");

	double lEnvSupportInf = GameConfig::getDouble("EnvSupportImageInf");

	mGlobalImage += mEnvSupport / 40 * lEnvSupportInf;

	// buildings image influence
	{
		double lBuildingImageScaleFactor = GameConfig::getDouble("BuildingImageScaleFactor");

		std::vector<boost::shared_ptr<Powerplant> > lPowerplants = getAllPowerplants();
		std::vector<boost::shared_ptr<ResourceBuilding> > lResourceBuildings = getResourceBuildings();

		for(size_t i = 0; i < lPowerplants.size(); ++i)
			mGlobalImage += GameConfig::getDouble(GameConfig::cCOTS(lPowerplants[i]->getSubtype()),
      cCOVImage)/lBuildingImageScaleFactor;

		for(size_t j = 0; j < lResourceBuildings.size(); ++j)
			mGlobalImage += GameConfig::getDouble(GameConfig::cCOTS(lResourceBuildings[j]->getSubtype()),
      cCOVImage)/lBuildingImageScaleFactor;

		if(mResearchBuilt)
			mGlobalImage += GameConfig::getDouble("COResearch_Image")/lBuildingImageScaleFactor;

		if(mPRBuilt)
			mGlobalImage += GameConfig::getDouble("COPublicRelations_Image")/lBuildingImageScaleFactor;

		if(mHeadquartersBuilt)
			mGlobalImage += GameConfig::getDouble("COHeadquarters_Image")/lBuildingImageScaleFactor;
	}

	if(mGlobalImage < 0.0)
		mGlobalImage = 0.0;
	if(mGlobalImage > 100.0)
		mGlobalImage = 100.0;

	mMoney -= mGlobalAdvertising * mConnectedCities.size();
	mCurrentYear.mAdvertising -=  mGlobalAdvertising * mConnectedCities.size();
	mCurrentMonth.mAdvertising -=  mGlobalAdvertising * mConnectedCities.size();
	mExpenses -= mGlobalAdvertising * mConnectedCities.size();
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<boost::shared_ptr<City> > Company::getConnectedCities(bool pForceUpdate)
{
	std::vector<boost::shared_ptr<City> > lConnectedCities;

	if (pForceUpdate) {
		for (size_t j = 0; j < mPowerNet->getSubnetCount(); j++) {
			std::vector<boost::shared_ptr<City> > lSubnetCities = mPowerNet->getSubnetCities(j);
			for (size_t k = 0; k < lSubnetCities.size(); k++)
				lConnectedCities.push_back(lSubnetCities[k]);
		}
		mConnectedCities = lConnectedCities;
	} else {
		lConnectedCities = mConnectedCities;
	}

	return lConnectedCities;
}

/*-----------------------------------------------------------------------------------------------*/

void Company::handleCustomers(void)
{
	EventHandler::raiseEvent(eUpdatePowerNet);

	mCompleteCustomers = 0;

	std::vector<boost::shared_ptr<City> > lConnectedCities = getConnectedCities(true);

	for (size_t i = 0; i < lConnectedCities.size(); i++) {
		int lMaxCustomers = lConnectedCities[i]->getResidentCount();
		int lCurrentCustomers = lConnectedCities[i]->getCustomers();
		int lPossibleCustomers = lMaxCustomers - lCurrentCustomers;

		int lNewCustomers = 0;

		double lImageScaleFactor = GameConfig::getDouble("ImageScaleFactor");
		double lPriceScaleFactor = GameConfig::getDouble("PriceScaleFactor");
		double lAdvertisingScaleFactor = GameConfig::getDouble("AdvertisingScaleFactor");
		double lPowerScaleFactor = GameConfig::getDouble("PowerScaleFactor");

		float lImageScaler = lImageScaleFactor * ((mGlobalImage - 25.0)) / 100 * 0.002;
		float lPriceScaler = lPriceScaleFactor * ((mGlobalPrice - 20.0) * -1) / 100 * 0.004;
		float lAdvertisingScaler = lAdvertisingScaleFactor * ((mGlobalAdvertising) / 100 * 0.002);
		float lPowerScaler = 0;

		if (lConnectedCities[i]->getCustomers() > 0)
			if (lConnectedCities[i]->getConsumption() > lConnectedCities[i]->getCurrentSupply())
				lPowerScaler = -0.004 * lPowerScaleFactor;

		if ((lImageScaler*lPriceScaler*lAdvertisingScaler*lPowerScaler) < 0.0) {
			lNewCustomers =   (lImageScaler * (float)lCurrentCustomers)
							+ (lPriceScaler * (float)lCurrentCustomers)
							+ (lAdvertisingScaler * (float)lCurrentCustomers)
							+ (lPowerScaler	* (float)lCurrentCustomers);
		} else {
			lNewCustomers =   (lImageScaler * (float)lPossibleCustomers)
							+ (lPriceScaler * (float)lPossibleCustomers)
							+ (lAdvertisingScaler * (float)lPossibleCustomers)
							+ (lPowerScaler * (float)lPossibleCustomers);
		}

		if ((lCurrentCustomers + lNewCustomers) >= 0)
			lConnectedCities[i]->setCustomers(lCurrentCustomers + lNewCustomers);
		else
			lConnectedCities[i]->setCustomers(0);

		mCompleteCustomers += lConnectedCities[i]->getCustomers();
	}

	float lPriceRigging = 1.0;
	
	if (mPriceActive)
		lPriceRigging = GameConfig::getDouble("PriceRiggingIncomeBoost");

	float lIncomeMultiplicator = GameConfig::getDouble("DefaultIncomeMultiplicator");

	if (mCustomerSupport == eNoSupport)
		lIncomeMultiplicator = GameConfig::getDouble("NoSupportIncomeFactor");
	else if (mCustomerSupport == eNormalSupport)
		lIncomeMultiplicator = GameConfig::getDouble("NormalSupportIncomeFactor");
	else if (mCustomerSupport == eExtensiveSupport)
		lIncomeMultiplicator = GameConfig::getDouble("ExtensiveSupportIncomeFactor");

	if (mContractTime == eNoTime)
		lIncomeMultiplicator *= GameConfig::getDouble("PriceRiggingIncomeBoost");
	else if (mContractTime == e12)
		lIncomeMultiplicator *= GameConfig::getDouble("12TimeIncomeFactor");
	else if (mContractTime == e24)
		lIncomeMultiplicator *= GameConfig::getDouble("24TimeIncomeFactor");

	mMoney += mGlobalPrice * (((float)mCompleteCustomers 
    * GameConfig::getDouble("AverageCustomerDemand")) 
		/ 1000) * lIncomeMultiplicator * lPriceRigging;
	mCurrentMonth.mCustomerIncome += mGlobalPrice * (((float)mCompleteCustomers 
    * GameConfig::getDouble("AverageCustomerDemand")) 
		/ 1000) * lIncomeMultiplicator * lPriceRigging;
	mCurrentYear.mCustomerIncome += mGlobalPrice * (((float)mCompleteCustomers 
    * GameConfig::getDouble("AverageCustomerDemand")) 
		/ 1000) * lIncomeMultiplicator * lPriceRigging;
	mIncome += mGlobalPrice * (((float)mCompleteCustomers 
    * GameConfig::getDouble("AverageCustomerDemand")) 
		/ 1000) * lIncomeMultiplicator * lPriceRigging;
}

/*-----------------------------------------------------------------------------------------------*/

void Company::handleExecutingOffers(void)
{
	std::vector<TradingOffer>::iterator it = mExecutingOffers.begin();

	bool lCancel = false;
	bool lReport = false;
	bool lMoney = false;

	while (it != mExecutingOffers.end()) {
		EventHandler::raiseEvent(eUpdateObligations);

		if ((*it).mWorldMarket) {
			if ((*it).mResource == eCoal)
				(*it).mFixedPrice = mCoalPrice;
			if ((*it).mResource == eGas)
				(*it).mFixedPrice = mGasPrice;
			if ((*it).mResource == eUranium)
				(*it).mFixedPrice = mUraniumPrice;
		}

		int lCheckFunds = (*it).mFixedPrice;

		if ((*it).mAmount > 0)
			lCheckFunds = (*it).mFixedPrice * -1;
		else if (mWarActive)
			(*it).mFixedPrice = (*it).mFixedPrice * 0.5;

		if ((mMoney - lCheckFunds * 1000) >= 0) {
			mMoney -= lCheckFunds * 1000;
			mCurrentMonth.mTrade -=  lCheckFunds * 1000;
			mCurrentYear.mTrade -=  lCheckFunds * 1000;

			if (lCheckFunds > 0)
				mExpenses -= lCheckFunds * 1000;
			else
				mIncome -= lCheckFunds * 1000;

			if ((*it).mResource == eCoal) {
				if ((mCoal - (*it).mAmount) >= 0) {
					adjustResource((*it).mResource,  (*it).mAmount*-1);
				} else {
					lCancel = true;
					lMoney = false;
					lReport = true;
				}
			}
			if ((*it).mResource == eGas) {
				if ((mGas - (*it).mAmount) >= 0) {
					adjustResource((*it).mResource,  (*it).mAmount*-1);
				} else {
					lCancel = true;
					lMoney = false;
					lReport = true;
				}
			}
			if ((*it).mResource == eUranium) {
				if ((mUranium - (*it).mAmount) >= 0) {
					adjustResource((*it).mResource,  (*it).mAmount*-1);
				} else {
					lCancel = true;
					lMoney = false;
					lReport = true;
				}
			}
		} else {
			lCancel = true;
			lMoney = true;
			lReport = true;
		}

		(*it).mWeeks--;

		if ((((*it).mWeeks <= 0) && ((*it).mPermanent == false)) || (lCancel == true)) {
			if (lReport)
				contractCanceled(lMoney, "", "", "");

			it = mExecutingOffers.erase(it);
		} else {
			it++;
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Company::contractCanceled(bool pMoney, std::string pPartner, std::string pBuySell,
							   std::string pResource)
{
	TickerMessage lMessage;

	if (pMoney) {
		lMessage.mMessage = StrLoc::get()->ContractCanceledMoney();
		lMessage.mDetail = StrLoc::get()->ContractCanceledMoneyDetail();
	} else {
		lMessage.mMessage = StrLoc::get()->ContractCanceledResources();
		lMessage.mDetail = StrLoc::get()->ContractCanceledResourcesDetail();
	}

	EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
	EventHandler::raiseEvent(eUpdateObligations);
}

/*-----------------------------------------------------------------------------------------------*/

void Company::monthPassed(EventData* pData)
{
	handleSpecialActions();
	createNewTradeOffers();
	handleLoans();

	mMoney -= (mEnvSupport * 1000);

	mCurrentMonth.mEnvironmental -= (mEnvSupport * 1000);
	mCurrentYear.mEnvironmental -= (mEnvSupport * 1000);

	mExpenses -= (mEnvSupport * 1000);

	mLastMonth = mCurrentMonth;
	mCurrentMonth = FinancialArchive();

	Weather lNewWeather;

	EventHandler::raiseEvent(eNewWeather, new EventArg<Weather>(lNewWeather));
}

void Company::yearPassed(EventData* pData)
{
	mLastYear = mCurrentYear;
	mCurrentYear = FinancialArchive();

	if ((mBankruptcyWarningIssued) && (mMoney >= GameConfig::getInt("MaxDebtAfterWarning")))
		mBankruptcyWarningIssued = false;

	if (mMoney < GameConfig::getInt("MaxDebtAfterWarning")) {
		if (mBankruptcyWarningIssued) {
			EventHandler::raiseEvent(eBankruptcy);
		} else {
			if (mMoney < GameConfig::getInt("MinDebtForWarning")) {
				EventHandler::raiseEvent(eBankruptcyWarning);
				mBankruptcyWarningIssued = true;
			}
		}
	}

	TickerMessage lMessage;
	lMessage.mMessage = StrLoc::get()->NewYear();
	lMessage.mPointOfInterest = Ogre::Vector2(-1.0, -1.0);
	lMessage.mUrgent = false;
	lMessage.mID = 0;

	EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
}

/*-----------------------------------------------------------------------------------------------*/

std::string TradingOffer::formatted(void)
{
	std::string lFormatted;

	if (mAmount < 0)
		lFormatted += StrLoc::get()->BuyF() + toString(mAmount*-1);
	else
		lFormatted += StrLoc::get()->SellF() + toString(mAmount);

	lFormatted += " " + getResourceUnit(mResource)
			   + " " + resourceString(mResource);

	if (mAmount < 0)
		lFormatted += StrLoc::get()->From();
	else
		lFormatted += StrLoc::get()->To();

	lFormatted += mTradingPartner;

	if (mPermanent) {
		if (!mWorldMarket)
			lFormatted += ", " +  StrLoc::get()->EveryWeek() + " (" 
			+ StrLoc::get()->PriceT() + ": " + toString(mFixedPrice) + L"k€"
			+ StrLoc::get()->TooltipPerWeek() + ")";
		else
			lFormatted += ", " +  StrLoc::get()->EveryWeek() + " (" 
			+ StrLoc::get()->PriceT() + ": " + StrLoc::get()->WorldMarket() 
			+ " - " + toString(mDiscount + 1) + "%)";
	} else {
		if (!mUnique) {
			if(!mWorldMarket) {
				lFormatted += StrLoc::get()->For() + toString(mWeeks) + " " 
					+ StrLoc::get()->Weeks() + " (" + StrLoc::get()->PriceT() 
					+ ": " + toString(mFixedPrice) + L"k€"
					+ StrLoc::get()->TooltipPerWeek() + ")";
			} else {
				if (mDiscount > 20)
					lFormatted +=  StrLoc::get()->For() + toString(mWeeks) 
					+ " " + StrLoc::get()->Weeks() + " (" + StrLoc::get()->PriceT() 
					+ ": " + StrLoc::get()->WorldMarket() + " - " + toString(mDiscount) + "%)";
				else
					lFormatted +=  StrLoc::get()->For() + toString(mWeeks) 
					+ " " + StrLoc::get()->Weeks() + " (" + StrLoc::get()->PriceT() 
					+ ": " + StrLoc::get()->WorldMarket() + ")";
			}
		} else {
			lFormatted += " (" + StrLoc::get()->PriceT() + ": " 
				+ toString(mFixedPrice) + L"k€)";
		}
	}

	return lFormatted;
}

/*-----------------------------------------------------------------------------------------------*/

std::string TradingOffer::formattedCurrent(void)
{
	std::string lFormatted;

	if (mAmount < 0)
		lFormatted += StrLoc::get()->BuyF() + toString(mAmount*-1);
	else
		lFormatted += StrLoc::get()->SellF() + toString(mAmount);

	lFormatted += " " + getResourceUnit(mResource)
		+ " " + resourceString(mResource);

	if (mAmount < 0)
		lFormatted += StrLoc::get()->From();
	else
		lFormatted += StrLoc::get()->To();

	lFormatted += mTradingPartner;

	if (mPermanent) {
		if (!mWorldMarket)
			lFormatted += " (" + StrLoc::get()->EveryWeekS() + ", " 
			+ StrLoc::get()->PriceT() + ": " + toString(mFixedPrice) + L"k€"
			+ StrLoc::get()->TooltipPerWeek() + ")";
		else
			lFormatted += " (" + StrLoc::get()->EveryWeekS() + ", " 
			+ StrLoc::get()->PriceT() + ": " + StrLoc::get()->WorldMarket() 
			+ " - " + toString(mDiscount + 1) + "%)";
	}
	else {
		if (!mWorldMarket)
			lFormatted += " (" + toString(mWeeks) + " " 
			+ StrLoc::get()->WeeksRemaining() + ", " + StrLoc::get()->PriceT() 
			+ ": " + toString(mFixedPrice) + L"k€" + StrLoc::get()->TooltipPerWeek() + ")";
		else
			lFormatted += " (" + toString(mWeeks) + " " 
			+ StrLoc::get()->WeeksRemaining() + ", " + StrLoc::get()->PriceT() 
			+ ": " + StrLoc::get()->WorldMarket() + " - " + toString(mDiscount + 1) + "%)";
	}

	return lFormatted;
}

/*-----------------------------------------------------------------------------------------------*/

void Company::createNewTradeOffers(void)
{
	mTradingOffers.clear();

	for (size_t i = 0; i < mTradingPartners.size(); i++) {
		if (mTradingPartners[i].mAvailable) {
			int lNumberOffers = (rand() % 4);

			for (int j = 0; j < lNumberOffers; j++) {
				TradingOffer lNewOffer;
				lNewOffer.mTradingPartner = mTradingPartners[i].mName;
				lNewOffer.mWorldMarket = randPercent(GameConfig::getDouble("WorldMarketProbability"));
				lNewOffer.mUnique = randPercent(GameConfig::getDouble("UniqueProbability"));
				lNewOffer.mDiscount = rand() % 51;
				lNewOffer.mResource = (eResourceType)(rand() % 3);
				lNewOffer.mWeeks = rand() % 27;

				int lPermanent = rand() % 5;

				if (lPermanent == 4)
					lNewOffer.mPermanent = true;
				else
					lNewOffer.mPermanent = false;


				if (lNewOffer.mUnique)
					lNewOffer.mAmount = (rand() % 5000) - 2500;
				else
					lNewOffer.mAmount = (rand() % 500) - 250;

				if (lNewOffer.mPermanent)
					lNewOffer.mAmount = (rand() % 500) - 250;

				if ((lNewOffer.mAmount > 100) || (lNewOffer.mAmount < -100)) {
					double lTradePriceFactor = GameConfig::getDouble("TradePriceFactor");
					lNewOffer.mFixedPrice = (rand() % (lNewOffer.mAmount * 2)) * lTradePriceFactor;

					if (!lNewOffer.mUnique) {
						if (lNewOffer.mWeeks > 3) {
							mTradingOffers.push_back(lNewOffer);
						}
					} else {
						mTradingOffers.push_back(lNewOffer);
					}
				}
			}
		}
	}

	EventHandler::raiseEvent(eUpdateTradeWindow);
}

/*-----------------------------------------------------------------------------------------------*/

bool Company::acceptTradingOffer(std::string pName)
{
	int lIndex;

	for (size_t i = 0; i < mTradingOffers.size(); i++) {
		if (pName == mTradingOffers[i].formatted()) {
			lIndex = i;
			break;
		}
	}

	if ((mTradingOffers[lIndex].mUnique) && (!mTradingOffers[lIndex].mPermanent)) {
		if (mTradingOffers[lIndex].mAmount > 0) {
			mTradingOffers[lIndex].mFixedPrice *= -1;
		} else {
			if (mTaxFreeTrades > 0) {
				mTradingOffers[lIndex].mFixedPrice 
					= mTradingOffers[lIndex].mFixedPrice * 0.5;

				mTaxFreeTrades--;

				TickerMessage lMessage;

				if (mTaxFreeTrades > 1)
					lMessage.mMessage = toString(mTaxFreeTrades) + StrLoc::get()->FreeTradesLeft();
				else if (mTaxFreeTrades == 1)
					lMessage.mMessage = toString(mTaxFreeTrades) + StrLoc::get()->FreeTradesLeft();
				else
					lMessage.mMessage = StrLoc::get()->FreeTradesGone();

				lMessage.mPointOfInterest = Ogre::Vector2(-1,-1);
				lMessage.mDetail = StrLoc::get()->FreeTradesDetail();
				lMessage.mUrgent = false;

				EventHandler::raiseEvent(eTickerMessage, 
					new EventArg<TickerMessage>(lMessage));
			}

			if (mWarActive)
				mTradingOffers[lIndex].mFixedPrice = mTradingOffers[lIndex].mFixedPrice * 0.5;
		}

		if ((mMoney - mTradingOffers[lIndex].mFixedPrice * 1000) >= 0) {
			mMoney -= mTradingOffers[lIndex].mFixedPrice * 1000;
			mCurrentYear.mTrade -= mTradingOffers[lIndex].mFixedPrice * 1000;
			mCurrentMonth.mTrade -= mTradingOffers[lIndex].mFixedPrice * 1000;

			if (mTradingOffers[lIndex].mFixedPrice > 0)
				mExpenses -= mTradingOffers[lIndex].mFixedPrice * 1000;
			else
				mIncome -= mTradingOffers[lIndex].mFixedPrice * 1000;

			if (mTradingOffers[lIndex].mResource == eCoal)
				if ((mCoal - mTradingOffers[lIndex].mAmount) >= 0)
					adjustResource(mTradingOffers[lIndex].mResource,
					mTradingOffers[lIndex].mAmount*-1);
				else
					return false;
			if (mTradingOffers[lIndex].mResource == eGas)
				if ((mGas - mTradingOffers[lIndex].mAmount) >= 0)
					adjustResource(mTradingOffers[lIndex].mResource,
					mTradingOffers[lIndex].mAmount*-1);
				else
					return false;
			if (mTradingOffers[lIndex].mResource == eUranium)
				if ((mUranium - mTradingOffers[lIndex].mAmount) >= 0)
					adjustResource(mTradingOffers[lIndex].mResource,
					mTradingOffers[lIndex].mAmount*-1);
				else
					return false;

			mTradingOffers.erase(mTradingOffers.begin() + lIndex);

			return true;
		}
	} else {
		mExecutingOffers.push_back(mTradingOffers[lIndex]);
		mTradingOffers.erase(mTradingOffers.begin() + lIndex);
		return true;
	}

	return false;
}

/*-----------------------------------------------------------------------------------------------*/

bool Company::buyTradingPartner(std::string pName)
{
	for (size_t i = 0; i < mTradingPartners.size(); i++) {
		if (mTradingPartners[i].mName.find(pName) != -1) {
			if (mMoney >= (mTradingPartners[i].mPrice*1000)) {
				mMoney -= mTradingPartners[i].mPrice*1000;
				mCurrentMonth.mTrade -= mTradingPartners[i].mPrice*1000;
				mCurrentYear.mTrade -= mTradingPartners[i].mPrice*1000;
				mTradingPartners[i].mAvailable = true;
				mExpenses -= mTradingPartners[i].mPrice*1000;
				return true;
			}
		}
	}

	return false;
}

/*-----------------------------------------------------------------------------------------------*/

void Company::addCompanyBuilding(boost::shared_ptr<CompanyBuilding> pBuilding)
{
	mCompanyBuildings.push_back(pBuilding);
}

/*-----------------------------------------------------------------------------------------------*/

void Company::updateAnimations(float pElapsedTime)
{
	for (size_t j = 0; j < mPowerNet->getSubnetCount(); j++) {
		std::vector<boost::shared_ptr<Powerplant> > lSubnetPowerplants = mPowerNet->
			getSubnetPowerplants(j);

		for (size_t k = 0; k < lSubnetPowerplants.size(); k++)
			lSubnetPowerplants[k]->updateAnimations(pElapsedTime);
	}
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<boost::shared_ptr<Powerplant> > Company::getAllPowerplants(void)
{
	std::vector<boost::shared_ptr<Powerplant> > lReturn;

	for (size_t j = 0; j < mPowerNet->getSubnetCount(); j++) {
		std::vector<boost::shared_ptr<Powerplant> > lSubnetPowerplants = mPowerNet->
			getSubnetPowerplants(j);

		for (size_t k = 0; k < lSubnetPowerplants.size(); k++)
			lReturn.push_back(lSubnetPowerplants[k]);
	}
	return lReturn;
}

/*-----------------------------------------------------------------------------------------------*/

void Company::handleLoans(void)
{
	std::vector<Loan>::iterator it = mCurrentLoans.begin();

	while (it != mCurrentLoans.end()) {
		(*it).mMonthLeft--;
		(*it).mAmountLeft -= (*it).mMonthlyRate;
		mMoney -= (*it).mMonthlyRate;
		mCurrentMonth.mLoanPayments -= (*it).mMonthlyRate;
		mCurrentYear.mLoanPayments -= (*it).mMonthlyRate;
		mExpenses -= (*it).mMonthlyRate;

		if ((*it).mMonthLeft <= 0)
			it = mCurrentLoans.erase(it);
		else
			it++;
	}

	EventHandler::raiseEvent(eUpdateLoans);
}

/*-----------------------------------------------------------------------------------------------*/

bool Company::takeOutLoan(Loan pLoan)
{
	mMoney += pLoan.mAmountLeft;

	pLoan.mAmountLeft = pLoan.mMonthLeft * pLoan.mMonthlyRate;

	mCurrentLoans.push_back(pLoan);
	return true;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<Loan> Company::getLoans(void)
{
	return mCurrentLoans;
}

/*-----------------------------------------------------------------------------------------------*/

bool Company::unscheduledRepayment(int pLoanIndex, int pAmount)
{
	if (mMoney >= pAmount) {
		mMoney -= pAmount;
		mExpenses -= pAmount;
		mCurrentMonth.mLoanPayments -= pAmount;
		mCurrentYear.mLoanPayments -= pAmount;

		mCurrentLoans[pLoanIndex].mAmountLeft -= pAmount;
		mCurrentLoans[pLoanIndex].mMonthlyRate = mCurrentLoans[pLoanIndex].mAmountLeft
			/ mCurrentLoans[pLoanIndex].mMonthLeft;

		if (mCurrentLoans[pLoanIndex].mAmountLeft <= 1001)
			mCurrentLoans.erase(mCurrentLoans.begin() + pLoanIndex);

		return true;
	}

	return false;
}

/*-----------------------------------------------------------------------------------------------*/

std::string Loan::formatted(void)
{
	return StrLoc::get()->LoanFormattedA()
		+ toString(mAmountLeft/1000)
		+ StrLoc::get()->LoanFormattedB()
		+ toString(mMonthlyRate/1000)
		+ StrLoc::get()->LoanFormattedC()
		+ toString(mMonthLeft)
		+ StrLoc::get()->LoanFormattedD();
}

/*-----------------------------------------------------------------------------------------------*/

ResearchSet::ResearchSet(void)
{
	for (int i = 0; i < 21; i ++) {
		mStarted[i] = false;
		mResearched[i] = false;
	}

	mName[0] = StrLoc::get()->Re1(); mPrice[0] = GameConfig::getInt("Re0Price"); mWeeks[0] = GameConfig::getInt("Re0Duration");
	mResearched[0] = false; mStarted[0] = false; mWeeksLeft[0] = 0; mPrerequesit[0] = -1;
	mName[1] = StrLoc::get()->Re2(); mPrice[1] = GameConfig::getInt("Re1Price"); mWeeks[1] = GameConfig::getInt("Re1Duration");
	mResearched[1] = false; mStarted[1] = false; mWeeksLeft[1] = 0; mPrerequesit[1] = -1;
	mName[2] = StrLoc::get()->Re3(); mPrice[2] = GameConfig::getInt("Re2Price"); mWeeks[2] = GameConfig::getInt("Re2Duration");
	mResearched[2] = false; mStarted[2] = false; mWeeksLeft[2] = 0; mPrerequesit[2] = -1;
	mName[3] = StrLoc::get()->Re4(); mPrice[3] = GameConfig::getInt("Re3Price"); mWeeks[3] = GameConfig::getInt("Re3Duration");
	mResearched[3] = false; mStarted[3] = false; mWeeksLeft[3] = 0; mPrerequesit[3] = 1;
	mName[4] = StrLoc::get()->Re5(); mPrice[4] = GameConfig::getInt("Re4Price"); mWeeks[4] = GameConfig::getInt("Re4Duration");
	mResearched[4] = false; mStarted[4] = false; mWeeksLeft[4] = 0; mPrerequesit[4] = 6;
	mName[5] = StrLoc::get()->Re6(); mPrice[5] = GameConfig::getInt("Re5Price"); mWeeks[5] = GameConfig::getInt("Re5Duration");
	mResearched[5] = false; mStarted[5] = false; mWeeksLeft[5] = 0; mPrerequesit[5] = 0;
	mName[6] = StrLoc::get()->Re7(); mPrice[6] = GameConfig::getInt("Re6Price"); mWeeks[6] = GameConfig::getInt("Re6Duration");
	mResearched[6] = false; mStarted[6] = false; mWeeksLeft[6] = 0; mPrerequesit[6] = 1;
	mName[7] = StrLoc::get()->Re8(); mPrice[7] = GameConfig::getInt("Re7Price"); mWeeks[7] = GameConfig::getInt("Re7Duration");
	mResearched[7] = false; mStarted[7] = false; mWeeksLeft[7] = 0; mPrerequesit[7] = 9;
	mName[8] = StrLoc::get()->Re9(); mPrice[8] = GameConfig::getInt("Re8Price"); mWeeks[8] = GameConfig::getInt("Re8Duration");
	mResearched[8] = false; mStarted[8] = false; mWeeksLeft[8] = 0; mPrerequesit[8] = 9;
	mName[9] = StrLoc::get()->Re10(); mPrice[9] = GameConfig::getInt("Re9Price"); mWeeks[9] = GameConfig::getInt("Re9Duration");
	mResearched[9] = false; mStarted[9] = false; mWeeksLeft[9] = 0; mPrerequesit[9] = 0;
	mName[10] = StrLoc::get()->Re11(); mPrice[10] = GameConfig::getInt("Re10Price"); mWeeks[10] = GameConfig::getInt("Re10Duration");
	mResearched[10] = false; mStarted[10] = false; mWeeksLeft[10] = 0; mPrerequesit[10] = 0;
	mName[11] = StrLoc::get()->Re12(); mPrice[11] = GameConfig::getInt("Re11Price"); mWeeks[11] = GameConfig::getInt("Re11Duration");
	mResearched[11] = false; mStarted[11] = false; mWeeksLeft[11] = 0; mPrerequesit[11] = 2;
	mName[12] = StrLoc::get()->Re13(); mPrice[12] = GameConfig::getInt("Re12Price"); mWeeks[12] = GameConfig::getInt("Re12Duration");
	mResearched[12] = false; mStarted[12] = false; mWeeksLeft[12] = 0; mPrerequesit[12] = 9;
	mName[13] = StrLoc::get()->Re14(); mPrice[13] = GameConfig::getInt("Re13Price"); mWeeks[13] = GameConfig::getInt("Re13Duration");
	mResearched[13] = false; mStarted[13] = false; mWeeksLeft[13] = 0; mPrerequesit[13] = 11;
	mName[14] = StrLoc::get()->Re15(); mPrice[14] = GameConfig::getInt("Re14Price"); mWeeks[14] = GameConfig::getInt("Re14Duration");
	mResearched[14] = false; mStarted[14] = false; mWeeksLeft[14] = 0; mPrerequesit[14] = 18;
	mName[15] = StrLoc::get()->Re16(); mPrice[15] = GameConfig::getInt("Re15Price"); mWeeks[15] = GameConfig::getInt("Re15Duration");
	mResearched[15] = false; mStarted[15] = false; mWeeksLeft[15] = 0; mPrerequesit[15] = 3;
	mName[16] = StrLoc::get()->Re17(); mPrice[16] = GameConfig::getInt("Re16Price"); mWeeks[16] = GameConfig::getInt("Re16Duration");
	mResearched[16] = false; mStarted[16] = false; mWeeksLeft[16] = 0; mPrerequesit[16] = 15;
	mName[17] = StrLoc::get()->Re18(); mPrice[17] = GameConfig::getInt("Re17Price"); mWeeks[17] = GameConfig::getInt("Re17Duration");
	mResearched[17] = false; mStarted[17] = false; mWeeksLeft[17] = 0; mPrerequesit[17] = 15;
	mName[18] = StrLoc::get()->Re19(); mPrice[18] = GameConfig::getInt("Re18Price"); mWeeks[18] = GameConfig::getInt("Re18Duration");
	mResearched[18] = false; mStarted[18] = false; mWeeksLeft[18] = 0; mPrerequesit[18] = 8;
	mName[19] = StrLoc::get()->Re20(); mPrice[19] = GameConfig::getInt("Re19Price"); mWeeks[19] = GameConfig::getInt("Re19Duration");
	mResearched[19] = false; mStarted[19] = false; mWeeksLeft[19] = 0; mPrerequesit[19] = 18;
	mName[20] = StrLoc::get()->Re21(); mPrice[20] = GameConfig::getInt("Re20Price"); mWeeks[20] = GameConfig::getInt("Re20Duration");
	mResearched[20] = false; mStarted[20] = false; mWeeksLeft[20] = 0; mPrerequesit[20] = 18;
}


/*-----------------------------------------------------------------------------------------------*/

std::string Company::getCurrentResearch(int &oPercentageCompleted)
{
	for (int i = 0; i < 21; i++) {
		if (mResearchSet.mStarted[i] == true) {
			oPercentageCompleted = ((float)(mResearchSet.mWeeks[i] 
			- mResearchSet.mWeeksLeft[i])/(float)mResearchSet.mWeeks[i])*99;
			return mResearchSet.mName[i];
		}
	}

	oPercentageCompleted = 0;
	return StrLoc::get()->Nothing();
}

/*-----------------------------------------------------------------------------------------------*/

bool Company::isAvailableInTree(size_t pResearchIndex)
{
	if (mResearchSet.mPrerequesit[pResearchIndex] == -1) {
		return true;
	} else {
		if (mResearchSet.mResearched[mResearchSet.mPrerequesit[pResearchIndex]])
			return true;
		else
			return false;
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Company::handleAdCampaigns(void)
{
	mAdBoost = 0;

	std::vector<AdCampaign>::iterator it = mCampaigns.begin();

	while (it != mCampaigns.end()) {
		(*it).mWeeksLeft--;

		if ((*it).mType == eInternet)
			mAdBoost += GameConfig::getDouble("ImageBoostInternetAd");
		else if ((*it).mType == eNewspaper)
			mAdBoost += GameConfig::getDouble("ImageBoostNewspaperAd");
		else if ((*it).mType == eRadio)
			mAdBoost += GameConfig::getDouble("ImageBoostRadioAd");
		else if ((*it).mType == eTV)
			mAdBoost += GameConfig::getDouble("ImageBoostTVAd");

		if ((*it).mWeeksLeft <= 0)
			it = mCampaigns.erase(it);
		else
			it++;
	}
}

/*-----------------------------------------------------------------------------------------------*/

bool Company::startResearch(int pIndex)
{
	if (mMoney >= mResearchSet.mPrice[pIndex]*1000) {
		mMoney -= mResearchSet.mPrice[pIndex]*1000;
		mCurrentMonth.mResearch -= mResearchSet.mPrice[pIndex]*1000;
		mCurrentYear.mResearch -= mResearchSet.mPrice[pIndex]*1000;

		mResearchSet.mStarted[pIndex] = true;
		mResearchSet.mWeeksLeft[pIndex] = mResearchSet.mWeeks[pIndex];

		TickerMessage lMessage;
		lMessage.mMessage = StrLoc::get()->ResearchStartedA() 
			+ mResearchSet.mName[pIndex] + StrLoc::get()->ResearchStartedB();
		lMessage.mPointOfInterest = Ogre::Vector2(-1,-1);
		lMessage.mUrgent = false;

		EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));

		return true;
	} else {
		return false;
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Company::updateResearch(void)
{
	for (int i = 0; i < 21; i++) {
		if (mResearchSet.mStarted[i] == true) {
			mResearchSet.mWeeksLeft[i]--;

			if(mResearchSet.mWeeksLeft[i] <= 0) {
				mResearchSet.mResearched[i] = true;
				mResearchSet.mStarted[i] = false;

				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->ResearchFinishedA() 
					+ mResearchSet.mName[i] + StrLoc::get()->ResearchFinishedB();
				lMessage.mPointOfInterest = Ogre::Vector2(-1, -1);
				lMessage.mUrgent = false;

				if(i == 20)
					EventHandler::raiseEvent(eShowNewspaper, 
						new EventArg<std::string>("np_fusion.png", 
						StrLoc::get()->NPFusion(), mName + " " 
						+ StrLoc::get()->NPFusionDetail()));

				EventHandler::raiseEvent(eTickerMessage, 
					new EventArg<TickerMessage>(lMessage));
				EventHandler::raiseEvent(eUpdateResearchableButtons);
				EventHandler::raiseEvent(eUpdateCOBuildingButtons);
			}
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

bool Company::runSpecialAction(eSpecialAction pType)
{
	int lPrice = 0;
	SpecialAction lSpecialAction;

	lSpecialAction.mType = pType;

	enum eSpecialAction { eBribe, eWar, ePrice, eEmployee, eTaxfraud, eWaste };

	if (pType == eBribe) {
		lSpecialAction.mDone = false;
		lSpecialAction.mMonthLeft = 8;
		lSpecialAction.mTradesLeft = 1;
		lPrice = GameConfig::getInt("PriceBribe");
	}
	else if (pType == ePrice) {
		lSpecialAction.mDone = false;
		lSpecialAction.mMonthLeft = 6;
		lSpecialAction.mTradesLeft = 0;
		lPrice = GameConfig::getInt("PricePriceRigging");
	}
	else if (pType == eWar) {
		lSpecialAction.mDone = false;
		lSpecialAction.mMonthLeft = 12;
		lSpecialAction.mTradesLeft = 0;
		lPrice = GameConfig::getInt("PriceWarSupport");
	}
	else if (pType == eEmployee) {
		lSpecialAction.mDone = false;
		lSpecialAction.mMonthLeft = 6;
		lSpecialAction.mTradesLeft = 0;
		lPrice = GameConfig::getInt("PriceNoTraining");
	}
	else if (pType == eWaste) {
		lSpecialAction.mDone = false;
		lSpecialAction.mMonthLeft = 6;
		lSpecialAction.mTradesLeft = 0;
		lPrice = GameConfig::getInt("PriceWasteDumping");
	}
	else if (pType == eTaxfraud) {
		lPrice = GameConfig::getInt("PriceTaxFraud");
	}

	if(mMoney >= lPrice) {
		mMoney -= lPrice;
		mCurrentMonth.mSpecialExpenses -= lPrice;
		mCurrentYear.mSpecialExpenses -= lPrice;
	} else {
		return false;
	}

	if (pType == eTaxfraud)
		mTaxFreeTrades += 4;
	else
		mSpecialActions.push_back(lSpecialAction);

	return true;

	EventHandler::raiseEvent(eUpdateSpecialActions);
}

/*-----------------------------------------------------------------------------------------------*/

void Company::handleSpecialActions(void)
{
	std::vector<SpecialAction>::iterator it = mSpecialActions.begin();

	while (it != mSpecialActions.end()) {
		if ((*it).mType == eBribe) {
			(*it).mMonthLeft--;

			if ((*it).mTradesLeft == 1) // running
				mBribeActive = true;

			if ((*it).mMonthLeft <= 0) {
				if ((*it).mTradesLeft == 1) {
					(*it).mTradesLeft = 0;
					(*it).mMonthLeft = 48;

					if (randPercent(GameConfig::getDouble("ExposeProbBribe"))) {
						EventHandler::raiseEvent(eShowNewspaper, 
							new EventArg<std::string>("np_bribe.png", 
							StrLoc::get()->NPBribe(), mName + " " 
							+ StrLoc::get()->NPBribeDetail()));

						if (mGlobalImage >= GameConfig::getInt("ImageHitBribe"))
							mGlobalImage -= GameConfig::getInt("ImageHitBribe");
						else
							mGlobalImage = 0;

						(*it).mDone = true;
					}
				} else {
					(*it).mDone = true;
				}
			}
		}
		else if ((*it).mType == ePrice) {
			(*it).mMonthLeft--;

			if ((*it).mMonthLeft <= 0) {
				mPriceActive = false;
				(*it).mDone = true;
			} else {
				mPriceActive = true;

				if (randPercent(GameConfig::getDouble("ExposeProbPriceRigging"))) {
					EventHandler::raiseEvent(eShowNewspaper, 
						new EventArg<std::string>("np_price.png", 
						StrLoc::get()->NPPriceRigging(), mName + " " 
						+ StrLoc::get()->NPPriceRiggingDetail()));

					if (mGlobalImage >= GameConfig::getInt("ImageHitPriceRigging"))
						mGlobalImage -= GameConfig::getInt("ImageHitPriceRigging");
					else
						mGlobalImage = 0;

					(*it).mDone = true;
					mPriceActive = false;
				}
			}
		}
		else if ((*it).mType == eWar) {
			(*it).mMonthLeft--;

			if ((*it).mMonthLeft <= 0) {
				mWarActive = false;
				(*it).mDone = true;
			} else {
				mWarActive = true;

				if (randPercent(GameConfig::getDouble("ExposeProbWarSupport"))) {
					EventHandler::raiseEvent(eShowNewspaper, 
						new EventArg<std::string>("np_war.png", 
						StrLoc::get()->NPWar(), mName + " " 
						+ StrLoc::get()->NPWarDetail()));

					if (mGlobalImage >= GameConfig::getInt("ImageHitWarSupport"))
						mGlobalImage -= GameConfig::getInt("ImageHitWarSupport");
					else
						mGlobalImage = 0;

					(*it).mDone = true;
					mWarActive = false;
				}
			}
		}
		else if ((*it).mType == eWaste) {
			(*it).mMonthLeft--;

			if ((*it).mMonthLeft <= 0) {
				mWasteActive = false;
				(*it).mDone = true;
			} else {
				mWasteActive = true;

				if (randPercent(GameConfig::getDouble("ExposeProbWasteDumping"))) {
					EventHandler::raiseEvent(eShowNewspaper, 
						new EventArg<std::string>("np_waste.png", 
						StrLoc::get()->NPWasteDumping(), mName 
						+ " " + StrLoc::get()->NPWasteDumpingDetail()));

					if (mGlobalImage >= GameConfig::getInt("ImageHitWasteDumping"))
						mGlobalImage -= GameConfig::getInt("ImageHitWasteDumping");
					else
						mGlobalImage = 0;

					(*it).mDone = true;
					mWasteActive = false;
				}
			}
		}
		else if ((*it).mType == eEmployee) {
			(*it).mMonthLeft--;

			if ((*it).mMonthLeft <= 0) {
				mNoTrainingActive = false;
				(*it).mDone = true;
			} else {
				mNoTrainingActive = true;

				if (randPercent(GameConfig::getDouble("ExposeProbNoTraining"))) {
					EventHandler::raiseEvent(eShowNewspaper, 
						new EventArg<std::string>("np_training.png", 
						StrLoc::get()->NPNoTraining(), mName + " " 
						+ StrLoc::get()->NPNoTrainingDetail()));

					if (mGlobalImage >= GameConfig::getInt("ImageHitNoTraining"))
						mGlobalImage -= GameConfig::getInt("ImageHitNoTraining");
					else
						mGlobalImage = 0;

					(*it).mDone = true;
					mNoTrainingActive = false;
				}
			}
		}

		if ((*it).mDone) {
			it = mSpecialActions.erase(it);
			EventHandler::raiseEvent(eUpdateSpecialActions);
		} else {
			it++;
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Company::removeResourceBuilding(boost::shared_ptr<Collidable> pObject)
{
	std::vector<boost::shared_ptr<ResourceBuilding> >::iterator it = mResourceBuildings.begin();

	while (it != mResourceBuildings.end()) {
		if ((*it)->getID() == pObject->getID())
			it = mResourceBuildings.erase(it);
		else
			it++;
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Company::handleMissionGoals(void)
{
	bool mAllDone = true;

	for (size_t i = 0; i < mMissionGoals.size(); ++i) {
		if (mMissionGoals[i].mType != eFreePlay) {
			if (mMissionGoals[i].mType == eMoney)
				mMissionGoals[i].mValueCurrent = mMoney;

			if (mMissionGoals[i].mType == eCustomerCount)
				mMissionGoals[i].mValueCurrent = mCompleteCustomers;

			if (mMissionGoals[i].mValueCurrent >= mMissionGoals[i].mValueNeeded)
				mMissionGoals[i].mDone = true;

			if (!mMissionGoals[i].mDone)
				mAllDone = false;
		} else {
			mAllDone = false;
		}
	}

	if(mAllDone && !mKeepPlaying)
		EventHandler::raiseEvent(eMissionDone);
}

/*-----------------------------------------------------------------------------------------------*/

bool Company::isSpecialActionRunning(eSpecialAction pType)
{
	for (size_t i = 0; i < mSpecialActions.size(); ++i)
		if (mSpecialActions[i].mType == pType)
			return true;

	if ((pType == eTaxfraud) && (mTaxFreeTrades > 0))
		return true;

	return false;
}

/*-----------------------------------------------------------------------------------------------*/

int Company::getTaxFreeTradesLeft(void)
{
	return mTaxFreeTrades;
}

/*-----------------------------------------------------------------------------------------------*/

void Company::handleAccidents(void)
{
	if (randPercent(GameConfig::getDouble("PoleCollapseProbability")/(mSupplyMaintenance+1)))
		mPowerNet->destroyRandomPole(mMap);

	std::vector<boost::shared_ptr<Powerplant> > lPowerplants = getAllPowerplants();

	for (size_t i = 0; i < lPowerplants.size(); ++i) {
		if ((randPercent(GameConfig::getDouble("PPShutdownProbability")/
      (lPowerplants[i]->getCondition()+1))) && (lPowerplants[i]->getCondition() < 70)) {
			if (!lPowerplants[i]->getDamaged()) {
				lPowerplants[i]->setDamaged();

				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->DamagedPPMessageA() + lPowerplants[i]->getName() 
					+ StrLoc::get()->DamagedPPMessageB();
				lMessage.mPointOfInterest = lPowerplants[i]->getPosition();
				lMessage.mUrgent = false;
				lMessage.mID = 0;
				lMessage.mDetail = StrLoc::get()->DamagedDetail();

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
			}
		}
	}

	// random accidents
	{
		bool lAccident = false;

		if (mSafety == eNoSafety)
			if (randPercent(GameConfig::getDouble("PPAccidentProbabilityNoSafety")))
				lAccident = true;
		else if (mSafety == eNormalSafety)
			if (randPercent(GameConfig::getDouble("PPAccidentProbabilityNormalSafety")))
				lAccident = true;
		else if (mSafety == eExtensiveSafety)
			if (randPercent(GameConfig::getDouble("PPAccidentProbabilityExtensiveSafety")))
				lAccident = true;

		if (lAccident) {
			int lPowerplantAffected = rand() % lPowerplants.size();

			EventHandler::raiseEvent(eShowNewspaper, 
				new EventArg<std::string>("np_accident.png", 
				StrLoc::get()->Accident(), StrLoc::get()->AccidentDetail1() + 
				lPowerplants[lPowerplantAffected]->getName() + StrLoc::get()->AccidentDetail2()));

			TickerMessage lMessage;
			lMessage.mMessage = StrLoc::get()->PPShutdown1() 
        + lPowerplants[lPowerplantAffected]->getName() +
				StrLoc::get()->PPShutdown2();
			lMessage.mPointOfInterest = lPowerplants[lPowerplantAffected]->getPosition();
			lMessage.mDetail = StrLoc::get()->PPAccidentDetail();

			EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));

			lPowerplants[lPowerplantAffected]->setDamaged();
		}
	}

	for (size_t j = 0; j < mResourceBuildings.size(); ++j) {
		if ((randPercent(GameConfig::getDouble("REShutdownProbability")/
      (mResourceBuildings[j]->getCondition()+1))) && (mResourceBuildings[j]->getCondition() < 70)) {
			if (!mResourceBuildings[j]->getDamaged()) {
				mResourceBuildings[j]->setDamaged();

				TickerMessage lMessage;
				lMessage.mMessage = StrLoc::get()->DamagedResourceMessageA() 
          + mResourceBuildings[j]->getName() 
					+ StrLoc::get()->DamagedResourceMessageB();
				lMessage.mPointOfInterest = mResourceBuildings[j]->getPosition();
				lMessage.mUrgent = false;
				lMessage.mID = 0;
				lMessage.mDetail = StrLoc::get()->DamagedDetail();

				EventHandler::raiseEvent(eTickerMessage, new EventArg<TickerMessage>(lMessage));
			}
		}
	}
}

/*-----------------------------------------------------------------------------------------------*/

void Company::dump(std::string pFilename)
{
}

/*-----------------------------------------------------------------------------------------------*/

bool Company::isCreditworthy(int pAmount, std::string& pWhyNot)
{
	if ((int)mCurrentLoans.size() >= GameConfig::getInt("LoanMaxCount")) {
		pWhyNot = StrLoc::get()->TooManyLoans();
		return false;
	}

	int lBaseVolume = GameConfig::getInt("LoanBaseVolume");
	int lNetWorthFactor = GameConfig::getInt("LoanNetWorthFactor");

	int lCompleteCreditVolume = 0;

	for (size_t i = 0; i < mCurrentLoans.size(); ++i)
		lCompleteCreditVolume += mCurrentLoans[i].mAmountLeft;

	int lCompleteK = (lCompleteCreditVolume + pAmount) / 1000;
	int lCreditMaxK = (lNetWorthFactor*mNetWorth + lBaseVolume) / 1000;

	int lNetWorthNeededK = ((lCompleteCreditVolume + pAmount - lBaseVolume)/lNetWorthFactor)/1000;

	if (lCompleteK > lCreditMaxK) {
		pWhyNot = StrLoc::get()->NotEnoughNetWorth() + StrLoc::get()->NetWorthNeeded() 
			+ toString(lNetWorthNeededK) + L"k€" + StrLoc::get()->NetWorthHave()
			+ toString(mNetWorth/1000) + L"k€";
		return false;
	}

	return true;
}
