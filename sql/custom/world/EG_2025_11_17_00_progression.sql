-- Instances and Raids
DELETE FROM `disables` WHERE (`sourceType` = 2) AND (`entry` IN (249,603,631,632,649,650,658,668,724));
DELETE FROM `disables` WHERE (`sourceType` = 8) AND (`entry` IN (249,603,631,632,649,650,658,668,724));
INSERT INTO `disables` (`sourceType`, `entry`, `flags`, `params_0`, `params_1`, `comment`) VALUES
(2, 249, 3, '', '', "Onyxia\'s Lair"),
(2, 603, 3, '', '', "Ulduar"),
(2, 631, 15, '', '', 'Icecrown Citadel'),
(2, 632, 3, '', '', 'The Forge of Souls'),
(2, 649, 15, '', '', 'Trial of the Crusader'),
(2, 650, 3, '', '', 'Trial of the Champion'),
(2, 658, 3, '', '', 'Pit of Saron'),
(2, 668, 3, '', '', 'Halls of Reflection'),
(2, 724, 15, '', '', 'Ruby Sanctum'),
(8, 249, 3, '', '', "Onyxia\'s Lair"),
(8, 603, 3, '', '', "Ulduar"),
(8, 631, 15, '', '', 'Icecrown Citadel'),
(8, 632, 3, '', '', 'The Forge of Souls'),
(8, 649, 15, '', '', 'Trial of the Crusader'),
(8, 650, 3, '', '', 'Trial of the Champion'),
(8, 658, 3, '', '', 'Pit of Saron'),
(8, 668, 3, '', '', 'Halls of Reflection'),
(8, 724, 15, '', '', 'Ruby Sanctum');

DELETE FROM `disables` WHERE (`sourceType` = 1) AND (`entry` IN (24589,24590));
INSERT INTO `disables` (`sourceType`, `entry`, `flags`, `params_0`, `params_1`, `comment`) VALUES
(1, 24589, 0, '', '', 'Lord Jaraxxus Must Die!'),
(1, 24590, 0, '', '', 'Lord Marrowgar Must Die!');

-- 24579	Sartharion Must Die!
-- 24580	Anub'Rekhan Must Die!
-- 24581	Noth the Plaguebringer Must Die!
-- 24582	Instructor Razuvious Must Die!
-- 24583	Patchwerk Must Die!
-- 24584	Malygos Must Die!
-- 24585	Flame Leviathan Must Die!
-- 24586	Razorscale Must Die!
-- 24587	Ignis the Furnace Master Must Die!
-- 24588	XT-002 Deconstructor Must Die!
UPDATE `quest_template` SET `RewardItem1` = 45624, `RewardItem2` = 40753 WHERE `ID` IN (24579,24580,24581,24582,24583,24584,24585,24586,24587,24588);

-- Daily
UPDATE `quest_template` SET `RewardItem1` = 45624 WHERE (`ID` = 24788);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 24789);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 24790);

-- Proof of Demise:
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 14199);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13256);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13255);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13254);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13253);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13252);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13251);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13250);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13249);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13248);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13247);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13246);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13245);

DELETE FROM `creature_queststarter` WHERE `id` = 20735;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES
(20735, 13245),
(20735, 13246),
(20735, 13247),
(20735, 13248),
(20735, 13249),
(20735, 13250),
(20735, 13251),
(20735, 13252),
(20735, 13253),
(20735, 13254),
(20735, 13255),
(20735, 13256),
(20735, 24579),
(20735, 24580),
(20735, 24581),
(20735, 24582),
(20735, 24583),
(20735, 24584),
(20735, 24585),
(20735, 24586),
(20735, 24587),
(20735, 24588),
(20735, 24589),
(20735, 24590);

DELETE FROM `creature_questender` WHERE `id` = 20735;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES
(20735, 13245),
(20735, 13246),
(20735, 13247),
(20735, 13248),
(20735, 13249),
(20735, 13250),
(20735, 13251),
(20735, 13252),
(20735, 13253),
(20735, 13254),
(20735, 13255),
(20735, 13256),
(20735, 24579),
(20735, 24580),
(20735, 24581),
(20735, 24582),
(20735, 24583),
(20735, 24584),
(20735, 24585),
(20735, 24586),
(20735, 24587),
(20735, 24588),
(20735, 24589),
(20735, 24590);

-- Timear Foresees
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13244);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13243);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13241);
UPDATE `quest_template` SET `RewardItem1` = 40753 WHERE (`ID` = 13240);

-- Normal encounters
UPDATE `creature_loot_template` SET `item` = 40753 WHERE `item` = 47241 AND `entry` IN (
15956, -- Anub'Rekhan
31125, -- Archavon the Stone Watcher
29932, -- Eck the Ferocious
33993, -- Emalon the Storm Watcher
15932, -- Gluth
16060, -- Gothik the Harvester
15953, -- Grand Widow Faerlina
15931, -- Grobbulus
15936, -- Heigan the Unclean
16061, -- Instructor Razuvious
15990, -- Kel'Thuzad
16011, -- Loatheb
15952, -- Maexxna
15954, -- Noth the Plaguebringer
16028, -- Patchwerk
15989, -- Sapphiron
28860, -- Sartharion
30451, -- Shadron
30452, -- Tenebron
15928, -- Thaddius
30449); -- Vesperon

-- Heroic encounters
UPDATE `creature_loot_template` SET `item` = 40753 WHERE `item` = 47241 AND `entry` IN (
31463, -- Amanitar (1)
30529, -- Anomalus (1)
31610, -- Anub'arak (1)
29249, -- Anub'Rekhan (1)
31722, -- Archavon the Stone Watcher (1)
31215, -- Chrono-Lord Epoch (1)
30397, -- Commander Kolurg (1)
30398, -- Commander Stoutbeard (1)
31506, -- Cyanigosa (1)
31367, -- Drakkari Elemental (1)
31558, -- Drakos the Interrogator (1)
31456, -- Elder Nadox (1)
33994, -- Emalon the Storm Watcher (1)
31507, -- Erekem (1)
31368, -- Gal'darah (1)
31533, -- General Bjarngrim (1)
29417, -- Gluth (1)
30774, -- Gortok Palehoof (1)
29955, -- Gothik the Harvester (1)
30510, -- Grand Magus Telestra (1)
29268, -- Grand Widow Faerlina (1)
29373, -- Grobbulus (1)
31611, -- Hadronox (1)
29701, -- Heigan the Unclean (1)
31464, -- Herald Volazj (1)
31508, -- Ichoron (1)
37627, -- Ick (1)
32313, -- Infinite Corruptor (1)
31674, -- Ingvar the Plunderer (1)
29940, -- Instructor Razuvious (1)
31537, -- Ionar (1)
31465, -- Jedoga Shadowseeker (1)
30061, -- Kel'Thuzad (1)
30540, -- Keristrasza (1)
31349, -- King Dred (1)
30788, -- King Ymiron (1)
35360, -- Koralon the Flame Watcher (1)
31612, -- Krik'thir the Gatewatcher (1)
31381, -- Krystallus (1)
31509, -- Lavanthor (1)
29718, -- Loatheb (1)
31538, -- Loken (1)
29278, -- Maexxna (1)
31560, -- Mage-Lord Urom (1)
31384, -- Maiden of Grief (1)
31211, -- Meathook (1)
30530, -- Moorabi (1)
31510, -- Moragg (1)
29615, -- Noth the Plaguebringer (1)
31350, -- Novos the Summoner (1)
30532, -- Ormorok the Tree-Shaper (1)
29324, -- Patchwerk (1)
30748, -- Prince Keleseth (1)
31469, -- Prince Taldaram (1)
31212, -- Salramm the Fleshcrafter (1)
29991, -- Sapphiron (1)
31311, -- Sartharion (1)
31386, -- Sjonnir The Ironshaper (1)
30807, -- Skadi the Ruthless (1)
31679, -- Skarvald the Constructor (1)
31370, -- Slad'ran (1)
30810, -- Svala Sorrowgrave (1)
29448, -- Thaddius (1)
31360, -- The Prophet Tharon'ja (1)
31362, -- Trollgore (1)
31559, -- Varos Cloudstrider (1)
31535, -- Vesperon (1)
31536, -- Volkhan (1)
31511, -- Xevozz (1)
31512); -- Zuramat the Obliterator (1)
UPDATE `item_loot_template` SET `item` = 40753 WHERE `item` = 47241 AND `entry` IN (
43346, -- Large Satchel of Spoils
43347); -- Satchel of Spoils
UPDATE `gameobject_loot_template` SET `item` = 40753 WHERE `item` = 47241 AND `entry` IN (
26094, -- Alexstrasza's Gift
26097, -- Alexstrasza's Gift
24524, -- Cache of Eregos
25192, -- Four Horsemen Chest
25193, -- Four Horsemen Chest
24589, -- Dark Runed Chest
26260); -- Tribunal Chest
UPDATE `item_loot_template` SET `item` = 45624 WHERE `item` = 49426 AND `entry` IN (
54535, -- Keg-Shaped Treasure Chest
54536); -- Satchel of Chilled Goods

-- Ulduar encounters
UPDATE `creature_loot_template` SET `item` = 45624 WHERE `item` = 47241 AND `entry` IN (
33515, -- Auriaya
34175, -- Auriaya (1)
33271, -- General Vezax
33449, -- General Vezax (1)
33118, -- Ignis the Furnace Master
33190, -- Ignis the Furnace Master (1)
33186, -- Razorscale
33724, -- Razorscale (1)
32927, -- Runemaster Molgeim
33692, -- Runemaster Molgeim (1)
32867, -- Steelbreaker
33693, -- Steelbreaker (1)
32857, -- Stormcaller Brundir
33694, -- Stormcaller Brundir (1)
33293, -- XT-002 Deconstructor
33885, -- XT-002 Deconstructor (1)
33288, -- Yogg-Saron
33955); -- Yogg-Saron (1)
UPDATE `item_loot_template` SET `item` = 45624 WHERE `item` = 47241 AND `entry` IN (
45875, -- Algalon
45878); -- Algalon HC
UPDATE `gameobject_loot_template` SET `item` = 45624 WHERE `item` = 47241 AND `entry` IN (
27085, -- Cache of Innovation
26963, -- Cache of Innovation
27086, -- Cache of Innovation
26967, -- Cache of Innovation
27061, -- Cache of Living Stone
26929, -- Cache of Living Stone
27073, -- Cache of Storms
27074, -- Cache of Storms
26955, -- Cache of Storms
26956, -- Cache of Storms
27068, -- Cache of Winter
26946, -- Cache of Winter
27069, -- Rare Cache of Winter
26950, -- Rare Cache of Winter
27078, -- Freya's Gift
27079, -- Freya's Gift
27080, -- Freya's Gift
27081, -- Freya's Gift
26959, -- Freya's Gift
26960, -- Freya's Gift
26961, -- Freya's Gift
26962, -- Freya's Gift
27030, -- Gift of the Observer
26974); -- Gift of the Observer
UPDATE `reference_loot_template` SET `Item` = 45624 WHERE `Item` = 47241 AND `entry` IN (
34349
-- 31311,Sartharion(1)
-- 31311,Sartharion(1)
-- 31311,Sartharion(1)
-- 31313,Flame Leviathan
-- 31313,Flame Leviathan
-- 31313,Flame Leviathan
-- 31313,Flame Leviathan
-- 31313,Flame Leviathan
-- 33288,Yogg-Saron
-- 33288,Yogg-Saron
-- 33955,Yogg-Saron(1)
-- 33955,Yogg-Saron(1)
-- 34003,Flame Leviathan(1)
-- 34003,Flame Leviathan(1)
-- 34003,Flame Leviathan(1)
-- 34003,Flame Leviathan(1)
-- 34003,Flame Leviathan(1)
);

-- Arenas
DELETE FROM `disables` WHERE (`sourceType` = 3) AND (`entry` IN (10,11));
INSERT INTO `disables` (`sourceType`, `entry`, `flags`, `params_0`, `params_1`, `comment`) VALUES
(3, 10, 0, '', '', 'Dalaran Sewers'),
(3, 11, 0, '', '', 'The Ring of Valor');

-- Arena vendors
DELETE FROM `creature` WHERE `guid` IN (125693, 125691, 125689, 133917, 133919, 133920);
DELETE FROM `creature_addon` WHERE `guid` IN (125693, 125691, 125689, 133917, 133919, 133920);
