// GENERATED - DO NOT EDIT
#ifndef LANGEN_H
#define LANGEN_H

/*-----------------------------------------------------------------------------------------------*/

//! Japanese language
class LangJA : public StrLoc {
public:

  Ogre::UTFString LanguageCode()
  { return Ogre::UTFString(L"ja"); }
  Ogre::UTFString GameTitle()
  { return Ogre::UTFString(L"エネルギータイクーン"); }
  Ogre::UTFString MyGUILanguage()
  { return Ogre::UTFString(L"日本語"); }
  Ogre::UTFString MainMenuClose()
  { return Ogre::UTFString(L"退出"); }
  Ogre::UTFString MainMenuOptions()
  { return Ogre::UTFString(L"オプション"); }
  Ogre::UTFString MainMenuNewGame()
  { return Ogre::UTFString(L"ニューゲーム"); }
  Ogre::UTFString MainMenuLoadGame()
  { return Ogre::UTFString(L"ロード"); }
  Ogre::UTFString MainMenuCredits()
  { return Ogre::UTFString(L"クレジット"); }
  Ogre::UTFString MainMenuSaveGame()
  { return Ogre::UTFString(L"セーブ"); }
  Ogre::UTFString MainMenuResumeGame()
  { return Ogre::UTFString(L"中断"); }
  Ogre::UTFString MainMenuBack()
  { return Ogre::UTFString(L"メニューに戻る"); }
  Ogre::UTFString OptionsMenuDisplayMode()
  { return Ogre::UTFString(L"画面モード:"); }
  Ogre::UTFString OptionsMenuGraphicsQuality()
  { return Ogre::UTFString(L"グラフィッククオリティ:"); }
  Ogre::UTFString OptionsMenuNormal()
  { return Ogre::UTFString(L"標準"); }
  Ogre::UTFString OptionsMenuHigh()
  { return Ogre::UTFString(L"高難度"); }
  Ogre::UTFString OptionsMenuFullscreen()
  { return Ogre::UTFString(L"フルスクリーン"); }
  Ogre::UTFString LoadMenuLoad()
  { return Ogre::UTFString(L"ロード"); }
  Ogre::UTFString LoadMenuSaved()
  { return Ogre::UTFString(L"セーブ: "); }
  Ogre::UTFString LoadMenuMap()
  { return Ogre::UTFString(L"地図: "); }
  Ogre::UTFString LoadMenuQuestion()
  { return Ogre::UTFString(L"ゲームは現在動作しています。本当にゲームをロードしますか?"); }
  Ogre::UTFString LoadMenuSelect()
  { return Ogre::UTFString(L"セーブデータを選択してください"); }
  Ogre::UTFString NewMenuStart()
  { return Ogre::UTFString(L"ゲームを開始"); }
  Ogre::UTFString NewMenuEasy()
  { return Ogre::UTFString(L"簡単"); }
  Ogre::UTFString NewMenuMedium()
  { return Ogre::UTFString(L"普通"); }
  Ogre::UTFString NewMenuHard()
  { return Ogre::UTFString(L"難しい"); }
  Ogre::UTFString NewMenuMissions()
  { return Ogre::UTFString(L"ミッション:"); }
  Ogre::UTFString NewMenuDifficulty()
  { return Ogre::UTFString(L"難易度:"); }
  Ogre::UTFString NewMenuQuestion()
  { return Ogre::UTFString(L"ゲームは現在動作しています。新しいゲームを開始しますか?"); }
  Ogre::UTFString NewMenuSelect()
  { return Ogre::UTFString(L"地図とミッションを選択してください。"); }
  Ogre::UTFString NewMenuMissionGoals()
  { return Ogre::UTFString(L"ミッションの目標:"); }
  Ogre::UTFString NewMenuMap()
  { return Ogre::UTFString(L"地図:"); }
  Ogre::UTFString SaveMenuSave()
  { return Ogre::UTFString(L"セーブ"); }
  Ogre::UTFString SaveMenuName()
  { return Ogre::UTFString(L"セーブゲームの名前を選択してください。"); }
  Ogre::UTFString SaveMenuOverwrite()
  { return Ogre::UTFString(L"その名前のセーブデータは既に存在します。上書きしますか?"); }
  Ogre::UTFString SaveMenuSaved()
  { return Ogre::UTFString(L"セーブされました"); }
  Ogre::UTFString Year()
  { return Ogre::UTFString(L"年"); }
  Ogre::UTFString January()
  { return Ogre::UTFString(L"1月"); }
  Ogre::UTFString February()
  { return Ogre::UTFString(L"2月"); }
  Ogre::UTFString March()
  { return Ogre::UTFString(L"3月"); }
  Ogre::UTFString April()
  { return Ogre::UTFString(L"4月"); }
  Ogre::UTFString May()
  { return Ogre::UTFString(L"5月"); }
  Ogre::UTFString June()
  { return Ogre::UTFString(L"6月"); }
  Ogre::UTFString July()
  { return Ogre::UTFString(L"7月"); }
  Ogre::UTFString August()
  { return Ogre::UTFString(L"8月"); }
  Ogre::UTFString September()
  { return Ogre::UTFString(L"9月"); }
  Ogre::UTFString October()
  { return Ogre::UTFString(L"10月"); }
  Ogre::UTFString November()
  { return Ogre::UTFString(L"11月"); }
  Ogre::UTFString December()
  { return Ogre::UTFString(L"12月"); }
  Ogre::UTFString TooltipBuildPowerplant()
  { return Ogre::UTFString(L"発電所を建設"); }
  Ogre::UTFString TooltipBuildDistribution()
  { return Ogre::UTFString(L"送電線を建設"); }
  Ogre::UTFString TooltipBuildResource()
  { return Ogre::UTFString(L"採掘建築を建設"); }
  Ogre::UTFString TooltipBuildMisc()
  { return Ogre::UTFString(L"企業建築を設置"); }
  Ogre::UTFString TooltipDemolish()
  { return Ogre::UTFString(L"建物を破壊"); }
  Ogre::UTFString TooltipSp1()
  { return Ogre::UTFString(L"Don't train employees for 6 month.\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceNoTraining")/1000)) + Ogre::UTFString(L"k€, -25% on operation cost)"); }
  Ogre::UTFString TooltipSp2()
  { return Ogre::UTFString(L"Run illegal waste dumping for 6 month.\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceWasteDumping")/1000)) + Ogre::UTFString(L"k€, -25% on operation cost)"); }
  Ogre::UTFString TooltipSp3()
  { return Ogre::UTFString(L"Commit trade tax fraud for the next 4 trade transactions.\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceTaxFraud")/1000)) + Ogre::UTFString(L"k€, -50% cost for trades)"); }
  Ogre::UTFString TooltipSp4()
  { return Ogre::UTFString(L"Bribe parties running in the next election.\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceBribe")/1000)) + Ogre::UTFString(L"k€, -80% mandatory environmental support for 4 years)"); }
  Ogre::UTFString TooltipSp5()
  { return Ogre::UTFString(L"Support the current governments war efforts.\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceWarSupport")/1000)) + Ogre::UTFString(L"k€, -50% on all resource trades for 1 year)"); }
  Ogre::UTFString TooltipSp6()
  { return Ogre::UTFString(L"Manipulate the prices together with your competitors.\n(") + Ogre::UTFString(toString(GameConfig::getInt("PricePriceRigging")/1000)) + Ogre::UTFString(L"k€, +50% on customer income for 1 year)"); }
  Ogre::UTFString TooltipSpNotAvailable()
  { return Ogre::UTFString(L"You don't have access to this special action yet.\nBuild a headquarter building to access this."); }
  Ogre::UTFString TooltipSp1Img()
  { return Ogre::UTFString(L"全ての従業員は現在トレーニングをしていません。"); }
  Ogre::UTFString TooltipSp2Img()
  { return Ogre::UTFString(L"Illegal waste dumping active"); }
  Ogre::UTFString TooltipSp3ImgA()
  { return Ogre::UTFString(L"Trade tax fraud active ("); }
  Ogre::UTFString TooltipSp3ImgB()
  { return Ogre::UTFString(L" trades left)"); }
  Ogre::UTFString TooltipSp4Img()
  { return Ogre::UTFString(L"Election bribe active"); }
  Ogre::UTFString TooltipSp5Img()
  { return Ogre::UTFString(L"War support active"); }
  Ogre::UTFString TooltipSp6Img()
  { return Ogre::UTFString(L"Price rigging active"); }
  Ogre::UTFString TooltipSpImgInactive()
  { return Ogre::UTFString(L"Special action inactive"); }
  Ogre::UTFString TooltipAd1()
  { return Ogre::UTFString(L"Run internet ad campaign. (") + Ogre::UTFString(GameConfig::getString("AdInternetCost")) + Ogre::UTFString(L"k€)"); }
  Ogre::UTFString TooltipAd2()
  { return Ogre::UTFString(L"Run newspaper ad campaign. (") + Ogre::UTFString(GameConfig::getString("AdNewspaperCost")) + Ogre::UTFString(L"k€)"); }
  Ogre::UTFString TooltipAd3()
  { return Ogre::UTFString(L"Run radio ad campaign. (") + Ogre::UTFString(GameConfig::getString("AdRadioCost")) + Ogre::UTFString(L"k€)"); }
  Ogre::UTFString TooltipAd4()
  { return Ogre::UTFString(L"Run TV ad campaign. (") + Ogre::UTFString(GameConfig::getString("AdTVCost")) + Ogre::UTFString(L"k€)"); }
  Ogre::UTFString TooltipAdNotAvailable()
  { return Ogre::UTFString(L"You don't have access to this ad form yet.\nBuild a public relations building to access this."); }
  Ogre::UTFString TooltipRe1()
  { return Ogre::UTFString(L"Research large coal powerplant."); }
  Ogre::UTFString TooltipRe2()
  { return Ogre::UTFString(L"Research large wind turbine."); }
  Ogre::UTFString TooltipRe3()
  { return Ogre::UTFString(L"Research advertising improvement."); }
  Ogre::UTFString TooltipRe4()
  { return Ogre::UTFString(L"Research bio powerplant."); }
  Ogre::UTFString TooltipRe5()
  { return Ogre::UTFString(L"Research wind efficiency improvement."); }
  Ogre::UTFString TooltipRe6()
  { return Ogre::UTFString(L"Research coal efficiency improvement."); }
  Ogre::UTFString TooltipRe7()
  { return Ogre::UTFString(L"Research offshore turbine."); }
  Ogre::UTFString TooltipRe8()
  { return Ogre::UTFString(L"Research gas efficiency improvement."); }
  Ogre::UTFString TooltipRe9()
  { return Ogre::UTFString(L"Research small nuclear powerplant."); }
  Ogre::UTFString TooltipRe10()
  { return Ogre::UTFString(L"Research large gas powerplant."); }
  Ogre::UTFString TooltipRe11()
  { return Ogre::UTFString(L"Research large coal extraction facility."); }
  Ogre::UTFString TooltipRe12()
  { return Ogre::UTFString(L"Research reduce operation cost"); }
  Ogre::UTFString TooltipRe13()
  { return Ogre::UTFString(L"Research large gas extraction facility."); }
  Ogre::UTFString TooltipRe14()
  { return Ogre::UTFString(L"Research CO2 reduction."); }
  Ogre::UTFString TooltipRe15()
  { return Ogre::UTFString(L"Research uranium efficiency."); }
  Ogre::UTFString TooltipRe16()
  { return Ogre::UTFString(L"Research large solar powerplant."); }
  Ogre::UTFString TooltipRe17()
  { return Ogre::UTFString(L"Research solar updraft powerplant."); }
  Ogre::UTFString TooltipRe18()
  { return Ogre::UTFString(L"Research solar efficiency."); }
  Ogre::UTFString TooltipRe19()
  { return Ogre::UTFString(L"Research large nuclear powerplant."); }
  Ogre::UTFString TooltipRe20()
  { return Ogre::UTFString(L"Research large uranium extraction facility."); }
  Ogre::UTFString TooltipRe21()
  { return Ogre::UTFString(L"Research nuclear fusion powerplant."); }
  Ogre::UTFString TooltipReNotAvailable()
  { return Ogre::UTFString(L"You don't have access to this upgrade yet.\nBuild a research center to access this."); }
  Ogre::UTFString TooltipMenu()
  { return Ogre::UTFString(L"メインメニューへ"); }
  Ogre::UTFString TooltipMission()
  { return Ogre::UTFString(L"目標を表示"); }
  Ogre::UTFString TooltipFinances()
  { return Ogre::UTFString(L"財務状況を表示"); }
  Ogre::UTFString TooltipResearch()
  { return Ogre::UTFString(L"研究を表示"); }
  Ogre::UTFString TooltipCEO()
  { return Ogre::UTFString(L"社長室へ"); }
  Ogre::UTFString TooltipTrade()
  { return Ogre::UTFString(L"交易画面を表示"); }
  Ogre::UTFString TooltipZoomIn()
  { return Ogre::UTFString(L"ズームイン"); }
  Ogre::UTFString TooltipZoomOut()
  { return Ogre::UTFString(L"ズームアウト"); }
  Ogre::UTFString TooltipRotateLeft()
  { return Ogre::UTFString(L"左に回転"); }
  Ogre::UTFString TooltipRotateRight()
  { return Ogre::UTFString(L"右に回転"); }
  Ogre::UTFString TooltipSlower()
  { return Ogre::UTFString(L"ゲームスピードを遅く"); }
  Ogre::UTFString TooltipFaster()
  { return Ogre::UTFString(L"ゲームスピードを早く"); }
  Ogre::UTFString TooltipPause()
  { return Ogre::UTFString(L"ゲームを中断"); }
  Ogre::UTFString TooltipPowerNets()
  { return Ogre::UTFString(L"Show power nets"); }
  Ogre::UTFString TooltipMessageArchive()
  { return Ogre::UTFString(L"Show message archive"); }
  Ogre::UTFString TooltipImage()
  { return Ogre::UTFString(L"Image: "); }
  Ogre::UTFString TooltipReliability()
  { return Ogre::UTFString(L"Reliability: "); }
  Ogre::UTFString TooltipNormal()
  { return Ogre::UTFString(L"Normal"); }
  Ogre::UTFString TooltipHigh()
  { return Ogre::UTFString(L"High"); }
  Ogre::UTFString TooltipLow()
  { return Ogre::UTFString(L"Low"); }
  Ogre::UTFString TooltipCost()
  { return Ogre::UTFString(L"Cost: "); }
  Ogre::UTFString TooltipPower()
  { return Ogre::UTFString(L"Power: "); }
  Ogre::UTFString TooltipOutput()
  { return Ogre::UTFString(L"Output: "); }
  Ogre::UTFString TooltipRods()
  { return Ogre::UTFString(L"Rods"); }
  Ogre::UTFString TooltipKCubicFeet()
  { return Ogre::UTFString(L"Kcubic feet"); }
  Ogre::UTFString TooltipKTons()
  { return Ogre::UTFString(L"Ktons"); }
  Ogre::UTFString TooltipPerWeek()
  { return Ogre::UTFString(L"/Week"); }
  Ogre::UTFString TooltipCapacity()
  { return Ogre::UTFString(L"Capacity: "); }
  Ogre::UTFString TooltipCoalSmall()
  { return Ogre::UTFString(L"小さな石炭発電所"); }
  Ogre::UTFString TooltipCoalLarge()
  { return Ogre::UTFString(L"大きな石炭発電所"); }
  Ogre::UTFString TooltipGasSmall()
  { return Ogre::UTFString(L"小さなガス発電所"); }
  Ogre::UTFString TooltipGasLarge()
  { return Ogre::UTFString(L"大きなガス発電所"); }
  Ogre::UTFString TooltipNuclearSmall()
  { return Ogre::UTFString(L"小さな原子力発電所"); }
  Ogre::UTFString TooltipNuclearLarge()
  { return Ogre::UTFString(L"大きな原子力発電所"); }
  Ogre::UTFString TooltipNuclearFusion()
  { return Ogre::UTFString(L"核融合発電所"); }
  Ogre::UTFString TooltipSolarSmall()
  { return Ogre::UTFString(L"小さな太陽光発電所"); }
  Ogre::UTFString TooltipSolarLarge()
  { return Ogre::UTFString(L"大きな太陽光発電所"); }
  Ogre::UTFString TooltipSolarUpdraft()
  { return Ogre::UTFString(L"太陽光チムニー"); }
  Ogre::UTFString TooltipWindSmall()
  { return Ogre::UTFString(L"小さな風力発電所"); }
  Ogre::UTFString TooltipWindLarge()
  { return Ogre::UTFString(L"大きな風力発電所"); }
  Ogre::UTFString TooltipWindOffshore()
  { return Ogre::UTFString(L"洋上風力発電所"); }
  Ogre::UTFString TooltipBio()
  { return Ogre::UTFString(L"バイオマス発電所"); }
  Ogre::UTFString TooltipHeadquarters()
  { return Ogre::UTFString(L"Company headquarters"); }
  Ogre::UTFString TooltipResearchBuilding()
  { return Ogre::UTFString(L"Research center"); }
  Ogre::UTFString TooltipPR()
  { return Ogre::UTFString(L"Public relations center"); }
  Ogre::UTFString TooltipCoalRSmall()
  { return Ogre::UTFString(L"Small coal mine"); }
  Ogre::UTFString TooltipCoalRLarge()
  { return Ogre::UTFString(L"Large coal mine"); }
  Ogre::UTFString TooltipGasRSmall()
  { return Ogre::UTFString(L"Small gas extraction facility"); }
  Ogre::UTFString TooltipGasRLarge()
  { return Ogre::UTFString(L"Large gas extraction facility"); }
  Ogre::UTFString TooltipUraniumRSmall()
  { return Ogre::UTFString(L"Small uranium mine"); }
  Ogre::UTFString TooltipUraniumRLarge()
  { return Ogre::UTFString(L"Large uranium mine"); }
  Ogre::UTFString TooltipLineSmall()
  { return Ogre::UTFString(L"Small powerline"); }
  Ogre::UTFString TooltipLineLarge()
  { return Ogre::UTFString(L"Large powerline"); }
  Ogre::UTFString NoMoney()
  { return Ogre::UTFString(L"You don't have enough money!"); }
  Ogre::UTFString NoResources()
  { return Ogre::UTFString(L"You don't have enough resources!"); }
  Ogre::UTFString TooltipRepair()
  { return Ogre::UTFString(L"Repair this building"); }
  Ogre::UTFString TooltipMMCapacity()
  { return Ogre::UTFString(L"Show capacity information"); }
  Ogre::UTFString TooltipMMDefault()
  { return Ogre::UTFString(L"Default view"); }
  Ogre::UTFString TooltipMMSupply()
  { return Ogre::UTFString(L"Show supply information"); }
  Ogre::UTFString TooltipMMResources()
  { return Ogre::UTFString(L"Show resource locations"); }
  Ogre::UTFString WantToQuit()
  { return Ogre::UTFString(L"Do you really want to quit?"); }
  Ogre::UTFString GoalNone()
  { return Ogre::UTFString(L"None"); }
  Ogre::UTFString CitySeeNet()
  { return Ogre::UTFString(L"... (see power net)"); }
  Ogre::UTFString CityNone()
  { return Ogre::UTFString(L"None"); }
  Ogre::UTFString PPOutput()
  { return Ogre::UTFString(L"Output: "); }
  Ogre::UTFString PPUsed()
  { return Ogre::UTFString(L"Power used: "); }
  Ogre::UTFString PPConnected()
  { return Ogre::UTFString(L"Connected to: "); }
  Ogre::UTFString Condition()
  { return Ogre::UTFString(L"Condition: "); }
  Ogre::UTFString BuiltYear()
  { return Ogre::UTFString(L"Year built: "); }
  Ogre::UTFString WeeklyCost()
  { return Ogre::UTFString(L"Weekly cost: "); }
  Ogre::UTFString DamagedNeedsRepair()
  { return Ogre::UTFString(L"Damaged, needs repair"); }
  Ogre::UTFString Loading()
  { return Ogre::UTFString(L"Loading..."); }
  Ogre::UTFString LoadingTerrain()
  { return Ogre::UTFString(L"Terrain"); }
  Ogre::UTFString LoadingTreesResources()
  { return Ogre::UTFString(L"Distributing trees & resources"); }
  Ogre::UTFString LoadingCompanyData()
  { return Ogre::UTFString(L"Company data"); }
  Ogre::UTFString LoadingPopulace()
  { return Ogre::UTFString(L"Populace"); }
  Ogre::UTFString COCoal()
  { return Ogre::UTFString(L"石炭: "); }
  Ogre::UTFString COGas()
  { return Ogre::UTFString(L"天然ガス: "); }
  Ogre::UTFString COUranium()
  { return Ogre::UTFString(L"ウラン: "); }
  Ogre::UTFString COKCubicShort()
  { return Ogre::UTFString(L"KCF"); }
  Ogre::UTFString COKTonShort()
  { return Ogre::UTFString(L"KTon"); }
  Ogre::UTFString CORodShort()
  { return Ogre::UTFString(L"Rod"); }
  Ogre::UTFString CONet()
  { return Ogre::UTFString(L"Net"); }
  Ogre::UTFString COCities()
  { return Ogre::UTFString(L"Cities: "); }
  Ogre::UTFString COPowerplants()
  { return Ogre::UTFString(L"Powerplants: "); }
  Ogre::UTFString COSelectCity()
  { return Ogre::UTFString(L"Select city for details."); }
  Ogre::UTFString COSelectPP()
  { return Ogre::UTFString(L"Select powerplant for details."); }
  Ogre::UTFString CONetEfficiency()
  { return Ogre::UTFString(L"Net efficiency: "); }
  Ogre::UTFString CONetAvailable()
  { return Ogre::UTFString(L"Power available: "); }
  Ogre::UTFString CONetNeeded()
  { return Ogre::UTFString(L"Needed by Customers: "); }
  Ogre::UTFString Residents()
  { return Ogre::UTFString(L"Residents: "); }
  Ogre::UTFString PowerNeeded()
  { return Ogre::UTFString(L"Needed by Customers:"); }
  Ogre::UTFString PowerSuppliedByYou()
  { return Ogre::UTFString(L"Supplied by you:"); }
  Ogre::UTFString Customers()
  { return Ogre::UTFString(L"Customers: "); }
  Ogre::UTFString SuppliedBy()
  { return Ogre::UTFString(L"Supplied by: "); }
  Ogre::UTFString Price()
  { return Ogre::UTFString(L"Price:"); }
  Ogre::UTFString UseGlobalSettings()
  { return Ogre::UTFString(L"Use global settings"); }
  Ogre::UTFString AdsBudget()
  { return Ogre::UTFString(L"Ads budget:"); }
  Ogre::UTFString Week()
  { return Ogre::UTFString(L"week"); }
  Ogre::UTFString PPMaintenance()
  { return Ogre::UTFString(L"Maintenance:"); }
  Ogre::UTFString Nothing()
  { return Ogre::UTFString(L"Nothing"); }
  Ogre::UTFString CurrentlyResearching()
  { return Ogre::UTFString(L"Currently researching:"); }
  Ogre::UTFString ResearchCaption()
  { return Ogre::UTFString(L"Research"); }
  Ogre::UTFString ResearchDone()
  { return Ogre::UTFString(L"This research project has already been completed."); }
  Ogre::UTFString ResearchOne()
  { return Ogre::UTFString(L"You can only have one research project at a time!"); }
  Ogre::UTFString ResearchNoAccess()
  { return Ogre::UTFString(L"You don't have access to this yet!"); }
  Ogre::UTFString ResearchNoAccessTree()
  { return Ogre::UTFString(L"You don't have access to this yet!"); }
  Ogre::UTFString StartResearchA()
  { return Ogre::UTFString(L"Start research on: '"); }
  Ogre::UTFString StartResearchB()
  { return Ogre::UTFString(L"'? ("); }
  Ogre::UTFString Re1()
  { return Ogre::UTFString(L"Large coal powerplant"); }
  Ogre::UTFString Re2()
  { return Ogre::UTFString(L"Large wind turbine"); }
  Ogre::UTFString Re3()
  { return Ogre::UTFString(L"Advertising improvement"); }
  Ogre::UTFString Re4()
  { return Ogre::UTFString(L"Bio powerplant"); }
  Ogre::UTFString Re5()
  { return Ogre::UTFString(L"Wind efficiency improvement"); }
  Ogre::UTFString Re6()
  { return Ogre::UTFString(L"Coal efficiency improvement"); }
  Ogre::UTFString Re7()
  { return Ogre::UTFString(L"Offshore turbine"); }
  Ogre::UTFString Re8()
  { return Ogre::UTFString(L"Gas efficiency improvement"); }
  Ogre::UTFString Re9()
  { return Ogre::UTFString(L"Small nuclear powerplant"); }

  Ogre::UTFString Re10()
  { return Ogre::UTFString(L"Large gas powerplant"); }
  Ogre::UTFString Re11()
  { return Ogre::UTFString(L"Large coal extraction facility"); }
  Ogre::UTFString Re12()
  { return Ogre::UTFString(L"Reduced operation cost"); }
  Ogre::UTFString Re13()
  { return Ogre::UTFString(L"Large gas extraction facility"); }
  Ogre::UTFString Re14()
  { return Ogre::UTFString(L"CO2 reduction"); }
  Ogre::UTFString Re15()
  { return Ogre::UTFString(L"Uranium efficiency improvement"); }
  Ogre::UTFString Re16()
  { return Ogre::UTFString(L"Large solar powerplant"); }
  Ogre::UTFString Re17()
  { return Ogre::UTFString(L"Solar updraft powerplant"); }
  Ogre::UTFString Re18()
  { return Ogre::UTFString(L"Solar efficiency improvement"); }
  Ogre::UTFString Re19()
  { return Ogre::UTFString(L"Large nuclear powerplant"); }
  Ogre::UTFString Re20()
  { return Ogre::UTFString(L"Large uranium extraction facility"); }
  Ogre::UTFString Re21()
  { return Ogre::UTFString(L"Nuclear fusion powerplant"); }
  Ogre::UTFString CEOCaption()
  { return Ogre::UTFString(L"CEOs Office"); }
  Ogre::UTFString CEOOffice()
  { return Ogre::UTFString(L"Office"); }
  Ogre::UTFString CustomersOpinionsFrom()
  { return Ogre::UTFString(L"Customer opinions from:"); }
  Ogre::UTFString NoOpinions()
  { return Ogre::UTFString(L"No customer opinions"); }
  Ogre::UTFString AllCities()
  { return Ogre::UTFString(L"All cities"); }
  Ogre::UTFString CEOCustomers()
  { return Ogre::UTFString(L"Customers"); }
  Ogre::UTFString Ad1Question()
  { return Ogre::UTFString(L"Run a Internet ad campaign for ") + Ogre::UTFString(GameConfig::getString("AdInternetCost")) + Ogre::UTFString(L"k€?"); }
  Ogre::UTFString Ad2Question()
  { return Ogre::UTFString(L"Run a newspaper ad campaign for ") + Ogre::UTFString(GameConfig::getString("AdNewspaperCost")) + Ogre::UTFString(L"k€?"); }
  Ogre::UTFString Ad3Question()
  { return Ogre::UTFString(L"Run a radio ad campaign for ") + Ogre::UTFString(GameConfig::getString("AdRadioCost")) + Ogre::UTFString(L"k€?"); }
  Ogre::UTFString Ad4Question()
  { return Ogre::UTFString(L"Run a TV ad campaign for ") + Ogre::UTFString(GameConfig::getString("AdTVCost")) + Ogre::UTFString(L"k€?"); }
  Ogre::UTFString AdNoAccess()
  { return Ogre::UTFString(L"You don't have access to this yet!\nBuild a public relations building to access this."); }
  Ogre::UTFString Sp1Question()
  { return Ogre::UTFString(L"Don't train employees for 6 month? (") + Ogre::UTFString(toString(GameConfig::getInt("PriceNoTraining")/1000)) + Ogre::UTFString(L"k€, -25% on operation cost)"); }
  Ogre::UTFString Sp2Question()
  { return Ogre::UTFString(L"Run illegal waste dumping for 6 month? (") + Ogre::UTFString(toString(GameConfig::getInt("PriceWasteDumping")/1000)) + Ogre::UTFString(L"k€, -25% on operation cost)"); }
  Ogre::UTFString Sp3Question()
  { return Ogre::UTFString(L"Commit trade tax fraud for the next 4 trade transactions?\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceTaxFraud")/1000)) + Ogre::UTFString(L"k€, -50% cost for trades)"); }
  Ogre::UTFString Sp4Question()
  { return Ogre::UTFString(L"Bribe parties running in the next election?\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceBribe")/1000)) + Ogre::UTFString(L"k€, -80% mandatory environmental support for 4 years)"); }
  Ogre::UTFString Sp5Question()
  { return Ogre::UTFString(L"Support the current governments war efforts? (") + Ogre::UTFString(toString(GameConfig::getInt("PriceWarSupport")/1000)) + Ogre::UTFString(L"k€, -50% on all resource trades for 1 year)"); }
  Ogre::UTFString Sp6Question()
  { return Ogre::UTFString(L"Manipulate the prices together with your competitors? (") + Ogre::UTFString(toString(GameConfig::getInt("PricePriceRigging")/1000)) + Ogre::UTFString(L"k€, +50% on customer income for 1 year)"); }
  Ogre::UTFString SpStarted()
  { return Ogre::UTFString(L"You already started this special action."); }
  Ogre::UTFString SpNoAccess()
  { return Ogre::UTFString(L"You don't have access to this yet!\nBuild a headquarters building to access this."); }
  Ogre::UTFString MissionCaption()
  { return Ogre::UTFString(L"Mission goals"); }
  Ogre::UTFString TickerCaption()
  { return Ogre::UTFString(L"Ticker archive"); }
  Ogre::UTFString TickerEmpty()
  { return Ogre::UTFString(L"The ticker archive is empty."); }
  Ogre::UTFString BuildingNoAccess()
  { return Ogre::UTFString(L"You don't have access to this building yet."); }
  Ogre::UTFString NoPowerNet()
  { return Ogre::UTFString(L"You don't have a power net."); }
  Ogre::UTFString InterestRate()
  { return Ogre::UTFString(L"Interest rate: "); }
  Ogre::UTFString MonthlyCharge()
  { return Ogre::UTFString(L"Monthly charge: "); }
  Ogre::UTFString NewMonthlyCharge()
  { return Ogre::UTFString(L"New monthly charge: "); }
  Ogre::UTFString RepairA()
  { return Ogre::UTFString(L"Repair "); }
  Ogre::UTFString RepairB()
  { return Ogre::UTFString(L" for "); }
  Ogre::UTFString UniqueHeadquarters()
  { return Ogre::UTFString(L"You can only build one headquarter building!"); }
  Ogre::UTFString UniqueResearch()
  { return Ogre::UTFString(L"You can only build one research building!"); }
  Ogre::UTFString UniquePR()
  { return Ogre::UTFString(L"You can only build one public relations building!"); }
  Ogre::UTFString Sp1Started()
  { return Ogre::UTFString(L"Special action 'No employee training' started."); }
  Ogre::UTFString Sp2Started()
  { return Ogre::UTFString(L"Special action 'Illegal waste dumping' started."); }
  Ogre::UTFString Sp3Started()
  { return Ogre::UTFString(L"Special action 'Tax fraud' started."); }
  Ogre::UTFString Sp4Started()
  { return Ogre::UTFString(L"Special action 'Election bribe' started."); }
  Ogre::UTFString Sp5Started()
  { return Ogre::UTFString(L"Special action 'War support' started."); }
  Ogre::UTFString Sp6Started()
  { return Ogre::UTFString(L"Special action 'Price rigging' started."); }
  Ogre::UTFString Sp1Detail()
  { return Ogre::UTFString(L"Your employess won't be trained for 6 month."); }
  Ogre::UTFString Sp2Detail()
  { return Ogre::UTFString(L"Illegal waste dumping saves you money, but uncovered it severely damages your image."); }
  Ogre::UTFString Sp3Detail()
  { return Ogre::UTFString(L"Tax fraud gives you 4 tax-free trades which saves a lot of money."); }
  Ogre::UTFString Sp4Detail()
  { return Ogre::UTFString(L"You won't have to pay environment payments for the next legislation period. Uncovered it will damage your image."); }
  Ogre::UTFString Sp5Detail()
  { return Ogre::UTFString(L"Saves you a lot of money when trading resources, but uncovered it severely damages your image."); }
  Ogre::UTFString Sp6Detail()
  { return Ogre::UTFString(L"Price rigging doubles your income, but uncovered it severely damages your image and you may have to pay a hefty fine."); }
  Ogre::UTFString Ad1Started()
  { return Ogre::UTFString(L"Internet ad campaign started."); }
  Ogre::UTFString Ad2Started()
  { return Ogre::UTFString(L"Newspaper ad campaign started."); }
  Ogre::UTFString Ad3Started()
  { return Ogre::UTFString(L"Radio ad campaign started."); }
  Ogre::UTFString Ad4Started()
  { return Ogre::UTFString(L"TV ad campaign started."); }
  Ogre::UTFString Ad1Detail()
  { return Ogre::UTFString(L"Ad campaigns improve your image, but only for a short time."); }
  Ogre::UTFString Ad2Detail()
  { return Ogre::UTFString(L"Ad campaigns improve your image, but only for a short time."); }
  Ogre::UTFString Ad3Detail()
  { return Ogre::UTFString(L"Ad campaigns improve your image, but only for a short time."); }
  Ogre::UTFString Ad4Detail()
  { return Ogre::UTFString(L"Ad campaigns improve your image, but only for a short time."); }
  Ogre::UTFString OfferNA()
  { return Ogre::UTFString(L"The offer is no longer available!"); }
  Ogre::UTFString ObligationNA()
  { return Ogre::UTFString(L"The obligation is no longer active!"); }
  Ogre::UTFString SelectOffer()
  { return Ogre::UTFString(L"Please select a trading offer."); }
  Ogre::UTFString SelectObligation()
  { return Ogre::UTFString(L"Please select an obligation."); }
  Ogre::UTFString SelectPartner()
  { return Ogre::UTFString(L"Please select a trading partner."); }
  Ogre::UTFString CancelObligation()
  { return Ogre::UTFString(L"Do you really want to cancel that obligation?"); }
  Ogre::UTFString SignTradeA()
  { return Ogre::UTFString(L"Do you want to sign a trade agreement with "); }
  Ogre::UTFString SignTradeB()
  { return Ogre::UTFString(L" for "); }
  Ogre::UTFString SignTradeC()
  { return Ogre::UTFString(L"?"); }
  Ogre::UTFString Coal()
  { return Ogre::UTFString(L"Coal"); }
  Ogre::UTFString Gas()
  { return Ogre::UTFString(L"Gas"); }
  Ogre::UTFString Uranium()
  { return Ogre::UTFString(L"Uranium"); }
  Ogre::UTFString CoalS()
  { return Ogre::UTFString(L"coal"); }
  Ogre::UTFString GasS()
  { return Ogre::UTFString(L"gas"); }
  Ogre::UTFString UraniumS()
  { return Ogre::UTFString(L"Uranium"); }
  Ogre::UTFString Sell()
  { return Ogre::UTFString(L"Sell"); }
  Ogre::UTFString Buy()
  { return Ogre::UTFString(L"Buy"); }
  Ogre::UTFString SelectLoan()
  { return Ogre::UTFString(L"Please select a current loan."); }
  Ogre::UTFString ChooseSum()
  { return Ogre::UTFString(L"Please choose a repayment sum."); }
  Ogre::UTFString RepayA()
  { return Ogre::UTFString(L"Do you want to repay "); }
  Ogre::UTFString RepayB()
  { return Ogre::UTFString(L"k€ on the selected loan?"); }
  Ogre::UTFString TakeoutA()
  { return Ogre::UTFString(L"Take out a loan of "); }
  Ogre::UTFString TakeoutB()
  { return Ogre::UTFString(L"k€ running "); }
  Ogre::UTFString TakeoutC()
  { return Ogre::UTFString(L" years?"); }
  Ogre::UTFString FinancesCaption()
  { return Ogre::UTFString(L"Finances"); }
  Ogre::UTFString PriceText()
  { return Ogre::UTFString(L"Standard price:"); }
  Ogre::UTFString AdsText()
  { return Ogre::UTFString(L"Standard advertising budget:"); }
  Ogre::UTFString PriceUnit()
  { return Ogre::UTFString(L"cent per kWh"); }
  Ogre::UTFString AdsUnit()
  { return Ogre::UTFString(L"EUR per week and connected city"); }
  Ogre::UTFString MaintenanceText()
  { return Ogre::UTFString(L"Facility maintenance:"); }
  Ogre::UTFString EnvText()
  { return Ogre::UTFString(L"Environmental support:"); }
  Ogre::UTFString EnvUnit()
  { return Ogre::UTFString(L"kEUR per month"); }
  Ogre::UTFString ContractText()
  { return Ogre::UTFString(L"Minimum contract term:"); }
  Ogre::UTFString SupportText()
  { return Ogre::UTFString(L"Customer support:"); }
  Ogre::UTFString SupplyText()
  { return Ogre::UTFString(L"Supply lines maintenance:"); }
  Ogre::UTFString SafetyText()
  { return Ogre::UTFString(L"Safety measures:"); }
  Ogre::UTFString TermNone()
  { return Ogre::UTFString(L"none"); }
  Ogre::UTFString Term12()
  { return Ogre::UTFString(L"12 month"); }
  Ogre::UTFString Term24()
  { return Ogre::UTFString(L"24 month"); }
  Ogre::UTFString SupportPoor()
  { return Ogre::UTFString(L"poor"); }
  Ogre::UTFString SupportNormal()
  { return Ogre::UTFString(L"normal"); }
  Ogre::UTFString SupportExtensive()
  { return Ogre::UTFString(L"extensive"); }
  Ogre::UTFString EmployeeText()
  { return Ogre::UTFString(L"Employee training:"); }
  Ogre::UTFString SafetyNone()
  { return Ogre::UTFString(L"none"); }
  Ogre::UTFString SafetyNormal()
  { return Ogre::UTFString(L"normal"); }
  Ogre::UTFString SafetyHeavy()
  { return Ogre::UTFString(L"heavy"); }
  Ogre::UTFString EmployeeNone()
  { return Ogre::UTFString(L"none"); }
  Ogre::UTFString EmployeeBasic()
  { return Ogre::UTFString(L"basic"); }
  Ogre::UTFString EmployeeExtensive()
  { return Ogre::UTFString(L"extensive"); }
  Ogre::UTFString SalesCaption()
  { return Ogre::UTFString(L"Sales"); }
  Ogre::UTFString IncomeCaption()
  { return Ogre::UTFString(L"Income/Expenses"); }
  Ogre::UTFString CurrentMonthText()
  { return Ogre::UTFString(L"Current month:"); }
  Ogre::UTFString LastMonthText()
  { return Ogre::UTFString(L"Last month:"); }
  Ogre::UTFString CurrentYearText()
  { return Ogre::UTFString(L"Current year:"); }
  Ogre::UTFString LastYearText()
  { return Ogre::UTFString(L"Last year:"); }
  Ogre::UTFString TotalText()
  { return Ogre::UTFString(L"Total:"); }
  Ogre::UTFString FLoan()
  { return Ogre::UTFString(L"Loan payments:"); }
  Ogre::UTFString FSafety()
  { return Ogre::UTFString(L"Safety/Training:"); }
  Ogre::UTFString FAdvertising()
  { return Ogre::UTFString(L"Advertising:"); }
  Ogre::UTFString FMaintenance()
  { return Ogre::UTFString(L"Maintenance:"); }
  Ogre::UTFString FOperation()
  { return Ogre::UTFString(L"Facilities operation:"); }
  Ogre::UTFString FEnvironment()
  { return Ogre::UTFString(L"Environmental costs:"); }
  Ogre::UTFString FResearch()
  { return Ogre::UTFString(L"Research expenses:"); }
  Ogre::UTFString FBuilding()
  { return Ogre::UTFString(L"Building expenses:"); }
  Ogre::UTFString FIncome()
  { return Ogre::UTFString(L"Customer income:"); }
  Ogre::UTFString FSpecial()
  { return Ogre::UTFString(L"Special expenses:"); }
  Ogre::UTFString FTrade()
  { return Ogre::UTFString(L"Trade:"); }
  Ogre::UTFString ChartTab()
  { return Ogre::UTFString(L"Charts"); }
  Ogre::UTFString ExpensesCheck()
  { return Ogre::UTFString(L"Expenses"); }
  Ogre::UTFString IncomeCheck()
  { return Ogre::UTFString(L"Income"); }
  Ogre::UTFString FundsCheck()
  { return Ogre::UTFString(L"Funds"); }
  Ogre::UTFString NetWorthCheck()
  { return Ogre::UTFString(L"Net worth"); }
  Ogre::UTFString LoanTab()
  { return Ogre::UTFString(L"Loans"); }
  Ogre::UTFString TakeOutButton()
  { return Ogre::UTFString(L"Request loan"); }
  Ogre::UTFString RepaymentButton()
  { return Ogre::UTFString(L"Unscheduled repayment"); }
  Ogre::UTFString NoLoansText()
  { return Ogre::UTFString(L"No current loans"); }
  Ogre::UTFString CurrentLoansText()
  { return Ogre::UTFString(L"Current loans:"); }
  Ogre::UTFString AmountText()
  { return Ogre::UTFString(L"Amount:"); }
  Ogre::UTFString PeriodUnit()
  { return Ogre::UTFString(L"years"); }
  Ogre::UTFString PeriodText()
  { return Ogre::UTFString(L"Period:"); }
  Ogre::UTFString TradeCaption()
  { return Ogre::UTFString(L"Trade"); }
  Ogre::UTFString TradingPartnersText()
  { return Ogre::UTFString(L"Trading partners:"); }
  Ogre::UTFString NoTradingPartners()
  { return Ogre::UTFString(L"No trading\npartners left"); }
  Ogre::UTFString WorldMarketText()
  { return Ogre::UTFString(L"World market prices:"); }
  Ogre::UTFString SignButton()
  { return Ogre::UTFString(L"Sign trade agreement"); }
  Ogre::UTFString NoTradeOffers()
  { return Ogre::UTFString(L"No trade offers this month."); }
  Ogre::UTFString OffersText()
  { return Ogre::UTFString(L"Offers:"); }
  Ogre::UTFString AcceptOfferButton()
  { return Ogre::UTFString(L"Accept offer"); }
  Ogre::UTFString NoObligations()
  { return Ogre::UTFString(L"You have no current obligations."); }
  Ogre::UTFString CancelOfferButton()
  { return Ogre::UTFString(L"Cancel"); }
  Ogre::UTFString CurrentObligationsText()
  { return Ogre::UTFString(L"Current\nobligations"); }
  Ogre::UTFString LoanFormattedA()
  { return Ogre::UTFString(L"Amount left: "); }
  Ogre::UTFString LoanFormattedB()
  { return Ogre::UTFString(L" k€, Rates left: "); }
  Ogre::UTFString LoanFormattedC()
  { return Ogre::UTFString(L" k€ for "); }
  Ogre::UTFString LoanFormattedD()
  { return Ogre::UTFString(L" months."); }
  Ogre::UTFString FreeTradesLeft()
  { return Ogre::UTFString(L" tax-free trades left."); }
  Ogre::UTFString FreeTradesGone()
  { return Ogre::UTFString(L"You've used up all of your tax-free trades."); }
  Ogre::UTFString DamagedResourceMessageA()
  { return Ogre::UTFString(L"Your resource building "); }
  Ogre::UTFString DamagedResourceMessageB()
  { return Ogre::UTFString(L" has been shut down due to a technical failure"); }
  Ogre::UTFString DamagedPPMessageA()
  { return Ogre::UTFString(L"Your powerplant "); }
  Ogre::UTFString DamagedPPMessageB()
  { return Ogre::UTFString(L" has been shut down due to a technical failure"); }
  Ogre::UTFString DamagedDetail()
  { return Ogre::UTFString(L"To repair this building select it and click the repair button"); }
  Ogre::UTFString ResearchFinishedA()
  { return Ogre::UTFString(L"Research on '"); }
  Ogre::UTFString ResearchFinishedB()
  { return Ogre::UTFString(L"' finished!"); }
  Ogre::UTFString ResearchStartedA()
  { return Ogre::UTFString(L"Research on '"); }
  Ogre::UTFString ResearchStartedB()
  { return Ogre::UTFString(L"' started."); }
  Ogre::UTFString BuyF()
  { return Ogre::UTFString(L"Buy "); }
  Ogre::UTFString SellF()
  { return Ogre::UTFString(L"Sell "); }
  Ogre::UTFString From()
  { return Ogre::UTFString(L" from "); }
  Ogre::UTFString To()
  { return Ogre::UTFString(L" to "); }
  Ogre::UTFString For()
  { return Ogre::UTFString(L" for "); }
  Ogre::UTFString EveryWeek()
  { return Ogre::UTFString(L"every week"); }
  Ogre::UTFString PriceT()
  { return Ogre::UTFString(L"Price"); }
  Ogre::UTFString WorldMarket()
  { return Ogre::UTFString(L"World market"); }
  Ogre::UTFString Weeks()
  { return Ogre::UTFString(L"weeks"); }
  Ogre::UTFString EveryWeekS()
  { return Ogre::UTFString(L"Every week"); }
  Ogre::UTFString WeeksRemaining()
  { return Ogre::UTFString(L"weeks remaining"); }
  Ogre::UTFString ContractCanceledResources()
  { return Ogre::UTFString(L"A contract has been canceled (No resources). Your trading partner is displeased."); }
  Ogre::UTFString ContractCanceledMoney()
  { return Ogre::UTFString(L"A contract has been canceled (No money). Your trading partner is displeased."); }
  Ogre::UTFString ContractCanceledResourcesDetail()
  { return Ogre::UTFString(L"You have run out of resources and are unable to fulfill this contract in the future."); }
  Ogre::UTFString ContractCanceledMoneyDetail()
  { return Ogre::UTFString(L"You have run out of money and are unable to fulfill this contract in the future."); }
  Ogre::UTFString GasStockDepleted()
  { return Ogre::UTFString(L"Your gas stock is depleted!"); }
  Ogre::UTFString CoalStockDepleted()
  { return Ogre::UTFString(L"Your coal stock is depleted!"); }
  Ogre::UTFString UraniumStockDepleted()
  { return Ogre::UTFString(L"Your Uranium stock is depleted!"); }
  Ogre::UTFString HeadquartersBuilt()
  { return Ogre::UTFString(L"Headquarters built. You got access to more special actions!"); }
  Ogre::UTFString HeadquartersBuiltDetail()
  { return Ogre::UTFString(L"You have built your companys headquarter. This means you now have access to all special actions. Open the CEO window to see the new actions."); }
  Ogre::UTFString ResearchBuilt()
  { return Ogre::UTFString(L"Research built. You got access to more research projects!"); }
  Ogre::UTFString ResearchBuiltDetail()
  { return Ogre::UTFString(L"You have built a research center. This means that you now have access to all research projects. Open the research window to see the new projects."); }
  Ogre::UTFString PRBuilt()
  { return Ogre::UTFString(L"Public relations built. You got access to more ad forms!"); }
  Ogre::UTFString PRBuiltDetail()
  { return Ogre::UTFString(L"You have built a public relations center. This means that you now have access to all ad forms. Open the CEO window to see the new options."); }
  Ogre::UTFString NewYear()
  { return Ogre::UTFString(L"A new year!"); }
  Ogre::UTFString NoBuild()
  { return Ogre::UTFString(L"You can't build there!"); }
  Ogre::UTFString NoBuildGas()
  { return Ogre::UTFString(L"You can't build there! You must build this on a gas deposit."); }
  Ogre::UTFString NoBuildUranium()
  { return Ogre::UTFString(L"You can't build there! You must build this on a uranium deposit."); }
  Ogre::UTFString NoBuildCoal()
  { return Ogre::UTFString(L"You can't build there! You must build this on a coal deposit."); }
  Ogre::UTFString NoBuildOffshore()
  { return Ogre::UTFString(L"You can't build there! Offshore turbines must be built in the sea."); }
  Ogre::UTFString ScreenSaved()
  { return Ogre::UTFString(L" written."); }
  Ogre::UTFString GameSaved()
  { return Ogre::UTFString(L"Game saved."); }
  Ogre::UTFString PowerplantText()
  { return Ogre::UTFString(L"Powerplant"); }
  Ogre::UTFString FullCapacity()
  { return Ogre::UTFString(L" is operating at full capacity."); }
  Ogre::UTFString NewspaperImage()
  { return Ogre::UTFString(L"newspaper_en.png"); }
  Ogre::UTFString NPFounded()
  { return Ogre::UTFString(L"New electric company founded!"); }
  Ogre::UTFString NPFoundedDetail()
  { return Ogre::UTFString(L"A new electric company has been founded. Will it be able to establish itself in the market?"); }
  Ogre::UTFString NPFusion()
  { return Ogre::UTFString(L"Nuclear\nfusion\nmastered!"); }
  Ogre::UTFString NPFusionDetail()
  { return Ogre::UTFString(L"is the worlds first electric company to master the nuclear fusion technology. Will this be a new era for mankind?"); }
  Ogre::UTFString NPBribe()
  { return Ogre::UTFString(L"Bribe\nscandal\nuncovered!"); }
  Ogre::UTFString NPBribeDetail()
  { return Ogre::UTFString(L"tried to bribe parties running for the current election to save mandatory environmental payments."); }
  Ogre::UTFString NPPriceRigging()
  { return Ogre::UTFString(L"Price\nrigging\nuncovered!"); }
  Ogre::UTFString NPPriceRiggingDetail()
  { return Ogre::UTFString(L"tried to rig prices together with its competitors."); }
  Ogre::UTFString NPWasteDumping()
  { return Ogre::UTFString(L"Illegal\nwaste\ndumping!"); }
  Ogre::UTFString NPWasteDumpingDetail()
  { return Ogre::UTFString(L"has been caught dumping waste illegally."); }
  Ogre::UTFString NPNoTraining()
  { return Ogre::UTFString(L"Lousy\nemployee\ntraining!"); }
  Ogre::UTFString NPNoTrainingDetail()
  { return Ogre::UTFString(L"tried to save money by not training its employees."); }
  Ogre::UTFString NPWar()
  { return Ogre::UTFString(L"War\nsupport\nuncovered!"); }
  Ogre::UTFString NPWarDetail()
  { return Ogre::UTFString(L"supported the government in its war efforts to save money trading resources."); }
  Ogre::UTFString MissionDoneCaption()
  { return Ogre::UTFString(L"Mission completed!"); }
  Ogre::UTFString MissionDoneText()
  { return Ogre::UTFString(L"Congratulations!\n\nYou have completed this mission. You may\nkeep playing or go to the menu and start\nanother mission."); }
  Ogre::UTFString KeepPlaying()
  { return Ogre::UTFString(L"Keep playing"); }
  Ogre::UTFString ToMenu()
  { return Ogre::UTFString(L"Quit to menu"); }
  Ogre::UTFString BankruptcyImminentCaption()
  { return Ogre::UTFString(L"Bankruptcy imminent!"); }
  Ogre::UTFString BankruptcyImminentText()
  { return Ogre::UTFString(L"Dear CEO of Default Corp.,\n\nIt has come to our attention that your\ndebt has exceeded 2.000kEUR. Reduce\nyour debt to a maximum of 1.000kEUR by\nthe end of this year or we will be forced\nto shut you down.\n\nYours sincerely,\nOmniBank"); }
  Ogre::UTFString BankruptcyCaption()
  { return Ogre::UTFString(L"Bankruptcy!"); }
  Ogre::UTFString BankruptcyText()
  { return Ogre::UTFString(L"Dear CEO of Default Corp.,\n\nSince you were unable to reduce your debt\nfollowing our warning, we are forced to\nshut your company down.\n\nYours sincerely,\nOmniBank"); }
  Ogre::UTFString Small()
  { return Ogre::UTFString(L"small"); }
  Ogre::UTFString Large()
  { return Ogre::UTFString(L"large"); }
  Ogre::UTFString ConnectThisCity()
  { return Ogre::UTFString(L"#CC0000Connect this city\nto gain customers"); }
  Ogre::UTFString CreditsText()
  { return Ogre::UTFString(L"Programming\nMichael Schießl\n\nGraphics\nPeter Schießl\n\nMusic\nKevin McLeod"); }
  Ogre::UTFString NoSavegameSelected()
  { return Ogre::UTFString(L"Select a savegame"); }
  Ogre::UTFString PowerNetCaption()
  { return Ogre::UTFString(L"Power nets"); }
  Ogre::UTFString PoleCollapsed()
  { return Ogre::UTFString(L"A pole collapsed due to bad maintenance. Click here to see where."); }
  Ogre::UTFString Building()
  { return Ogre::UTFString(L"Building..."); }
  Ogre::UTFString TooManyLoans()
  { return Ogre::UTFString(L"You have too many open loans. The bank refuses to give you another."); }
  Ogre::UTFString NotEnoughNetWorth()
  { return Ogre::UTFString(L"Your net worth isn't enough for this loan. "); }
  Ogre::UTFString NetWorthNeeded()
  { return Ogre::UTFString(L"Needed: "); }
  Ogre::UTFString NetWorthHave()
  { return Ogre::UTFString(L", you have: "); }
  Ogre::UTFString ForecastForThisMonth()
  { return Ogre::UTFString(L"Weather Forecast:"); }
  Ogre::UTFString StrongWind()
  { return Ogre::UTFString(L"Strong wind"); }
  Ogre::UTFString NormalWind()
  { return Ogre::UTFString(L"Average wind"); }
  Ogre::UTFString LittleWind()
  { return Ogre::UTFString(L"Little wind"); }
  Ogre::UTFString PlentySunshine()
  { return Ogre::UTFString(L"Plenty of sunshine"); }
  Ogre::UTFString NormalSunshine()
  { return Ogre::UTFString(L"Normal sunshine"); }
  Ogre::UTFString HardlySunshine()
  { return Ogre::UTFString(L"Hardly any sunshine"); }
  Ogre::UTFString Tutorial()
  { return Ogre::UTFString(L"Tutorial"); }
  Ogre::UTFString Step()
  { return Ogre::UTFString(L"Step"); }
  Ogre::UTFString TutorialBack()
  { return Ogre::UTFString(L"Back"); }
  Ogre::UTFString TutorialContinue()
  { return Ogre::UTFString(L"Next"); }
  Ogre::UTFString TutorialExit()
  { return Ogre::UTFString(L"Close tutorial"); }
  Ogre::UTFString TutorialReallyClose()
  { return Ogre::UTFString(L"Do you really want to close the tutorial?"); }
  Ogre::UTFString NextTutorial()
  { return Ogre::UTFString(L"Next tutorial"); }
  Ogre::UTFString OpinionIsOK()
  { return Ogre::UTFString(L" is OK."); }
  Ogre::UTFString OpinionDontLike1()
  { return Ogre::UTFString(L"I don't like "); }
  Ogre::UTFString OpinionDontLike2()
  { return Ogre::UTFString(L""); }
  Ogre::UTFString OpinionIsGreat()
  { return Ogre::UTFString(L" is a great company."); }
  Ogre::UTFString OpinionSawAd1()
  { return Ogre::UTFString(L"I just saw an ad from "); }
  Ogre::UTFString OpinionSawAd2()
  { return Ogre::UTFString(L""); }
  Ogre::UTFString OpinionPriceHigh1()
  { return Ogre::UTFString(L"The price from "); }
  Ogre::UTFString OpinionPriceHigh2()
  { return Ogre::UTFString(L" is too high"); }
  Ogre::UTFString OpinionPriceOK1()
  { return Ogre::UTFString(L"The price from "); }
  Ogre::UTFString OpinionPriceOK2()
  { return Ogre::UTFString(L" is OK"); }
  Ogre::UTFString DefaultCompanyName()
  { return Ogre::UTFString(L"Standard Electric Ltd"); }
  Ogre::UTFString QuestionCompanyName()
  { return Ogre::UTFString(L"How would you like to name your company?"); }
  Ogre::UTFString PleaseChooseCompanyName()
  { return Ogre::UTFString(L"Please choose a name for your company."); }
  Ogre::UTFString EndTutorial()
  { return Ogre::UTFString(L"End tutorial"); }
  Ogre::UTFString NextTutorialQuestion()
  { return Ogre::UTFString(L"End this tutorial and continue with the next one?"); }
  Ogre::UTFString Repairs1()
  { return Ogre::UTFString(L"Repair works will take "); }
  Ogre::UTFString Repairs2()
  { return Ogre::UTFString(L" month(s)."); }
  Ogre::UTFString IsBeingRepaired()
  { return Ogre::UTFString(L"Under repair..."); }
  Ogre::UTFString RepairsComplete1()
  { return Ogre::UTFString(L"Repairs for "); }
  Ogre::UTFString RepairsComplete2()
  { return Ogre::UTFString(L" completed."); }
  Ogre::UTFString Germany()
  { return Ogre::UTFString(L"Germany"); }
  Ogre::UTFString USA()
  { return Ogre::UTFString(L"USA"); }
  Ogre::UTFString GameAutoSaved()
  { return Ogre::UTFString(L"Game autosaved."); }
  Ogre::UTFString Accident()
  { return Ogre::UTFString(L"Powerplant\naccident!"); }
  Ogre::UTFString AccidentDetail1()
  { return Ogre::UTFString(L""); }
  Ogre::UTFString AccidentDetail2()
  { return Ogre::UTFString(L" was shut down today after an accident."); }
  Ogre::UTFString PPShutdown1()
  { return Ogre::UTFString(L"Your powerplant "); }
  Ogre::UTFString PPShutdown2()
  { return Ogre::UTFString(L" has been shut down after due to an accident."); }
  Ogre::UTFString CoalStockDepletedDetail()
  { return Ogre::UTFString(L"Your coal stock is empty. You can aquire coal by trading it or by conveying it for yourself by building a coal mine."); }
  Ogre::UTFString UraniumStockDepletedDetail()
  { return Ogre::UTFString(L"Your uranium stock is empty. You can aquire uranium by trading it or by conveying it for yourself by building a uranium mine."); }
  Ogre::UTFString GasStockDepletedDetail()
  { return Ogre::UTFString(L"Your gas stock is empty. You can aquire gas by trading it or by conveying it for yourself by building a gas extraction facility."); }
  Ogre::UTFString FreeTradesDetail()
  { return Ogre::UTFString(L"Free trades are available as a special action in the \"CEO\"-window."); }
  Ogre::UTFString PPAccidentDetail()
  { return Ogre::UTFString(L"If you want to avoid future accidents increase your safety measures."); }
  Ogre::UTFString PoleCollapsedDetail()
  { return Ogre::UTFString(L"If you want to avoid poles collapsing increase your supply lines maintenance spendings."); }
  Ogre::UTFString FullCapacityDetail()
  { return Ogre::UTFString(L"You might want to build new powerplants to satisfy the power demand."); }
  Ogre::UTFString DemoTime()
  { return Ogre::UTFString(L"Demo, time left:"); }
  Ogre::UTFString DemoNoSave()
  { return Ogre::UTFString(L"You can't save your game in the Demo."); }
  Ogre::UTFString DemoTimeUpCaption()
  { return Ogre::UTFString(L"Demotime expired"); }
  Ogre::UTFString DemoTimeUpText()
  { return Ogre::UTFString(L"Your demotime has expired. Close the game or visit\nwww.energietycoon.de to buy the game."); }
  std::vector< std::vector< Ogre::UTFString > > TutorialStep()
  {
    std::vector< std::vector< Ogre::UTFString > > lReturn;
    std::vector< Ogre::UTFString > lSteps1;
    std::vector< Ogre::UTFString > lSteps2;
    std::vector< Ogre::UTFString > lSteps3;

    lSteps1.push_back(L"Welcome to the energy tycoon tutorial.\nHere you will learn how to lead a successful energy company.\n\nClick 'Next' to continue the tutorial.");
    lSteps1.push_back(L"First, you should familiarize yourself with the camera.\nMove the mouse to the screen edges in order to move the viewing area.\nAlternatively, you can also hold down the right mouse button or use the arrow keys of your keyboard.\nTo zoom in or enlarge the viewing area, use your mouse wheel, the keys 'W' and 'S' or '+' and '-' in the bottom left of the screen.\nTo rotate the view to hold down the mouse wheel, use the keys 'A' and 'D', or '<-' and '->' in the bottom left of the screen.\n\nGet familiar with the controls, and then click 'Next' to continue the tutorial.");
    lSteps1.push_back(L"The first step in building up an energy company is to build a power plant. Click the button with the picture of a power plant to get a selection of available power plants. Select the button 'Small wind turbine' to build a power plant. You can now determine the place of construction for the plant.\nAs long as the outlines of the power plant are shown in green you're clear to build, but should it appear in red, you can not build at this place.\nFind a city now and build the wind farm in the area arround it.\n\nThen click 'Next' to continue the tutorial.");
    lSteps1.push_back(L"You have now built a power plant. However, it doesn't earn you any money, because it is not connected to a city.\nTo build a power line select the button with the picture of an electricity pylon.\nNow select the button 'Small power line'\nYou can now build a power line. If you move the mouse pointer over an object that you can connect to the power line the mouse pointer alters its appearance.\nNow connect your power plant and a city. By right-clicking, you can leave the building mode.\n\nWhen you're done, click 'Next' to continue the tutorial.");
    lSteps1.push_back(L"You now have a city that is connected to a power plant, customers can now buy your electricity. Click on the city in order to see how many customers you have and how much power they claim.\n\nWhether and how many customers you win or lose depends on many factors that you will get to know in the next tutorial.\n\nClick 'Next Tutorial' button to continue with it.");

    lSteps2.push_back(L"Welcome to the 2nd Energy tycoon tutorial.\nHere you will learn how to deal with your customers, you will also learn about your research and finance departments.\n\nClick 'Next' to continue the tutorial.");
    lSteps2.push_back(L"As you probably noticed, some power plants are already in your possession. One of the cities that you are supplying with electricity is Munich.\nClick the city to obtain detailed information about the city.\nYou can see now how many people live in the city, how many customers you have in this city, how much power they need and how much you provide. You can also set the electricity price and the advertising budget for this city.\nIts customer base is constantly changing.\n\nClick 'Next' to learn how to attract customers.");
    lSteps2.push_back(L"The main factors that influence your customer development are the electricity price, the advertising budget and image. You can set your price and advertising budget individually for each city, or globally by setting it in the finances window. Your image depends on several factors. Among other things, environmentally friendly power plants, and research projects. Change the advertising budget, or start a promotion in the 'Office' window and watch the customer development.\n\nClick 'Next' to continue with the tutorial.");
    lSteps2.push_back(L"It is very important that you constantly keep an eye on your finances to prevent a threat of insolvency in time.\nOpen the 'Finance' window, by clicking the button located to the left at the bottom of your screen, or alternatively the button left of your current balance at the top left of your screen.\nOn the first page you can set rates, service settings and much more. Each of these settings will affect your image, your finances, and the customer development.\n\nClick 'Next' to continue the tutorial.");
    lSteps2.push_back(L"On the second page of the financial window you will see a summary table of your finances.\nIt shows both the current month and the current year and the full last month and last year.\nOn the third page of the financial window is a graphical representation of your finances. You can hide or show individual graphs in by checking the boxes on the top of the graph.\nIf you happen to be short on cash and need money, you can go to the fourth page of the financial window and take out a loan.\n\nClick 'Next' to continue the tutorial.");
    lSteps2.push_back(L"One of the main divisions is your research department.\nWithout exploring new buildings, plants or technologies you can not achieve great success.\nOpen the window by clicking the research button on the bottom left of your screen. You now see a tree with all research projects that can be explored. To unlock all the projects you need to build a research center, also you can run just one research project at the same time, so think carefully what you want to research.\n\nClick 'Next' to continue the tutorial.");
    lSteps2.push_back(L"You've finished the second Energy Tycoon tutorial.\n\nIn the third and final tutorial you will learn how to deal with resources.\nYou can go to the next tutorial or continue playing the game by simply closing this window.");

    lSteps3.push_back(L"Welcome to the 3rd Energy tycoon tutorial.\nHere you will learn how to deal with resources.\n\nClick 'Next' to continue the tutorial.");
    lSteps3.push_back(L"You have two options how to gain resources:\nYou buy your raw materials on the international commodity market, or build your own mines in order to get raw materials from them.\nBoth options have advantages and disadvantages and you may not have all resources available on the map you're playing.\n\nClick 'Next' to learn how to deal with raw materials.");
    lSteps3.push_back(L"First, open the window 'Trade'. You can find the button on the bottom left of your screen.\nTop left you will see trading partners with whom you have still not signed a trade agreement, but who could possibly offer you very good deals.\nTop right you will see the trend in world prices of the 3 available raw materials. Trade agreements are concluded either at a fixed price or at world market prices, in this case, this graph is of great importance.\n\nClick on 'Next' to see how you conclude trade agreements and manage them.");
    lSteps3.push_back(L"In the lower part of the trading window, you will see the currently available offers and your ongoing obligations.\nThe available offers change every month.\nYou have the option of filtering the offers by commodity or by sale and purchase offers by using the box to the right.\nNow select any agreement and click 'Accept offer'. Simple trade actions are executed immediately and disappear from the list. Actions running over a long period will appear below.\n\nClick 'Next' to see how to sell your own goods.");
    lSteps3.push_back(L"To mine raw materials, you should first click the button 'Show resource locations' right of your minimap. Now you see what resources are located on the map and where they are.\n\nGreen:\tUranium\nRed:\tGas\nBlue:\tCoal\n\nTo learn how to build your own mines click 'Next'.");
    lSteps3.push_back(L"Now click on the 'Build resource gathering facilites' on the lower left of your screen. You will now see a selection of buildings you can build. There may not be all buildings available if you haven't researched them.\nSelect the 'Small uranium mine' and browse to the map for one uranium deposit.\nMineral deposits are indicated by small icons on which you can build mines. Now build your uranium mine.\n\nClick 'Next' to continue.");
    lSteps3.push_back(L"You have completet the Energy Tycoon tutorial.\nYou can close this window and continue playing or quit the game and start a free game or a mission.\n\nEnjoy!");

    lReturn.push_back(lSteps1);
    lReturn.push_back(lSteps2);
    lReturn.push_back(lSteps3);

    return lReturn;
  }
};

/*-----------------------------------------------------------------------------------------------*/

#endif // LANGEN_H
