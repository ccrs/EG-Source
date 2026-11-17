--
DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (10619, 10620) AND `type` = 12;
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES
(10619, 12, 0, 0, ''),
(10620, 12, 1, 0, '');

DELETE FROM `reference_loot_template` WHERE `Entry` = 34367 AND `Item` IN (45454, 45464);
INSERT INTO `reference_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(34367, 45464, 0, 0, 0, 1, 1, 1, 1, NULL);

DELETE FROM `creature_loot_template` WHERE `Entry` IN (32857, 32867, 32927) AND `Item` = 45087 AND `Reference` = 0;

DELETE FROM `creature_loot_template` WHERE `Entry` = 33724 AND `Item` = 2 AND `Reference` = 34154;

UPDATE `gameobject_loot_template` SET `MaxCount` = 2 WHERE `Entry` = 26946 AND `Item` = 2 AND `Reference` = 12029;

DELETE FROM `gameobject_loot_template` WHERE `Entry` = 26962 AND `Item` = 3 AND `Reference` = 34154;

DELETE FROM `gameobject_loot_template` WHERE `Entry` = 26950 AND `Item` IN (1, 45038, 45087);
INSERT INTO `gameobject_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(26950, 1, 34154, 10, 0, 1, 0, 1, 1, NULL),
(26950, 45038, 0, 16, 0, 1, 0, 1, 1, NULL),
(26950, 45087, 0, 41, 0, 1, 0, 1, 1, NULL);

DELETE FROM `reference_loot_template` WHERE `Entry` = 34378;
INSERT INTO `reference_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(34378, 45088, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45089, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45090, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45091, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45092, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45093, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45094, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45095, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45096, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45097, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45098, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45099, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45100, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45101, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45102, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45103, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45104, 0, 4, 0, 1, 1, 1, 1, NULL),
(34378, 45105, 0, 4, 0, 1, 1, 1, 1, NULL);

UPDATE `creature_loot_template` SET `Reference` = 34378 WHERE `Entry` = 33113 AND `Item` IN (11, 12) AND `Reference` = 34154;
UPDATE `creature_loot_template` SET `Reference` = 34378 WHERE `Entry` = 33288 AND `Item` IN (5, 7) AND `Reference` = 34154;
UPDATE `gameobject_loot_template` SET `Reference` = 34378 WHERE `Entry` IN (27080, 27081) AND `Item` = 3 AND `Reference` = 34154;

DELETE FROM `reference_loot_template` WHERE `Entry` = 34357 AND `Item` IN (45867, 45868, 45869, 45870, 45871);
DELETE FROM `creature_loot_template` WHERE `Entry` = 33293 AND `Item` IN (1, 2, 45867, 45868, 45869, 45870, 45871);
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(33293, 1, 34357, 100, 0, 3, 0, 1, 1, NULL),
(33293, 2, 34357, 100, 0, 1, 0, 1, 1, NULL),
(33293, 45867, 0, 0, 0, 2, 1, 1, 1, NULL),
(33293, 45868, 0, 0, 0, 2, 1, 1, 1, NULL),
(33293, 45869, 0, 0, 0, 2, 1, 1, 1, NULL),
(33293, 45870, 0, 0, 0, 2, 1, 1, 1, NULL),
(33293, 45871, 0, 0, 0, 2, 1, 1, 1, NULL);
UPDATE `reference_loot_template` SET `LootMode` = 3 WHERE `Entry` IN (34154, 34357, 34358) AND `LootMode` = 1;

UPDATE `gameobject_loot_template` SET `Chance` = 18 WHERE `Entry` IN (26959, 26960, 26961) AND `Item` = 45038 AND `Reference` = 0;

UPDATE `creature_loot_template` SET `Chance` = 18 WHERE `Entry` = 33693 AND `Item` = 45038 AND `Reference` = 0;

DELETE FROM `creature_loot_template` WHERE `Entry` = 33449 AND `Item` IN (45498, 45511);
DELETE FROM `reference_loot_template` WHERE `Entry` = 34374 AND `Item` IN (45498, 45511);
INSERT INTO `reference_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(34374, 45498, 0, 0, 0, 1, 1, 1, 1, NULL),
(34374, 45511, 0, 0, 0, 1, 1, 1, 1, NULL);
