--
UPDATE `creature_template` SET `lootid` = 31610 WHERE `entry` = 31610;
DELETE FROM `creature_loot_template` WHERE `Entry` = 31610;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 1 AND `SourceGroup` = 31610;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES 
(31610, 26040, 26040, 100, 0, 1, 0, 1, 1, ''),
(31610, 37232, 0, 0, 0, 1, 2, 1, 1, ''),
(31610, 37235, 0, 0, 0, 1, 2, 1, 1, ''),
(31610, 37236, 0, 0, 0, 1, 2, 1, 1, ''),
(31610, 37237, 0, 0, 0, 1, 2, 1, 1, ''),
(31610, 37238, 0, 0, 0, 1, 1, 1, 1, ''),
(31610, 37240, 0, 0, 0, 1, 1, 1, 1, ''),
(31610, 37241, 0, 0, 0, 1, 1, 1, 1, ''),
(31610, 37242, 0, 0, 0, 1, 1, 1, 1, ''),
(31610, 39152, 0, 25, 0, 1, 0, 1, 1, ''),
(31610, 40753, 0, 100, 0, 1, 0, 1, 1, ''),
(31610, 41796, 0, 17, 0, 1, 0, 1, 1, ''),
(31610, 43102, 0, 100, 0, 1, 0, 1, 1, ''),
(31610, 43228, 0, 100, 0, 1, 0, 1, 4, ''),
(31610, 43411, 0, 100, 1, 1, 0, 1, 1, ''),
(31610, 43726, 0, 100, 1, 1, 0, 1, 1, ''),
(31610, 43852, 0, 8.5, 0, 1, 0, 1, 1, ''),
(31610, 43876, 0, 8.5, 0, 1, 0, 1, 1, ''),
(31610, 45912, 0, 1, 0, 1, 0, 1, 1, '');
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(1, 31610, 43228, 0, 0, 1, 0, 57940, 0, 0, 0, NULL),
(1, 31610, 43411, 0, 0, 9, 0, 13167, 0, 0, 0, NULL),
(1, 31610, 43726, 0, 0, 9, 0, 13254, 0, 0, 0, NULL);
