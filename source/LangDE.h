// GENERATED - DO NOT EDIT
#ifndef LANGDE_H
#define LANGDE_H

/*-----------------------------------------------------------------------------------------------*/

//! German language
class LangDE : public StrLoc {
public:

  Ogre::UTFString LanguageCode()
  { return Ogre::UTFString(L"de"); }
  Ogre::UTFString GameTitle()
  { return Ogre::UTFString(L"Energie Tycoon"); }
  Ogre::UTFString MyGUILanguage()
  { return Ogre::UTFString(L"German"); }
  Ogre::UTFString MainMenuClose()
  { return Ogre::UTFString(L"Beenden"); }
  Ogre::UTFString MainMenuOptions()
  { return Ogre::UTFString(L"Optionen"); }
  Ogre::UTFString MainMenuNewGame()
  { return Ogre::UTFString(L"Neues Spiel"); }
  Ogre::UTFString MainMenuLoadGame()
  { return Ogre::UTFString(L"Spiel laden"); }
  Ogre::UTFString MainMenuCredits()
  { return Ogre::UTFString(L"Mitwirkende"); }
  Ogre::UTFString MainMenuSaveGame()
  { return Ogre::UTFString(L"Spiel speichern"); }
  Ogre::UTFString MainMenuResumeGame()
  { return Ogre::UTFString(L"Zurück zum Spiel"); }
  Ogre::UTFString MainMenuBack()
  { return Ogre::UTFString(L"Zurück zum Menü"); }
  Ogre::UTFString OptionsMenuDisplayMode()
  { return Ogre::UTFString(L"Grafikmodus:"); }
  Ogre::UTFString OptionsMenuGraphicsQuality()
  { return Ogre::UTFString(L"Grafikqualität:"); }
  Ogre::UTFString OptionsMenuNormal()
  { return Ogre::UTFString(L"Normal"); }
  Ogre::UTFString OptionsMenuHigh()
  { return Ogre::UTFString(L"Hoch"); }
  Ogre::UTFString OptionsMenuFullscreen()
  { return Ogre::UTFString(L"Vollbild"); }
  Ogre::UTFString LoadMenuLoad()
  { return Ogre::UTFString(L"Laden"); }
  Ogre::UTFString LoadMenuSaved()
  { return Ogre::UTFString(L"Gespeichert: "); }
  Ogre::UTFString LoadMenuMap()
  { return Ogre::UTFString(L"Karte: "); }
  Ogre::UTFString LoadMenuQuestion()
  { return Ogre::UTFString(L"Es läuft gerade ein Spiel. Wollen Sie wirklich dieses Spiel laden?"); }
  Ogre::UTFString LoadMenuSelect()
  { return Ogre::UTFString(L"Bitte wählen Sie einen Spielstand aus."); }
  Ogre::UTFString NewMenuStart()
  { return Ogre::UTFString(L"Spiel starten"); }
  Ogre::UTFString NewMenuEasy()
  { return Ogre::UTFString(L"Leicht"); }
  Ogre::UTFString NewMenuMedium()
  { return Ogre::UTFString(L"Normal"); }
  Ogre::UTFString NewMenuHard()
  { return Ogre::UTFString(L"Schwer"); }
  Ogre::UTFString NewMenuMissions()
  { return Ogre::UTFString(L"Missionen:"); }
  Ogre::UTFString NewMenuDifficulty()
  { return Ogre::UTFString(L"Schwierigkeit:"); }
  Ogre::UTFString NewMenuQuestion()
  { return Ogre::UTFString(L"Es läuft gerade ein Spiel. Wollen Sie wirklich ein neues Spiel starten?"); }
  Ogre::UTFString NewMenuSelect()
  { return Ogre::UTFString(L"Bitte wählen Sie eine Karte und eine Mission aus."); }
  Ogre::UTFString NewMenuMissionGoals()
  { return Ogre::UTFString(L"Missionsziele:"); }
  Ogre::UTFString NewMenuMap()
  { return Ogre::UTFString(L"Karten:"); }
  Ogre::UTFString SaveMenuSave()
  { return Ogre::UTFString(L"Speichern"); }
  Ogre::UTFString SaveMenuName()
  { return Ogre::UTFString(L"Bitte wählen Sie einen Namen für Ihren Spielstand."); }
  Ogre::UTFString SaveMenuOverwrite()
  { return Ogre::UTFString(L"Ein Spielstand mit diesem Namen existiert bereits. überschreiben?"); }
  Ogre::UTFString SaveMenuSaved()
  { return Ogre::UTFString(L"Spiel gespeichert."); }
  Ogre::UTFString Year()
  { return Ogre::UTFString(L"Jahr"); }
  Ogre::UTFString January()
  { return Ogre::UTFString(L"Januar"); }
  Ogre::UTFString February()
  { return Ogre::UTFString(L"Februar"); }
  Ogre::UTFString March()
  { return Ogre::UTFString(L"März"); }
  Ogre::UTFString April()
  { return Ogre::UTFString(L"April"); }
  Ogre::UTFString May()
  { return Ogre::UTFString(L"Mai"); }
  Ogre::UTFString June()
  { return Ogre::UTFString(L"Juni"); }
  Ogre::UTFString July()
  { return Ogre::UTFString(L"Juli"); }
  Ogre::UTFString August()
  { return Ogre::UTFString(L"August"); }
  Ogre::UTFString September()
  { return Ogre::UTFString(L"September"); }
  Ogre::UTFString October()
  { return Ogre::UTFString(L"Oktober"); }
  Ogre::UTFString November()
  { return Ogre::UTFString(L"November"); }
  Ogre::UTFString December()
  { return Ogre::UTFString(L"Dezember"); }
  Ogre::UTFString TooltipBuildPowerplant()
  { return Ogre::UTFString(L"Kraftwerke bauen"); }
  Ogre::UTFString TooltipBuildDistribution()
  { return Ogre::UTFString(L"Stromleitungen bauen"); }
  Ogre::UTFString TooltipBuildResource()
  { return Ogre::UTFString(L"Anlagen zur Resourcenförderung bauen"); }
  Ogre::UTFString TooltipBuildMisc()
  { return Ogre::UTFString(L"Firmenebäude bauen"); }
  Ogre::UTFString TooltipDemolish()
  { return Ogre::UTFString(L"Gebäude abreißen"); }
  Ogre::UTFString TooltipSp1()
  { return Ogre::UTFString(L"Keine Mitarbeiterausbildung für 6 Monate.\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceNoTraining")/1000)) + Ogre::UTFString(L"k€, -25% auf Betriebskosten)"); }
  Ogre::UTFString TooltipSp2()
  { return Ogre::UTFString(L"Illegale Müllbeseitigung für 6 Monate.\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceWasteDumping")/1000)) + Ogre::UTFString(L"k€, -25% auf Betriebskosten)"); }
  Ogre::UTFString TooltipSp3()
  { return Ogre::UTFString(L"Handlessteuer hinterziehen für die nächsten 4 Transaktionen.\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceTaxFraud")/1000)) + Ogre::UTFString(L"k€, -50% auf Handelsabschlüsse)"); }
  Ogre::UTFString TooltipSp4()
  { return Ogre::UTFString(L"Die zur Wahl antretenden Parteien schmieren.\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceBribe")/1000)) + Ogre::UTFString(L"k€, -80% Umweltschutzzahlungen für 4 Jahre)"); }
  Ogre::UTFString TooltipSp5()
  { return Ogre::UTFString(L"Kriegsanstrengungen der Regierung unterstützen.\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceWarSupport")/1000)) + Ogre::UTFString(L"k€, -50% auf alle Handelsaktionen für 1 Jahr)"); }
  Ogre::UTFString TooltipSp6()
  { return Ogre::UTFString(L"Preisabsprache mit den Konkurrenten.\n(") + Ogre::UTFString(toString(GameConfig::getInt("PricePriceRigging")/1000)) + Ogre::UTFString(L"k€, +50% Kundeneinkommen für 1 Jahr)"); }
  Ogre::UTFString TooltipSpNotAvailable()
  { return Ogre::UTFString(L"Zu dieser Spezialaktion haben Sie noch keinen Zugriff.\nBauen Sie das Hauptquartier um auf diese Aktionen zuzugreifen."); }
  Ogre::UTFString TooltipSp1Img()
  { return Ogre::UTFString(L"Keine Mitarbeiterausbildung aktiv"); }
  Ogre::UTFString TooltipSp2Img()
  { return Ogre::UTFString(L"Illegale Müllbeseitigung aktiv"); }
  Ogre::UTFString TooltipSp3ImgA()
  { return Ogre::UTFString(L"Handelssteuerhinterziehung aktiv ("); }
  Ogre::UTFString TooltipSp3ImgB()
  { return Ogre::UTFString(L" Handelsverträge übrig)"); }
  Ogre::UTFString TooltipSp4Img()
  { return Ogre::UTFString(L"Schmiergeld zur Wahl aktiv"); }
  Ogre::UTFString TooltipSp5Img()
  { return Ogre::UTFString(L"Kriegsunterstützung aktiv"); }
  Ogre::UTFString TooltipSp6Img()
  { return Ogre::UTFString(L"Preisabsprachen aktiv"); }
  Ogre::UTFString TooltipSpImgInactive()
  { return Ogre::UTFString(L"Spezialaktion inaktiv"); }
  Ogre::UTFString TooltipAd1()
  { return Ogre::UTFString(L"Anzeigenkampagne im Internet. (") + Ogre::UTFString(GameConfig::getString("AdInternetCost")) + Ogre::UTFString(L"k€)"); }
  Ogre::UTFString TooltipAd2()
  { return Ogre::UTFString(L"Zeitungsanzeigen. (") + Ogre::UTFString(GameConfig::getString("AdNewspaperCost")) + Ogre::UTFString(L"k€)"); }
  Ogre::UTFString TooltipAd3()
  { return Ogre::UTFString(L"Radiowerbung. (") + Ogre::UTFString(GameConfig::getString("AdRadioCost")) + Ogre::UTFString(L"k€)"); }
  Ogre::UTFString TooltipAd4()
  { return Ogre::UTFString(L"TV-Spots. (") + Ogre::UTFString(GameConfig::getString("AdTVCost")) + Ogre::UTFString(L"k€)"); }
  Ogre::UTFString TooltipAdNotAvailable()
  { return Ogre::UTFString(L"Auf diese Werbeform haben Sie noch keinen Zugriff.\nBauen Sie das PR-Gebäude um darauf Zugriff zu bekommen."); }
  Ogre::UTFString TooltipRe1()
  { return Ogre::UTFString(L"Großes Kohlekraftwerk erforschen."); }
  Ogre::UTFString TooltipRe2()
  { return Ogre::UTFString(L"Großes Windkraftwerk erforschen."); }
  Ogre::UTFString TooltipRe3()
  { return Ogre::UTFString(L"Verbesserung der Anzeigenkampagnen erforschen."); }
  Ogre::UTFString TooltipRe4()
  { return Ogre::UTFString(L"Biomassekraftwerk erforschen."); }
  Ogre::UTFString TooltipRe5()
  { return Ogre::UTFString(L"Effizienzsteigerung für Windkraftwerke erforschen."); }
  Ogre::UTFString TooltipRe6()
  { return Ogre::UTFString(L"Effizienzsteigerung für Kohlekraftwerke erforschen."); }
  Ogre::UTFString TooltipRe7()
  { return Ogre::UTFString(L"Offshorewindkraft erforschen."); }
  Ogre::UTFString TooltipRe8()
  { return Ogre::UTFString(L"Effizienzsteigerung für Gaskraftwerke erforschen."); }
  Ogre::UTFString TooltipRe9()
  { return Ogre::UTFString(L"Kleines Atomkraftwerk erforschen."); }
  Ogre::UTFString TooltipRe10()
  { return Ogre::UTFString(L"Großes Gaskraftwerk erforschen."); }
  Ogre::UTFString TooltipRe11()
  { return Ogre::UTFString(L"Große Kohleförderanlage erforschen."); }
  Ogre::UTFString TooltipRe12()
  { return Ogre::UTFString(L"Verringerte Betriebskosten erforschen."); }
  Ogre::UTFString TooltipRe13()
  { return Ogre::UTFString(L"Große Gasförderanlage erforschen."); }
  Ogre::UTFString TooltipRe14()
  { return Ogre::UTFString(L"CO2-Reduktion erforschen"); }
  Ogre::UTFString TooltipRe15()
  { return Ogre::UTFString(L"Effizienzsteigerung für Atomkraftwerke erforschen."); }
  Ogre::UTFString TooltipRe16()
  { return Ogre::UTFString(L"Großes Solarkraftwerk erforschen."); }
  Ogre::UTFString TooltipRe17()
  { return Ogre::UTFString(L"Aufwindkraftwerk erforschen."); }
  Ogre::UTFString TooltipRe18()
  { return Ogre::UTFString(L"Effizienzsteigerung für Solarkraftwerke erforschen."); }
  Ogre::UTFString TooltipRe19()
  { return Ogre::UTFString(L"Großes Atomkraftwerk erforschen."); }
  Ogre::UTFString TooltipRe20()
  { return Ogre::UTFString(L"Große Uranförderanlage erforschen."); }
  Ogre::UTFString TooltipRe21()
  { return Ogre::UTFString(L"Fusionskraftwerk erforschen."); }
  Ogre::UTFString TooltipReNotAvailable()
  { return Ogre::UTFString(L"Auf dieses Forschungsprojekt haben Sie noch keinen Zugriff.\nBauen Sie ein Forschungszentrum, um darauf Zugriff zu erhalten."); }
  Ogre::UTFString TooltipMenu()
  { return Ogre::UTFString(L"Zum Hauptmenü"); }
  Ogre::UTFString TooltipMission()
  { return Ogre::UTFString(L"Zeige Missionsziele"); }
  Ogre::UTFString TooltipFinances()
  { return Ogre::UTFString(L"Zeige Finanzen"); }
  Ogre::UTFString TooltipResearch()
  { return Ogre::UTFString(L"Gehe zur Forschung"); }
  Ogre::UTFString TooltipCEO()
  { return Ogre::UTFString(L"Gehe ins Büro"); }
  Ogre::UTFString TooltipTrade()
  { return Ogre::UTFString(L"Zeige Handel"); }
  Ogre::UTFString TooltipZoomIn()
  { return Ogre::UTFString(L"Vergrößern"); }
  Ogre::UTFString TooltipZoomOut()
  { return Ogre::UTFString(L"Verkleinern"); }
  Ogre::UTFString TooltipRotateLeft()
  { return Ogre::UTFString(L"Nach links drehen"); }
  Ogre::UTFString TooltipRotateRight()
  { return Ogre::UTFString(L"Nach rechts drehen"); }
  Ogre::UTFString TooltipSlower()
  { return Ogre::UTFString(L"Spielgeschwindigkeit verringern"); }
  Ogre::UTFString TooltipFaster()
  { return Ogre::UTFString(L"Spielgeschwindigkeit erhöhen"); }
  Ogre::UTFString TooltipPause()
  { return Ogre::UTFString(L"Spiel pausieren"); }
  Ogre::UTFString TooltipPowerNets()
  { return Ogre::UTFString(L"Zeige Netze"); }
  Ogre::UTFString TooltipMessageArchive()
  { return Ogre::UTFString(L"Zeige Nachrichtenarchiv"); }
  Ogre::UTFString TooltipImage()
  { return Ogre::UTFString(L"Image: "); }
  Ogre::UTFString TooltipReliability()
  { return Ogre::UTFString(L"Zuverlässigkeit: "); }
  Ogre::UTFString TooltipNormal()
  { return Ogre::UTFString(L"Normal"); }
  Ogre::UTFString TooltipHigh()
  { return Ogre::UTFString(L"Hoch"); }
  Ogre::UTFString TooltipLow()
  { return Ogre::UTFString(L"Schlecht"); }
  Ogre::UTFString TooltipCost()
  { return Ogre::UTFString(L"Preis: "); }
  Ogre::UTFString TooltipPower()
  { return Ogre::UTFString(L"Leistung: "); }
  Ogre::UTFString TooltipOutput()
  { return Ogre::UTFString(L"Förderung: "); }
  Ogre::UTFString TooltipRods()
  { return Ogre::UTFString(L"Brennst."); }
  Ogre::UTFString TooltipKCubicFeet()
  { return Ogre::UTFString(L"kKubikm."); }
  Ogre::UTFString TooltipKTons()
  { return Ogre::UTFString(L"KTn."); }
  Ogre::UTFString TooltipPerWeek()
  { return Ogre::UTFString(L"/Woche"); }
  Ogre::UTFString TooltipCapacity()
  { return Ogre::UTFString(L"Max. Last: "); }
  Ogre::UTFString TooltipCoalSmall()
  { return Ogre::UTFString(L"Kleines Kohlekraftwerk"); }
  Ogre::UTFString TooltipCoalLarge()
  { return Ogre::UTFString(L"Großes Kohlekraftwerk"); }
  Ogre::UTFString TooltipGasSmall()
  { return Ogre::UTFString(L"Kleines Gaskraftwerk"); }
  Ogre::UTFString TooltipGasLarge()
  { return Ogre::UTFString(L"Großes Gaskraftwerk"); }
  Ogre::UTFString TooltipNuclearSmall()
  { return Ogre::UTFString(L"Kleines Atomkraftwerk"); }
  Ogre::UTFString TooltipNuclearLarge()
  { return Ogre::UTFString(L"Großes Atomkraftwerk"); }
  Ogre::UTFString TooltipNuclearFusion()
  { return Ogre::UTFString(L"Fusionskraftwerk"); }
  Ogre::UTFString TooltipSolarSmall()
  { return Ogre::UTFString(L"Kleines Solarkraftwerk"); }
  Ogre::UTFString TooltipSolarLarge()
  { return Ogre::UTFString(L"Großes Solarkraftwerk"); }
  Ogre::UTFString TooltipSolarUpdraft()
  { return Ogre::UTFString(L"Aufwindkraftwerk"); }
  Ogre::UTFString TooltipWindSmall()
  { return Ogre::UTFString(L"Kleine Windkraftanlage"); }
  Ogre::UTFString TooltipWindLarge()
  { return Ogre::UTFString(L"Große Windkraftanlage"); }
  Ogre::UTFString TooltipWindOffshore()
  { return Ogre::UTFString(L"Offshore-Windkraftanlage"); }
  Ogre::UTFString TooltipBio()
  { return Ogre::UTFString(L"Biomassekraftwerk"); }
  Ogre::UTFString TooltipHeadquarters()
  { return Ogre::UTFString(L"Firmenhauptsitz"); }
  Ogre::UTFString TooltipResearchBuilding()
  { return Ogre::UTFString(L"Forschungszentr."); }
  Ogre::UTFString TooltipPR()
  { return Ogre::UTFString(L"PR-Zentrum"); }
  Ogre::UTFString TooltipCoalRSmall()
  { return Ogre::UTFString(L"Kleine Kohlemine"); }
  Ogre::UTFString TooltipCoalRLarge()
  { return Ogre::UTFString(L"Große Kohlemine"); }
  Ogre::UTFString TooltipGasRSmall()
  { return Ogre::UTFString(L"Kleine Gasförderanlage"); }
  Ogre::UTFString TooltipGasRLarge()
  { return Ogre::UTFString(L"Große Gasförderanlage"); }
  Ogre::UTFString TooltipUraniumRSmall()
  { return Ogre::UTFString(L"Kleine Uranmine"); }
  Ogre::UTFString TooltipUraniumRLarge()
  { return Ogre::UTFString(L"Große Uranmine"); }
  Ogre::UTFString TooltipLineSmall()
  { return Ogre::UTFString(L"Kleine Stromleitung"); }
  Ogre::UTFString TooltipLineLarge()
  { return Ogre::UTFString(L"Große Stromleitung"); }
  Ogre::UTFString NoMoney()
  { return Ogre::UTFString(L"Sie haben nicht genügend Geld!"); }
  Ogre::UTFString NoResources()
  { return Ogre::UTFString(L"Sie haben nicht genügend Rohstoffe!"); }
  Ogre::UTFString TooltipRepair()
  { return Ogre::UTFString(L"Diese Gebäude reparieren"); }
  Ogre::UTFString TooltipMMCapacity()
  { return Ogre::UTFString(L"Zeige Kapazitätsinformationen"); }
  Ogre::UTFString TooltipMMDefault()
  { return Ogre::UTFString(L"Standardansicht"); }
  Ogre::UTFString TooltipMMSupply()
  { return Ogre::UTFString(L"Zeige Versorgungsinformationen"); }
  Ogre::UTFString TooltipMMResources()
  { return Ogre::UTFString(L"Zeige Rohstoffvorkommen"); }
  Ogre::UTFString WantToQuit()
  { return Ogre::UTFString(L"Wirklich beenden?"); }
  Ogre::UTFString GoalNone()
  { return Ogre::UTFString(L"Keine"); }
  Ogre::UTFString CitySeeNet()
  { return Ogre::UTFString(L"... (siehe Netzübers.)"); }
  Ogre::UTFString CityNone()
  { return Ogre::UTFString(L"Keine"); }
  Ogre::UTFString PPOutput()
  { return Ogre::UTFString(L"Leistung: "); }
  Ogre::UTFString PPUsed()
  { return Ogre::UTFString(L"Last: "); }
  Ogre::UTFString PPConnected()
  { return Ogre::UTFString(L"Verbunden mit: "); }
  Ogre::UTFString Condition()
  { return Ogre::UTFString(L"Zustand: "); }
  Ogre::UTFString BuiltYear()
  { return Ogre::UTFString(L"Baujahr: "); }
  Ogre::UTFString WeeklyCost()
  { return Ogre::UTFString(L"Kosten/Woche: "); }
  Ogre::UTFString DamagedNeedsRepair()
  { return Ogre::UTFString(L"Reparaturbedürftig"); }
  Ogre::UTFString Loading()
  { return Ogre::UTFString(L"Lade..."); }
  Ogre::UTFString LoadingTerrain()
  { return Ogre::UTFString(L"Karte"); }
  Ogre::UTFString LoadingTreesResources()
  { return Ogre::UTFString(L"Verteile Vegetation & Resourcen"); }
  Ogre::UTFString LoadingCompanyData()
  { return Ogre::UTFString(L"Firmendaten"); }
  Ogre::UTFString LoadingPopulace()
  { return Ogre::UTFString(L"Bevölkerung"); }
  Ogre::UTFString COCoal()
  { return Ogre::UTFString(L"Kohle: "); }
  Ogre::UTFString COGas()
  { return Ogre::UTFString(L"Gas: "); }
  Ogre::UTFString COUranium()
  { return Ogre::UTFString(L"Uran: "); }
  Ogre::UTFString COKCubicShort()
  { return Ogre::UTFString(L"KKM"); }
  Ogre::UTFString COKTonShort()
  { return Ogre::UTFString(L"KT"); }
  Ogre::UTFString CORodShort()
  { return Ogre::UTFString(L"BS"); }
  Ogre::UTFString CONet()
  { return Ogre::UTFString(L"Netz"); }
  Ogre::UTFString COCities()
  { return Ogre::UTFString(L"Städte: "); }
  Ogre::UTFString COPowerplants()
  { return Ogre::UTFString(L"Kraftwerke: "); }
  Ogre::UTFString COSelectCity()
  { return Ogre::UTFString(L"Für Details Stadt auswählen."); }
  Ogre::UTFString COSelectPP()
  { return Ogre::UTFString(L"Für Details Kraftwerk auswählen."); }
  Ogre::UTFString CONetEfficiency()
  { return Ogre::UTFString(L"Netzauslastung: "); }
  Ogre::UTFString CONetAvailable()
  { return Ogre::UTFString(L"Leistung verfügb.: "); }
  Ogre::UTFString CONetNeeded()
  { return Ogre::UTFString(L"Leistung genutzt: "); }
  Ogre::UTFString Residents()
  { return Ogre::UTFString(L"Einwohner: "); }
  Ogre::UTFString PowerNeeded()
  { return Ogre::UTFString(L"Von Kunden benöt.:"); }
  Ogre::UTFString PowerSuppliedByYou()
  { return Ogre::UTFString(L"Zur Verfügung:"); }
  Ogre::UTFString Customers()
  { return Ogre::UTFString(L"Kunden: "); }
  Ogre::UTFString SuppliedBy()
  { return Ogre::UTFString(L"Versorgt durch: "); }
  Ogre::UTFString Price()
  { return Ogre::UTFString(L"Preis:"); }
  Ogre::UTFString UseGlobalSettings()
  { return Ogre::UTFString(L"Globale Einst. verwenden"); }
  Ogre::UTFString AdsBudget()
  { return Ogre::UTFString(L"Werbeetat:"); }
  Ogre::UTFString Week()
  { return Ogre::UTFString(L"Woche"); }
  Ogre::UTFString PPMaintenance()
  { return Ogre::UTFString(L"Wartung:"); }
  Ogre::UTFString Nothing()
  { return Ogre::UTFString(L"Nichts"); }
  Ogre::UTFString CurrentlyResearching()
  { return Ogre::UTFString(L"Es wird erforscht:"); }
  Ogre::UTFString ResearchCaption()
  { return Ogre::UTFString(L"Forschung"); }
  Ogre::UTFString ResearchDone()
  { return Ogre::UTFString(L"Dieses Forschungsprojekt wurde bereits abgeschlossen."); }
  Ogre::UTFString ResearchOne()
  { return Ogre::UTFString(L"Sie können nur ein Forschungsprjekt gleichzeitig betreiben."); }
  Ogre::UTFString ResearchNoAccess()
  { return Ogre::UTFString(L"Auf diese Forschungsprojekt haben Sie noch keinen Zugriff."); }
  Ogre::UTFString ResearchNoAccessTree()
  { return Ogre::UTFString(L"Auf diese Forschungsprojekt haben Sie noch keinen Zugriff."); }
  Ogre::UTFString StartResearchA()
  { return Ogre::UTFString(L"Forschungsprojekt '"); }
  Ogre::UTFString StartResearchB()
  { return Ogre::UTFString(L"' starten? ("); }
  Ogre::UTFString Re1()
  { return Ogre::UTFString(L"Großes Kohlekraftwerk"); }
  Ogre::UTFString Re2()
  { return Ogre::UTFString(L"Große Windkraftanlage"); }
  Ogre::UTFString Re3()
  { return Ogre::UTFString(L"Verbesserung der Anzeigenkampagnen"); }
  Ogre::UTFString Re4()
  { return Ogre::UTFString(L"Biomassekraftwerk"); }
  Ogre::UTFString Re5()
  { return Ogre::UTFString(L"Effizienzsteigerung für Windkraftanlagen"); }
  Ogre::UTFString Re6()
  { return Ogre::UTFString(L"Effizienzsteigerung für Kohlekraftwerke"); }
  Ogre::UTFString Re7()
  { return Ogre::UTFString(L"Offshore-Windkraftanlage"); }
  Ogre::UTFString Re8()
  { return Ogre::UTFString(L"Effizienzsteigerung für Gaskraftwerke"); }
  Ogre::UTFString Re9()
  { return Ogre::UTFString(L"Kleines Atomkraftwerk"); }
  Ogre::UTFString Re10()
  { return Ogre::UTFString(L"Großes Gaskraftwerk"); }
  Ogre::UTFString Re11()
  { return Ogre::UTFString(L"Große Kohlemine"); }
  Ogre::UTFString Re12()
  { return Ogre::UTFString(L"Reduzierung der Betriebskosten"); }
  Ogre::UTFString Re13()
  { return Ogre::UTFString(L"Große Gasförderanlage"); }
  Ogre::UTFString Re14()
  { return Ogre::UTFString(L"CO2-Reduktion"); }
  Ogre::UTFString Re15()
  { return Ogre::UTFString(L"Effizienzsteigerung für Atomkraftwerke"); }
  Ogre::UTFString Re16()
  { return Ogre::UTFString(L"Großes Solarkraftwerk"); }
  Ogre::UTFString Re17()
  { return Ogre::UTFString(L"Aufwindkraftwerk"); }
  Ogre::UTFString Re18()
  { return Ogre::UTFString(L"Effizienzsteigerung für Solarkraftwerke"); }
  Ogre::UTFString Re19()
  { return Ogre::UTFString(L"Großes Atomkraftwerk"); }
  Ogre::UTFString Re20()
  { return Ogre::UTFString(L"Große Uranmine"); }
  Ogre::UTFString Re21()
  { return Ogre::UTFString(L"Fusionskraftwerk"); }
  Ogre::UTFString CEOCaption()
  { return Ogre::UTFString(L"Büro"); }
  Ogre::UTFString CEOOffice()
  { return Ogre::UTFString(L"Büro"); }
  Ogre::UTFString CustomersOpinionsFrom()
  { return Ogre::UTFString(L"Kundenmeinungen aus:"); }
  Ogre::UTFString NoOpinions()
  { return Ogre::UTFString(L"Keine Kundenmeinungen"); }
  Ogre::UTFString AllCities()
  { return Ogre::UTFString(L"Alle Städte"); }
  Ogre::UTFString CEOCustomers()
  { return Ogre::UTFString(L"Kunden"); }
  Ogre::UTFString Ad1Question()
  { return Ogre::UTFString(L"Internetwerbung schalten für ") + Ogre::UTFString(GameConfig::getString("AdInternetCost")) + Ogre::UTFString(L"k€?"); }
  Ogre::UTFString Ad2Question()
  { return Ogre::UTFString(L"Zeitungsanzeigen kaufen für ") + Ogre::UTFString(GameConfig::getString("AdNewspaperCost")) + Ogre::UTFString(L"k€?"); }
  Ogre::UTFString Ad3Question()
  { return Ogre::UTFString(L"Radiowerbung schalten für ") + Ogre::UTFString(GameConfig::getString("AdRadioCost")) + Ogre::UTFString(L"k€?"); }
  Ogre::UTFString Ad4Question()
  { return Ogre::UTFString(L"TV-Spots kaufen für ") + Ogre::UTFString(GameConfig::getString("AdTVCost")) + Ogre::UTFString(L"k€?"); }
  Ogre::UTFString AdNoAccess()
  { return Ogre::UTFString(L"Auf diese Werbeform haben Sie noch keinen Zugriff. Bauen Sie ein PR-Gebäude um Zugriff zu erhalten."); }
  Ogre::UTFString Sp1Question()
  { return Ogre::UTFString(L"Keine Mitarbeiterausbildung für 6 Monate? (") + Ogre::UTFString(toString(GameConfig::getInt("PriceNoTraining")/1000)) + Ogre::UTFString(L"k€, -25% auf Betriebskosten)"); }
  Ogre::UTFString Sp2Question()
  { return Ogre::UTFString(L"Illegale Müllbeseitigung für 6 Monate? (") + Ogre::UTFString(toString(GameConfig::getInt("PriceWasteDumping")/1000)) + Ogre::UTFString(L"k€, -25% auf Betriebskosten)"); }
  Ogre::UTFString Sp3Question()
  { return Ogre::UTFString(L"Handlessteuer hinterziehen für die nächsten 4 Transaktionen?\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceTaxFraud")/1000)) + Ogre::UTFString(L"k€, -50% auf Handelsabschlüsse)"); }
  Ogre::UTFString Sp4Question()
  { return Ogre::UTFString(L"Die zur Wahl antretenden Parteien schmieren?\n(") + Ogre::UTFString(toString(GameConfig::getInt("PriceBribe")/1000)) + Ogre::UTFString(L"k€, -80% Umweltschutzzahlungen für 4 Jahre)"); }
  Ogre::UTFString Sp5Question()
  { return Ogre::UTFString(L"Kriegsanstrengungen der Regierung unterstützen? (") + Ogre::UTFString(toString(GameConfig::getInt("PriceWarSupport")/1000)) + Ogre::UTFString(L"k€, -50% auf alle Handelsaktionen für 1 Jahr)"); }
  Ogre::UTFString Sp6Question()
  { return Ogre::UTFString(L"Preisabsprache mit den Konkurrenten? (") + Ogre::UTFString(toString(GameConfig::getInt("PricePriceRigging")/1000)) + Ogre::UTFString(L"k€, +50% Kundeneinkommen für 1 Jahr)"); }
  Ogre::UTFString SpStarted()
  { return Ogre::UTFString(L"Diese Spezialaktion läuft bereits."); }
  Ogre::UTFString SpNoAccess()
  { return Ogre::UTFString(L"Auf diese Spezialaktion haben Sie noch keinen Zugriff. Bauen Sie einen Firmenhauptsitz um Zugriff zu erhalten."); }
  Ogre::UTFString MissionCaption()
  { return Ogre::UTFString(L"Missionsziele"); }
  Ogre::UTFString TickerCaption()
  { return Ogre::UTFString(L"Nachrichtenarchiv"); }
  Ogre::UTFString TickerEmpty()
  { return Ogre::UTFString(L"Es befinden sich keine Nachrichten im Archiv."); }
  Ogre::UTFString BuildingNoAccess()
  { return Ogre::UTFString(L"Dieses Gebäude können Sie noch nicht bauen."); }
  Ogre::UTFString NoPowerNet()
  { return Ogre::UTFString(L"Sie haben noch kein Stromnetz."); }
  Ogre::UTFString InterestRate()
  { return Ogre::UTFString(L"Zinssatz: "); }
  Ogre::UTFString MonthlyCharge()
  { return Ogre::UTFString(L"Monatl. Rate: "); }
  Ogre::UTFString NewMonthlyCharge()
  { return Ogre::UTFString(L"Neue monatl. Rate: "); }
  Ogre::UTFString RepairA()
  { return Ogre::UTFString(L"Repariere "); }
  Ogre::UTFString RepairB()
  { return Ogre::UTFString(L" für "); }
  Ogre::UTFString UniqueHeadquarters()
  { return Ogre::UTFString(L"Sie können nur einen Firmenhauptsitz bauen."); }
  Ogre::UTFString UniqueResearch()
  { return Ogre::UTFString(L"Sie können nur ein Forschungszentrum bauen."); }
  Ogre::UTFString UniquePR()
  { return Ogre::UTFString(L"Sie können nur ein PR-Gebäude bauen."); }
  Ogre::UTFString Sp1Started()
  { return Ogre::UTFString(L"Spezialaktion 'Keine Mitarbeiterausbildung' gestartet."); }
  Ogre::UTFString Sp2Started()
  { return Ogre::UTFString(L"Spezialaktion 'Illegale Müllbeseitigung' gestartet."); }
  Ogre::UTFString Sp3Started()
  { return Ogre::UTFString(L"Spezialaktion 'Handelssteuerhinterziehung' gestartet."); }
  Ogre::UTFString Sp4Started()
  { return Ogre::UTFString(L"Spezialaktion 'Schmiergeld zur Wahl' gestartet."); }
  Ogre::UTFString Sp5Started()
  { return Ogre::UTFString(L"Spezialaktion 'Kriegsunterstützung' gestartet."); }
  Ogre::UTFString Sp6Started()
  { return Ogre::UTFString(L"Spezialaktion 'Preisabsprachen' gestartet."); }
  Ogre::UTFString Sp1Detail()
  { return Ogre::UTFString(L"Ihre Mitarbeiter werden für 6 Monate nicht fortgebildet."); }
  Ogre::UTFString Sp2Detail()
  { return Ogre::UTFString(L"Durch illegale Müllbeseitung können Sie viel Geld sparen, werden Sie jedoch erwischt leidet Ihr Image."); }
  Ogre::UTFString Sp3Detail()
  { return Ogre::UTFString(L"Durch Handelssteuerhinterziehung können Sie 4 mal ohne Steuern zu bezahlen handeln."); }
  Ogre::UTFString Sp4Detail()
  { return Ogre::UTFString(L"Für die nächste Legislaturperiode müssen Sie keine Umweltschutzzahlungen leisten. Sollten Sie erwischt werden leidet Ihr Image."); }
  Ogre::UTFString Sp5Detail()
  { return Ogre::UTFString(L"Sie können eine ganze Legislaturperiode steuerfrei handeln, sollten Sie jedoch erwischt werden wird Ihr Image dahin sein."); }
  Ogre::UTFString Sp6Detail()
  { return Ogre::UTFString(L"Ihre Einnahmen werden sich verdoppeln, solltetn Sie jedoch erwischt werden wird Ihr Image sehr leiden."); }
  Ogre::UTFString Ad1Started()
  { return Ogre::UTFString(L"Internetwerbung gestartet."); }
  Ogre::UTFString Ad2Started()
  { return Ogre::UTFString(L"Zeitungsanzeigen verö") + Ogre::UTFString(L"ffentlicht."); }
  Ogre::UTFString Ad3Started()
  { return Ogre::UTFString(L"Radiowerbung gestartet."); }
  Ogre::UTFString Ad4Started()
  { return Ogre::UTFString(L"TV-Spots gestartet."); }
  Ogre::UTFString Ad1Detail()
  { return Ogre::UTFString(L"Werbekampagnen verbessern Ihr Image, allerdings nur für einen kurzen Zeitraum."); }
  Ogre::UTFString Ad2Detail()
  { return Ogre::UTFString(L"Werbekampagnen verbessern Ihr Image, allerdings nur für einen kurzen Zeitraum."); }
  Ogre::UTFString Ad3Detail()

  { return Ogre::UTFString(L"Werbekampagnen verbessern Ihr Image, allerdings nur für einen kurzen Zeitraum."); }
  Ogre::UTFString Ad4Detail()
  { return Ogre::UTFString(L"Werbekampagnen verbessern Ihr Image, allerdings nur für einen kurzen Zeitraum."); }
  Ogre::UTFString OfferNA()
  { return Ogre::UTFString(L"Dieses Angebot ist nicht mehr verfügbar!"); }
  Ogre::UTFString ObligationNA()
  { return Ogre::UTFString(L"Der Handelsvertrag ist ausgelaufen oder wurde storniert!"); }
  Ogre::UTFString SelectOffer()
  { return Ogre::UTFString(L"Bitte wählen Sie ein Handelsangebot aus."); }
  Ogre::UTFString SelectObligation()
  { return Ogre::UTFString(L"Bitte wählen Sie einen Handelsvertrag aus."); }
  Ogre::UTFString SelectPartner()
  { return Ogre::UTFString(L"Bitte wählen Sie einen Handelspartner aus."); }
  Ogre::UTFString CancelObligation()
  { return Ogre::UTFString(L"Wollen Sie diesen Vertrag wirklich kündigen?"); }
  Ogre::UTFString SignTradeA()
  { return Ogre::UTFString(L"Wollen Sie mit "); }
  Ogre::UTFString SignTradeB()
  { return Ogre::UTFString(L" ein Handelsabkommen für "); }
  Ogre::UTFString SignTradeC()
  { return Ogre::UTFString(L"abschließen?"); }
  Ogre::UTFString Coal()
  { return Ogre::UTFString(L"Kohle"); }
  Ogre::UTFString Gas()
  { return Ogre::UTFString(L"Gas"); }
  Ogre::UTFString Uranium()
  { return Ogre::UTFString(L"Uran"); }
  Ogre::UTFString CoalS()
  { return Ogre::UTFString(L"Kohle"); }
  Ogre::UTFString GasS()
  { return Ogre::UTFString(L"Gas"); }
  Ogre::UTFString UraniumS()
  { return Ogre::UTFString(L"Uran"); }
  Ogre::UTFString Sell()
  { return Ogre::UTFString(L"Verkauf"); }
  Ogre::UTFString Buy()
  { return Ogre::UTFString(L"Ankauf"); }
  Ogre::UTFString SelectLoan()
  { return Ogre::UTFString(L"Bitte wählen Sie einen laufenden Kredit aus."); }
  Ogre::UTFString ChooseSum()
  { return Ogre::UTFString(L"Bitte legen Sie ein Summe fest."); }
  Ogre::UTFString RepayA()
  { return Ogre::UTFString(L"Wollen Sie "); }
  Ogre::UTFString RepayB()
  { return Ogre::UTFString(L"k€ von dem ausgewählten Kredit zurückzahlen?"); }
  Ogre::UTFString TakeoutA()
  { return Ogre::UTFString(L"Wollen Sie einen Kredit von "); }
  Ogre::UTFString TakeoutB()
  { return Ogre::UTFString(L"k€ mit einer Laufzeit von "); }
  Ogre::UTFString TakeoutC()
  { return Ogre::UTFString(L" Jahren aufnehmen?"); }
  Ogre::UTFString FinancesCaption()
  { return Ogre::UTFString(L"Finanzen"); }
  Ogre::UTFString PriceText()
  { return Ogre::UTFString(L"Globaler Strompreis:"); }
  Ogre::UTFString AdsText()
  { return Ogre::UTFString(L"Globaler Werbeetat:"); }
  Ogre::UTFString PriceUnit()
  { return Ogre::UTFString(L"cent/kWh"); }
  Ogre::UTFString AdsUnit()
  { return Ogre::UTFString(L"EUR pro Woche und Stadt"); }
  Ogre::UTFString MaintenanceText()
  { return Ogre::UTFString(L"Anlagenwartung:"); }
  Ogre::UTFString EnvText()
  { return Ogre::UTFString(L"Umweltschutzzahlungen:"); }
  Ogre::UTFString EnvUnit()
  { return Ogre::UTFString(L"kEUR/Monat"); }
  Ogre::UTFString ContractText()
  { return Ogre::UTFString(L"Mindestvertragslaufzeit:"); }
  Ogre::UTFString SupportText()
  { return Ogre::UTFString(L"Kundenbetreuung:"); }
  Ogre::UTFString SupplyText()
  { return Ogre::UTFString(L"Wartung der Stromleitungen:"); }
  Ogre::UTFString SafetyText()
  { return Ogre::UTFString(L"Sicherheitsmaßnahmen:"); }
  Ogre::UTFString TermNone()
  { return Ogre::UTFString(L"keine"); }
  Ogre::UTFString Term12()
  { return Ogre::UTFString(L"12 Monate"); }
  Ogre::UTFString Term24()
  { return Ogre::UTFString(L"24 Monate"); }
  Ogre::UTFString SupportPoor()
  { return Ogre::UTFString(L"schlecht"); }
  Ogre::UTFString SupportNormal()
  { return Ogre::UTFString(L"normal"); }
  Ogre::UTFString SupportExtensive()
  { return Ogre::UTFString(L"hervorragend"); }
  Ogre::UTFString EmployeeText()
  { return Ogre::UTFString(L"Mitarbeiterausbildung:"); }
  Ogre::UTFString SafetyNone()
  { return Ogre::UTFString(L"keine"); }
  Ogre::UTFString SafetyNormal()
  { return Ogre::UTFString(L"normal"); }
  Ogre::UTFString SafetyHeavy()
  { return Ogre::UTFString(L"erheblich"); }
  Ogre::UTFString EmployeeNone()
  { return Ogre::UTFString(L"keine"); }
  Ogre::UTFString EmployeeBasic()
  { return Ogre::UTFString(L"normal"); }
  Ogre::UTFString EmployeeExtensive()
  { return Ogre::UTFString(L"ausführlich"); }
  Ogre::UTFString SalesCaption()
  { return Ogre::UTFString(L"Verkauf"); }
  Ogre::UTFString IncomeCaption()
  { return Ogre::UTFString(L"Einkommen/Ausgabe"); }
  Ogre::UTFString CurrentMonthText()
  { return Ogre::UTFString(L"Aktueller Monat:"); }
  Ogre::UTFString LastMonthText()
  { return Ogre::UTFString(L"Letzter Monat:"); }
  Ogre::UTFString CurrentYearText()
  { return Ogre::UTFString(L"Aktuelles Jahr:"); }
  Ogre::UTFString LastYearText()
  { return Ogre::UTFString(L"Letztes Jahr:"); }
  Ogre::UTFString TotalText()
  { return Ogre::UTFString(L"Komplett:"); }
  Ogre::UTFString FLoan()
  { return Ogre::UTFString(L"Kredittilgung:"); }
  Ogre::UTFString FSafety()
  { return Ogre::UTFString(L"Sicherheit/Ausbildung:"); }
  Ogre::UTFString FAdvertising()
  { return Ogre::UTFString(L"Werbung:"); }
  Ogre::UTFString FMaintenance()
  { return Ogre::UTFString(L"Wartung:"); }
  Ogre::UTFString FOperation()
  { return Ogre::UTFString(L"Betriebskosten:"); }
  Ogre::UTFString FEnvironment()
  { return Ogre::UTFString(L"Umweltschutz:"); }
  Ogre::UTFString FResearch()
  { return Ogre::UTFString(L"Forschungsausgaben:"); }
  Ogre::UTFString FBuilding()
  { return Ogre::UTFString(L"Bauprojekte:"); }
  Ogre::UTFString FIncome()
  { return Ogre::UTFString(L"Einnahmen:"); }
  Ogre::UTFString FSpecial()
  { return Ogre::UTFString(L"Spezialausgaben:"); }
  Ogre::UTFString FTrade()
  { return Ogre::UTFString(L"Handel:"); }
  Ogre::UTFString ChartTab()
  { return Ogre::UTFString(L"Graphen"); }
  Ogre::UTFString ExpensesCheck()
  { return Ogre::UTFString(L"Ausgaben"); }
  Ogre::UTFString IncomeCheck()
  { return Ogre::UTFString(L"Einkommen"); }
  Ogre::UTFString FundsCheck()
  { return Ogre::UTFString(L"Vermögen"); }
  Ogre::UTFString NetWorthCheck()
  { return Ogre::UTFString(L"Firmenwert"); }
  Ogre::UTFString LoanTab()
  { return Ogre::UTFString(L"Kredite"); }
  Ogre::UTFString TakeOutButton()
  { return Ogre::UTFString(L"Kredit aufnehmen"); }
  Ogre::UTFString RepaymentButton()
  { return Ogre::UTFString(L"Sonderzahlung"); }
  Ogre::UTFString NoLoansText()
  { return Ogre::UTFString(L"Keine laufenden Kredite"); }
  Ogre::UTFString CurrentLoansText()
  { return Ogre::UTFString(L"Laufende Kredite:"); }
  Ogre::UTFString AmountText()
  { return Ogre::UTFString(L"Höhe:"); }
  Ogre::UTFString PeriodUnit()
  { return Ogre::UTFString(L"Jahre"); }
  Ogre::UTFString PeriodText()
  { return Ogre::UTFString(L"Laufzeit:"); }
  Ogre::UTFString TradeCaption()
  { return Ogre::UTFString(L"Handel"); }
  Ogre::UTFString TradingPartnersText()
  { return Ogre::UTFString(L"Handelspartner:"); }
  Ogre::UTFString NoTradingPartners()
  { return Ogre::UTFString(L"Keine Handelspartner übrig"); }
  Ogre::UTFString WorldMarketText()
  { return Ogre::UTFString(L"Weltmarktpreise:"); }
  Ogre::UTFString SignButton()
  { return Ogre::UTFString(L"Handelabkommen abschließen"); }
  Ogre::UTFString NoTradeOffers()
  { return Ogre::UTFString(L"In diesem Monat gibt es keine Angebote."); }
  Ogre::UTFString OffersText()
  { return Ogre::UTFString(L"Angebote:"); }
  Ogre::UTFString AcceptOfferButton()
  { return Ogre::UTFString(L"Angebot annehmen"); }
  Ogre::UTFString NoObligations()
  { return Ogre::UTFString(L"Sie haben keine laufenden Verträge"); }
  Ogre::UTFString CancelOfferButton()
  { return Ogre::UTFString(L"Auflösen"); }
  Ogre::UTFString CurrentObligationsText()
  { return Ogre::UTFString(L"Laufende\nVerträge"); }
  Ogre::UTFString LoanFormattedA()
  { return Ogre::UTFString(L"Betrag übrig: "); }
  Ogre::UTFString LoanFormattedB()
  { return Ogre::UTFString(L" k€, Raten übrig: "); }
  Ogre::UTFString LoanFormattedC()
  { return Ogre::UTFString(L" k€ für "); }
  Ogre::UTFString LoanFormattedD()
  { return Ogre::UTFString(L" Monate."); }
  Ogre::UTFString FreeTradesLeft()
  { return Ogre::UTFString(L" steuerfreie Handelsaktionen übrig."); }
  Ogre::UTFString FreeTradesGone()
  { return Ogre::UTFString(L"Sie habe alle steuerfrei Handelsaktionen aufgebraucht."); }
  Ogre::UTFString DamagedResourceMessageA()
  { return Ogre::UTFString(L"Ihre Anlage "); }
  Ogre::UTFString DamagedResourceMessageB()
  { return Ogre::UTFString(L" wurde wegen eines technischen Fehlers heruntergefahren."); }
  Ogre::UTFString DamagedPPMessageA()
  { return Ogre::UTFString(L"Ihr Kraftwerk "); }
  Ogre::UTFString DamagedPPMessageB()
  { return Ogre::UTFString(L" wurde wegen eines technischen Fehlers heruntergefahren."); }
  Ogre::UTFString DamagedDetail()
  { return Ogre::UTFString(L"Am das Gebäude zu reparieren wählen Sie es and und klicken Sie auf die Reparatur-Schaltfläche."); }
  Ogre::UTFString ResearchFinishedA()
  { return Ogre::UTFString(L"Erforschung von '"); }
  Ogre::UTFString ResearchFinishedB()
  { return Ogre::UTFString(L"' abgeschlossen!"); }
  Ogre::UTFString ResearchStartedA()
  { return Ogre::UTFString(L"Erforschung von '"); }
  Ogre::UTFString ResearchStartedB()
  { return Ogre::UTFString(L"' begonnen."); }
  Ogre::UTFString BuyF()
  { return Ogre::UTFString(L"Ankauf von "); }
  Ogre::UTFString SellF()
  { return Ogre::UTFString(L"Verkauf von "); }
  Ogre::UTFString From()
  { return Ogre::UTFString(L" von "); }
  Ogre::UTFString To()
  { return Ogre::UTFString(L" an "); }
  Ogre::UTFString For()
  { return Ogre::UTFString(L" für "); }
  Ogre::UTFString EveryWeek()
  { return Ogre::UTFString(L"unbefristet"); }
  Ogre::UTFString PriceT()
  { return Ogre::UTFString(L"Preis"); }
  Ogre::UTFString WorldMarket()
  { return Ogre::UTFString(L"Weltmarkt"); }
  Ogre::UTFString Weeks()
  { return Ogre::UTFString(L"Wochen"); }
  Ogre::UTFString EveryWeekS()
  { return Ogre::UTFString(L"Unbefristet"); }
  Ogre::UTFString WeeksRemaining()
  { return Ogre::UTFString(L"Wochen übrig"); }
  Ogre::UTFString ContractCanceledResources()
  { return Ogre::UTFString(L"Ein Handelsvertrag wurde aufgelöst (Zu wenige Rohstoffe)."); }
  Ogre::UTFString ContractCanceledMoney()
  { return Ogre::UTFString(L"Ein Handelsvertrag wurde aufgelöst (Kein Geld)."); }
  Ogre::UTFString ContractCanceledResourcesDetail()
  { return Ogre::UTFString(L"Ein Handelsvertrag wurde aufgelöst weil Sie nicht genügend Rohstoffe zur Verfügung hatten."); }
  Ogre::UTFString ContractCanceledMoneyDetail()
  { return Ogre::UTFString(L"Ein Handelsvertrag wurde aufgelöst weil Sie nicht genügend Geld zur Verfügung hatten."); }
  Ogre::UTFString GasStockDepleted()
  { return Ogre::UTFString(L"Ihr Gasvorrat ist aufgebraucht!"); }
  Ogre::UTFString CoalStockDepleted()
  { return Ogre::UTFString(L"Ihr Kohlevorrat ist aufgebraucht!"); }
  Ogre::UTFString UraniumStockDepleted()
  { return Ogre::UTFString(L"Ihr Uranvorrat ist aufgebraucht!"); }
  Ogre::UTFString HeadquartersBuilt()
  { return Ogre::UTFString(L"Firmenhauptsitz gebaut. Sie habe jetzt Zugriff auf alle Spezialaktionen!"); }
  Ogre::UTFString HeadquartersBuiltDetail()
  { return Ogre::UTFString(L"Keine"); }
  Ogre::UTFString ResearchBuilt()
  { return Ogre::UTFString(L"Forschungszentrum gebaut. Sie haben jetzt Zugriff auf alle Forschungsprojekte!"); }
  Ogre::UTFString ResearchBuiltDetail()
  { return Ogre::UTFString(L"Keine"); }
  Ogre::UTFString PRBuilt()
  { return Ogre::UTFString(L"PR-Zentrum gebaut. Sie haben jetzt Zugriff auf alle Werbeformen!"); }
  Ogre::UTFString PRBuiltDetail()
  { return Ogre::UTFString(L"Keine"); }
  Ogre::UTFString NewYear()
  { return Ogre::UTFString(L"Ein neues Jahr!"); }
  Ogre::UTFString NoBuild()
  { return Ogre::UTFString(L"Sie können dort nicht bauen!"); }
  Ogre::UTFString NoBuildGas()
  { return Ogre::UTFString(L"Dieses Gebäude muss auf einem Gasvorkommen errichtet werden."); }
  Ogre::UTFString NoBuildUranium()
  { return Ogre::UTFString(L"Dieses Gebäude muss auf einem Uranvorkommen errichtet werden."); }
  Ogre::UTFString NoBuildCoal()
  { return Ogre::UTFString(L"Dieses Gebäude muss auf einem Kohlevorkommen errichtet werden."); }
  Ogre::UTFString NoBuildOffshore()
  { return Ogre::UTFString(L"Offshore-Windkraftanlagen müssen im Meer errichtet werden."); }
  Ogre::UTFString ScreenSaved()
  { return Ogre::UTFString(L" gespeichert."); }
  Ogre::UTFString GameSaved()
  { return Ogre::UTFString(L"Spielstand gespeichert."); }
  Ogre::UTFString PowerplantText()
  { return Ogre::UTFString(L"Kraftwerk"); }
  Ogre::UTFString FullCapacity()
  { return Ogre::UTFString(L" läuft mit maximaler Leistung."); }
  Ogre::UTFString NewspaperImage()
  { return Ogre::UTFString(L"newspaper_de.png"); }
  Ogre::UTFString NPFounded()
  { return Ogre::UTFString(L"Neuer Strom-\nversorger\ngegründet!"); }
  Ogre::UTFString NPFoundedDetail()
  { return Ogre::UTFString(L"Ein neues Energieunternehmen wurde gegründet. Wird es sich auf dem Markt behaupten können?"); }
  Ogre::UTFString NPFusion()
  { return Ogre::UTFString(L"Nukleare\nFusion\ngemeistert!"); }
  Ogre::UTFString NPFusionDetail()
  { return Ogre::UTFString(L"ist weltweit der erste Stromversorger der erfolgreich einen Fusionsreaktor betreibt. Ein Wendepunkt in der Menschheitsgeschichte?"); }
  Ogre::UTFString NPBribe()
  { return Ogre::UTFString(L"Schmiergeld-\nskandal\naufgedeckt!"); }
  Ogre::UTFString NPBribeDetail()
  { return Ogre::UTFString(L" ist verwickelt in ..."); }
  Ogre::UTFString NPPriceRigging()
  { return Ogre::UTFString(L"Preisab-\nsprachen\naufgedeckt!"); }
  Ogre::UTFString NPPriceRiggingDetail()
  { return Ogre::UTFString(L"hat illegalerweise ..."); }
  Ogre::UTFString NPWasteDumping()
  { return Ogre::UTFString(L"Illegale\nMüll-\nbeseitigung!"); }
  Ogre::UTFString NPWasteDumpingDetail()
  { return Ogre::UTFString(L"beseitigte seinen Müll ..."); }
  Ogre::UTFString NPNoTraining()
  { return Ogre::UTFString(L"Schlechte\nMitarbeiter-\nausbildung!"); }
  Ogre::UTFString NPNoTrainingDetail()
  { return Ogre::UTFString(L"hat seine Mitarbeiter nur ..."); }
  Ogre::UTFString NPWar()
  { return Ogre::UTFString(L"Kriegs-\nunterstützung\naufgedeckt!"); }
  Ogre::UTFString NPWarDetail()
  { return Ogre::UTFString(L"unterstützte die Regierung ..."); }
  Ogre::UTFString MissionDoneCaption()
  { return Ogre::UTFString(L"Mission erfolgreich!"); }
  Ogre::UTFString MissionDoneText()
  { return Ogre::UTFString(L"Glückwunsch!\n\nSie haben diese Mission erfolgreich beendet.\nSie können weiterspielen oder dieses Spiel\nbeenden."); }
  Ogre::UTFString KeepPlaying()
  { return Ogre::UTFString(L"Weiterspielen"); }
  Ogre::UTFString ToMenu()
  { return Ogre::UTFString(L"Beenden"); }
  Ogre::UTFString BankruptcyImminentCaption()
  { return Ogre::UTFString(L"Konkurs steht kurz bevor!"); }
  Ogre::UTFString BankruptcyImminentText()
  { return Ogre::UTFString(L"Sehr geehrter Vorstandsvorsitzender,\n\nIhre Schulden übersteigen einen Betrag\nvon 2.000 kEUR. Falls Sie es nicht schaffen\nbis zum Ende des Jahres Ihre Schulden\nauf max. 1.000 kEUR zu reduzieren\nwerden wir Ihre Firma schließen müssen.\n\nMit freundlichen Grüßen\nOmniBank"); }
  Ogre::UTFString BankruptcyCaption()
  { return Ogre::UTFString(L"Konkurs!"); }
  Ogre::UTFString BankruptcyText()
  { return Ogre::UTFString(L"Sehr geehrter Vorstandsvorsitzender,\n\nDa Sie Ihre Schulden auf unsere Warnung\nhin nicht reduzieren konnten, sehen wir\nuns gezwungen Ihre Firma zu schließen.\n\nMit freundlichen Grüßen\nOmniBank"); }
  Ogre::UTFString Small()
  { return Ogre::UTFString(L"klein"); }
  Ogre::UTFString Large()
  { return Ogre::UTFString(L"groß"); }
  Ogre::UTFString ConnectThisCity()
  { return Ogre::UTFString(L"#CC0000Verbinden Sie diese Stadt\num Kunden zu gewinnen"); }
  Ogre::UTFString CreditsText()
  { return Ogre::UTFString(L"Programmierung\nMichael Schießl\n\nGrafik\nPeter Schießl\n\nMusik\nKevin McLeod"); }
  Ogre::UTFString NoSavegameSelected()
  { return Ogre::UTFString(L"Wählen Sie einen Spielstand"); }
  Ogre::UTFString PowerNetCaption()
  { return Ogre::UTFString(L"Stromnetze"); }
  Ogre::UTFString PoleCollapsed()
  { return Ogre::UTFString(L"Ein Strommast ist aufgrund von Wartungsmängeln eingestürzt. Klicken Sie hier um herauszufinden wo."); }
  Ogre::UTFString Building()
  { return Ogre::UTFString(L"Bauen..."); }
  Ogre::UTFString TooManyLoans()
  { return Ogre::UTFString(L"Sie haben zu viele laufende Kredite. Die Bank verweigert Ihnen einen weiteren Kredit."); }
  Ogre::UTFString NotEnoughNetWorth()
  { return Ogre::UTFString(L"Sie haben zuwenig Eigenkapital um diesen Kredit zu erhalten. "); }
  Ogre::UTFString NetWorthNeeded()
  { return Ogre::UTFString(L"Benötigt: "); }
  Ogre::UTFString NetWorthHave()
  { return Ogre::UTFString(L", Aktuell: "); }
  Ogre::UTFString ForecastForThisMonth()
  { return Ogre::UTFString(L"Wettervorhersage:"); }
  Ogre::UTFString StrongWind()
  { return Ogre::UTFString(L"Starke Winde"); }
  Ogre::UTFString NormalWind()
  { return Ogre::UTFString(L"Normale Winde"); }
  Ogre::UTFString LittleWind()
  { return Ogre::UTFString(L"Kaum Wind"); }
  Ogre::UTFString PlentySunshine()
  { return Ogre::UTFString(L"Viel Sonnenschein"); }
  Ogre::UTFString NormalSunshine()
  { return Ogre::UTFString(L"Normaler Sonnenschein"); }
  Ogre::UTFString HardlySunshine()
  { return Ogre::UTFString(L"Kaum Sonnenschein"); }
  Ogre::UTFString Tutorial()
  { return Ogre::UTFString(L"Tutorial"); }
  Ogre::UTFString Step()
  { return Ogre::UTFString(L"Schritt"); }
  Ogre::UTFString TutorialBack()
  { return Ogre::UTFString(L"Zurück"); }
  Ogre::UTFString TutorialContinue()
  { return Ogre::UTFString(L"Weiter"); }
  Ogre::UTFString TutorialExit()
  { return Ogre::UTFString(L"Tutorial beenden"); }
  Ogre::UTFString TutorialReallyClose()
  { return Ogre::UTFString(L"Wollen Sie das Tutorial wirklich beenden?"); }
  Ogre::UTFString NextTutorial()
  { return Ogre::UTFString(L"Nächstes Tutorial"); }
  Ogre::UTFString OpinionIsOK()
  { return Ogre::UTFString(L" ist OK."); }
  Ogre::UTFString OpinionDontLike1()
  { return Ogre::UTFString(L"Ich mag "); }
  Ogre::UTFString OpinionDontLike2()
  { return Ogre::UTFString(L" nicht."); }
  Ogre::UTFString OpinionIsGreat()
  { return Ogre::UTFString(L" ist ein tolles Unternehmen."); }
  Ogre::UTFString OpinionSawAd1()
  { return Ogre::UTFString(L"Ich habe gerade Werbung von "); }
  Ogre::UTFString OpinionSawAd2()
  { return Ogre::UTFString(L" gesehen"); }
  Ogre::UTFString OpinionPriceHigh1()
  { return Ogre::UTFString(L"Der Preis von "); }
  Ogre::UTFString OpinionPriceHigh2()
  { return Ogre::UTFString(L" ist mir zu hoch."); }
  Ogre::UTFString OpinionPriceOK1()
  { return Ogre::UTFString(L"Der Preis von "); }
  Ogre::UTFString OpinionPriceOK2()
  { return Ogre::UTFString(L" ist OK"); }
  Ogre::UTFString DefaultCompanyName()
  { return Ogre::UTFString(L"Standard Elektrik GmbH"); }
  Ogre::UTFString QuestionCompanyName()
  { return Ogre::UTFString(L"Wie wollen Sie ihre Firma nennen?"); }
  Ogre::UTFString PleaseChooseCompanyName()
  { return Ogre::UTFString(L"Bitte wählen Sie einen Firmennamen aus."); }
  Ogre::UTFString EndTutorial()
  { return Ogre::UTFString(L"Tutorial beenden"); }
  Ogre::UTFString NextTutorialQuestion()
  { return Ogre::UTFString(L"Dieses Tutorial beenden und mit dem nächsten fortfahren?"); }
  Ogre::UTFString Repairs1()
  { return Ogre::UTFString(L"Die Reparaturen werden "); }
  Ogre::UTFString Repairs2()
  { return Ogre::UTFString(L" Monat(e) dauern."); }
  Ogre::UTFString IsBeingRepaired()
  { return Ogre::UTFString(L"Wird repariert..."); }
  Ogre::UTFString RepairsComplete1()
  { return Ogre::UTFString(L"Die Reparaturarbeiten bei "); }
  Ogre::UTFString RepairsComplete2()
  { return Ogre::UTFString(L" wurden abgeschlossen."); }
  Ogre::UTFString Germany()
  { return Ogre::UTFString(L"Deutschland"); }
  Ogre::UTFString USA()
  { return Ogre::UTFString(L"USA"); }
  Ogre::UTFString GameAutoSaved()
  { return Ogre::UTFString(L"Spiel wurde automatisch gespeichert."); }
  Ogre::UTFString Accident()
  { return Ogre::UTFString(L"Unfall im\nKraftwerk!"); }
  Ogre::UTFString AccidentDetail1()
  { return Ogre::UTFString(L"Das Kraftwerk "); }
  Ogre::UTFString AccidentDetail2()
  { return Ogre::UTFString(L" wurde heute aufgrund eines Unfalls heruntergefahren."); }
  Ogre::UTFString PPShutdown1()
  { return Ogre::UTFString(L"Ihr Kraftwerk "); }
  Ogre::UTFString PPShutdown2()
  { return Ogre::UTFString(L" wurde aufgrund eines Unfalls heruntergefahren."); }
  Ogre::UTFString CoalStockDepletedDetail()
  { return Ogre::UTFString(L"Ihr Kohlevorrat ist komplett aufgebraucht. Beschaffen Sie sich Kohle indem Sie handeln oder selbst eine Kohlemine betreieben."); }
  Ogre::UTFString UraniumStockDepletedDetail()
  { return Ogre::UTFString(L"Ihr Uranvorrat ist komplett aufgebraucht. Beschaffen Sie sich Uran indem Sie handeln oder selbst eine Uranmine betreieben."); }
  Ogre::UTFString GasStockDepletedDetail()
  { return Ogre::UTFString(L"Ihr Gasvorrat ist komplett aufgebraucht. Beschaffen Sie sich Gas indem Sie handeln oder selbst eine Gasförderanlage betreieben."); }
  Ogre::UTFString FreeTradesDetail()
  { return Ogre::UTFString(L"Durch Steuerhinterziehung im \"CEO\"-Fenster erkaufen Sie sich 6 steuerfreie Handelsaktionen."); }
  Ogre::UTFString PPAccidentDetail()
  { return Ogre::UTFString(L"Falls Sie zukünftige Unfälle vermeiden wollen, verbessern Sie ihre Sicherheitsmaßnahmen."); }
  Ogre::UTFString PoleCollapsedDetail()
  { return Ogre::UTFString(L"Falls Sie vermeiden wollen, dass es auch in Zukunft zu Einstürzen kommt, verbessern Sie Ihre Wartung der Versorgungseinrichtungen."); }
  Ogre::UTFString FullCapacityDetail()
  { return Ogre::UTFString(L"Sie sollten weitere Kraftwerke bauen um die Stromnachfrage zu befriedigen."); }
  Ogre::UTFString DemoTime()
  { return Ogre::UTFString(L"Demozeit übrig:"); }
  Ogre::UTFString DemoNoSave()
  { return Ogre::UTFString(L"In der Demo können Sie Ihr Spiel nicht speichern."); }
  Ogre::UTFString DemoTimeUpCaption()
  { return Ogre::UTFString(L"Demozeit abgelaufen"); }
  Ogre::UTFString DemoTimeUpText()
  { return Ogre::UTFString(L"Die Demozeit ist leider abgelaufen. Beenden Sie das Spiel oder\nbesuchen Sie www.energietycoon.de um das Spiel zu kaufen."); }
  std::vector< std::vector< Ogre::UTFString > > TutorialStep()
  {
    std::vector<std::vector< Ogre::UTFString > > lReturn;
    std::vector< Ogre::UTFString > lSteps1;
    std::vector< Ogre::UTFString > lSteps2;
    std::vector< Ogre::UTFString > lSteps3;

    lSteps1.push_back(L"Willkommen zum Energie-Tycoon Tutorial.\nHier werden Sie lernen wie Sie ein erfolgreiches Energieunternehmen führen.\n\nKlicken Sie auf \"Weiter\" um im Tutorial fortzufahren.");
    lSteps1.push_back(L"Zuerst sollten Sie sich mit der Kamera vertraut machen.\nBewegen Sie die Maus an den Spielfeldrand um den Bildauschnitt zu verschieben.\nAlternativ können Sie auch die rechte Maustaste gedrückt halten oder die Pfeiltasten ihrer Tastatur verwenden.\nUm den Bildauschnitt zu verkleinern oder zu vergrößern benutzen Sie Ihr Mausrad, die Tasten 'W' und 'S' oder die Schaltflächen '+' und '-' unten links.\nUm die Ansicht zu rotieren halten Sie das Mausrad gedrückt, benutzen Sie die Tasten 'A' und 'D', oder verwenden Sie die Schaltflächen '<-' und '->' unten links.\n\nMachen Sie sich mit der Steuerung vertraut und klicken anschließend auf 'Weiter' um im Tutorial fortzufahren.");
    lSteps1.push_back(L"Der erste Schritt beim Aufbau eines Energieunternehmens besteht darin ein Kraftwerk zu errichten. Klicken Sie unten links auf die Schaltfläche mit dem Bild eines Kraftwerks um eine Auswahl verfügbarer Kraftwerke zu erhalten. Wählen Sie die Schaltfläche 'Kleines Windkraftwerk' um ein Kraftwerk zu errichten. Sie können jetzt den Bauort ihres Kraftwerks bestimmen.\nSolange die Umrisse des Kraftwerks grün dargestellt werden ist alles in Ordnung, sollte es jedoch in rot erscheinen können Sie an diesem Ort nicht bauen.\nSuchen Sie sich jetzt eine Stadt und bauen Sie in der Umgebung ihr Windkraftwerk.\n\nKlicken Sie anschließend auf 'Weiter' um im Tutorial fortzufahren.");
    lSteps1.push_back(L"Sie haben jetzt ein Kraftwerk errichtet. Allerdings können Sie damit noch kein Geld verdienen, da es noch mit keiner Stadt verbunden ist.\nUm eine Stromleitung zu errichten wählen Sie die Schaltfläche mit dem Bild eines Strommasten.\nWählen Sie jetzt die Schaltfläche 'Kleine Stromleitung'\nSie können jetzt eine Stromleitung bauen. Wenn Sie den Mauszeiger über ein Objekt bewegen, dass Sie an die Stromleitung anschließen können andert der Mauszeiger sein Aussehen.\nVerbinden Sie jetzt Ihr Kraftwerk und eine Stadt. Mit einem Rechtsklick können Sie den Baumodus wieder verlassen.\nWenn Sie fertig sind klicken Sie auf 'Weiter' um im Tutorial fortzufahren.");
    lSteps1.push_back(L"Sie haben jetzt eine Stadt an ein Kraftwerk angeschlossen, d.h. Kunden können jetzt Ihren Strom kaufen. Klicken Sie auf die Stadt um zu sehen wieviele Kunden Sie haben und wieviel Leistung diese beanspruchen.\n\nOb und wieviele Kunden Sie dazugewinnen oder verlieren hängt von vielen Faktoren ab, die Sie im nächsten Tutorial kennenlernen werden.\nKlicken Sie auf 'Nächstes Tutorial' um damit fortzufahren.");

    lSteps2.push_back(L"Willkommen zum 2. Energie-Tycoon Tutorial.\nHier werden Sie lernen wie Sie mit Ihren Kunden umgehen, ausserdem werden Sie Ihre Forschungs- und Finanzabteilungen kennenlernen.\n\nKlicken Sie auf \"Weiter\" um im Tutorial fortzufahren.");
    lSteps2.push_back(L"Wie Sie sicher schon bemerkt haben, befinden Sich bereits einige Kraftwerke in Ihrem Besitz. Eine der Städte die Sie mit Strom versorgen ist München.\nKlicken Sie die Stadt an um detailierte Informationen über die Stadt zu erhalten.\nSie sehen jetzt wieviele Einwohner die Stadt hat, wieviele Kunden Sie in dieser Stadt haben, wieviel Leistung diese benötigen und wieviel Sie zur Verfügung stellen. Ausserdem können Sie den Strompreis und das Werbebudget für diese Stadt einstellen.\nIhre Kundenzahl ändert sich ständig.\n\nKlicken Sie auf \"Weiter\" um zu lernen wie Sie Kunden gewinnen können.");
    lSteps2.push_back(L"Die wichtigsten Faktoren, die Ihre Kundenentwicklung beeinflussen sind der Strompreis, der Werbeetat und ihr Image. Preis und Werbeetat können Sie im Finanzfenster oder für jede Stadt individuell einstellen indem Sie die Stadt anwählen. Ihr Image dagegen hängt von mehreren Faktoren ab. Unter anderem wie umweltfreundliche ihre Kraftwerke sind, von ihren Werbeaktionen und von einigen Forschungsprojekten. Verändern Sie den Werbeetat oder starten Sie eine Werbeaktion im \"Büro\"-Fenster und beobachten Sie die Kundenentwicklung.\n\nKlicken Sie auf \"Weiter\" um mit dem Tutorial fortzufahren.");
    lSteps2.push_back(L"Es ist sehr wichtig, dass Sie ständig Ihre Finanzen im Auge behalten und falls notwendig eine drohende Insolvenz rechtzeitig verhindern.\nÖffnen Sie das Fenster \"Finanzen\", die Schaltfläche finden Sie links unten auf ihrem Bildschirm oder alternativ auch neben Ihrem aktuellen Kontostand links oben.\nAuf der ersten Seite legen Sie Preise, Wartungseinstellungen und vieles mehr fest. Jede dieser Einstellungen hat Auswirkungen auf Ihr Image, Ihre Finanzen oder die Kundenentwicklung.\n\nKlicken Sie auf \"Weiter\" um im Tutorial fortzufahren.");
    lSteps2.push_back(L"Auf der zweiten Seite des Finanzfensters sehen Sie eine tabellarische Übersicht ihrer Finanzen.\nEs werden sowohl der laufenden Monat und das laufenden Jahr als auch der vollständige vergangene Monat und das Jahr angezeigt.\nAuf der dritten Seite des Finanzfensters finden Sie eine graphische Darstellung Ihrer Finanzen. Sie können einzelne Graphen aus- oder einblenden indem Sie die Kästchen über den Graphen markieren.\nSollten Sie einmal knapp bei Kasse sein und dringen Geld brauchen können Sie auf der vierten Seite des Finanzfensters Kredite aufnehmen.\n\nKlicken Sie auf \"Weiter\" um im Tutorial fortzufahren.");
    lSteps2.push_back(L"Eine der wichtigsten Abteilungen ist Ihre Forschungsabteilung.\nOhne neue Gebäude, Kraftwerke oder Technologien zu erforschen werden Sie keine großen Erfolge erzielen könne.\nÖffnen Sie das Forschungsfenster indem Sie auf die Schaltfläche \"Forschung\" links unten auf Ihrem Bildschirm klicken. Sie sehen jetzt einen Forschungsbaum mit allen Projekten die Sie erforschen können. Um alle Projekte freizuschalten müssen Sie ein Forschungszentrum bauen, ausserdem können Sie nur ein Forschungsprojekt zur selben Zeit betreiben, also überlegen Sie gut was Sie erforschen wollen.\n\nKlicken Sie auf \"Weiter\" um im Tutorial fortzufahren.");
    lSteps2.push_back(L"Hiermit haben Sie das zweite Energie-Tycoon Tutorial beendet.\n\nIm dritten und letzten Tutorial lernen Sie wie Sie mit Resourcen umgehen.\nSie können mit dem nächsten Tutorial fortfahren oder dieses Spiel fortsetzen indem Sie einfach dieses Fenster schließen.");

    lSteps3.push_back(L"Willkommen zum 3. Energie-Tycoon Tutorial.\nHier werden Sie lernen wie Sie mit Resourcen umgehen.\n\nKlicken Sie auf \"Weiter\" um im Tutorial fortzufahren.");
    lSteps3.push_back(L"Sie haben zwei Möglichkeiten sich Resourcen zu beschaffen:\nSie erwerben Ihre Rohstoffe auf dem internationalen Rohstoffmarkt, oder Sie bauen Ihre eigenen Förderstätten um selbst Rohstoffe zu fördern.\nBeide Möglichkeiten haben Vor- und Nachteile und unter Umständen stehen Ihnen auf Ihrer Spielkarte nicht alle Rohstoffe zur Verfügung um sie selbst zu fördern.\n\nKlicken Sie auf \"Weiter\" um zu erfahren wie Sie mit Rohstoffen handeln.");
    lSteps3.push_back(L"Öffnen Sie zuerst das Fenster \"Handel\". Sie finden die Schaltfläche links unten auf ihrem Bildschirm.\nOben links sehen Sie Handelspartner mit denen Sie noch keinen Handelsvertrag abgeschlossen haben, Ihnen jedoch möglicherweise sehr gute Angebote bieten könnten.\nRechts oben sehen Sie die Entwicklung der Weltmarktpreise der 3 verfügbaren Rohstoffe. Handelsverträge werden entweder zu einem Fixpreis abgeschlossen oder zu Weltmarktpreisen, in diesem Fall ist diese Anzeige von großer Bedeutung.\n\nKlicken Sie auf \"Weiter\" um zu lernen wie Sie Handelsverträge abschließen und verwalten.");
    lSteps3.push_back(L"Im unteren Teil des Handelsfensters sehen Sie die zurzeit verfügbaren Angebote und Ihre laufenden Verpflichtungen.\nDie verfügbaren Angebote ändern sich jeden Monat.\nSie haben die Möglichkeit die Angebote nach Rohstoffen und/oder nach Verkaufs- und Kaufangeboten zu filtern indem Sie die Kästchen rechts markieren.\nWählen Sie jetzt einen beliebigen Vertrag aus und klicken Sie auf \"Angebot annehmen\". Einfache Handelsaktionen werden sofort abgewickelt und verschwinden aus der Liste. Aktionen die über einen längeren Zeitraum laufen werden unterhalb angezeigt.\n\nKlicken Sie auf \"Weiter\" um zu erfahren wie Sie eigene Rohstoffe fördern.");
    lSteps3.push_back(L"Um selbst Rohstoffe zu fördern sollten Sie zuerst neben der Übersichtskarte rechts unten die Schaltfläche \"Zeige Rohstoffvorkommen\" betätigen. Sie sehen jetzt welche Rohstoffe sich auf der Spielkarte befinden.\n\nGrün:\tUran\nRot:\tGas\nBlau:\tKohle\n\nUm zu erfahren wie Sie eigene Förderstätten errichten klicken Sie auf \"Weiter\".");
    lSteps3.push_back(L"Klicken Sie jetzt auf die Schaltfläche \"Anlagen zur Resourcenförderung bauen\" links unten auf Ihrem Bildschirm. Sie sehen jetzt eine Auswahl von Gebäuden die Sie errichten können. Unter Umständen sind noch nicht alle Gebäude erforscht.\n Wählen Sie die \"Kleine Uranmine\" und suchen Sie mithilfe der Übersichtskarte ein Uranvorkommen.\nRohstoffvorkommen werden durch kleine Symbole angezeigt auf denen Sie Förderstätten bauen können. Bauen Sie jetzt Ihre Uranmine auf einem Uranvorkommen.\n\nKlicken Sie auf \"Weiter\" um fortzufahren.");
    lSteps3.push_back(L"Hiermit haben Sie das Energie-Tycoon Tutorial beendet.\nSie können dieses Fenster schließen und weiterspielen oder das Spiel beenden und ein Freies Spiel oder eine Mission starten.\n\nViel Spaß!");

    lReturn.push_back(lSteps1);
    lReturn.push_back(lSteps2);
    lReturn.push_back(lSteps3);

    return lReturn;
  }
};

/*-----------------------------------------------------------------------------------------------*/

#endif // LANGDE_H
