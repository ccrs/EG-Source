--
UPDATE `creature_loot_template` SET `LootMode` = 31 WHERE `Entry` = 34003 AND `Item` = 45038;

DELETE FROM `reference_loot_template` WHERE `Entry` = 34352 AND `Item` IN (45086, 45110);
DELETE FROM `creature_loot_template` WHERE `Entry` = 34003 AND `Item` IN (45086, 45110);
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(34003, 45086, 0, 14, 0, 31, 2, 1, 1, 'Flame Leviathan (25) - Rising Sun'),
(34003, 45110, 0, 14, 0, 31, 2, 1, 1, 'Flame Leviathan (25) - Titanguard');

UPDATE `creature_loot_template` SET `MinCount` = 2, `MaxCount` = 2 WHERE `Entry` = 33113 AND `Item` = 3;
UPDATE `creature_loot_template` SET `MinCount` = 3, `MaxCount` = 3 WHERE `Entry` = 33113 AND `Item` = 5;
UPDATE `creature_loot_template` SET `MinCount` = 4, `MaxCount` = 4 WHERE `Entry` = 33113 AND `Item` = 7;
UPDATE `creature_loot_template` SET `MinCount` = 5, `MaxCount` = 5 WHERE `Entry` = 33113 AND `Item` = 9;

UPDATE `creature_loot_template` SET `MinCount` = 2, `MaxCount` = 2 WHERE `Entry` = 34003 AND `Item` = 1;
UPDATE `creature_loot_template` SET `MinCount` = 3, `MaxCount` = 3 WHERE `Entry` = 34003 AND `Item` = 3;
UPDATE `creature_loot_template` SET `MinCount` = 4, `MaxCount` = 4 WHERE `Entry` = 34003 AND `Item` = 5;
UPDATE `creature_loot_template` SET `MinCount` = 5, `MaxCount` = 5 WHERE `Entry` = 34003 AND `Item` = 7;
UPDATE `creature_loot_template` SET `MinCount` = 6, `MaxCount` = 6 WHERE `Entry` = 34003 AND `Item` = 9;
