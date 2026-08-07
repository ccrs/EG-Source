--
UPDATE `creature_loot_template` SET `LootMode` = 3 WHERE `Entry` IN (15928, 15952, 15990, 16011) AND `Item` IN (40753, 45912);
UPDATE `gameobject_loot_template` SET `LootMode` = 3 WHERE `Entry` = 25192 AND `Item` = 40753;
UPDATE `reference_loot_template` SET `LootMode` = 3 WHERE `Entry` IN (34136, 34139, 34143, 34146, 34149);
--
DELETE FROM `creature_loot_template` WHERE `Entry` IN (15928, 15952, 15990, 16011) AND `LootMode` = 2;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(15952, 34139, 34139, 100, 0, 2, 0, 1, 4, 'Maexxna - Curse of Idiocy - 25man loot'),
(16011, 40637, 0, 0, 0, 2, 1, 1, 1, 'Loatheb - Curse of Idiocy - 25man loot'),
(16011, 40638, 0, 0, 0, 2, 1, 1, 1, 'Loatheb - Curse of Idiocy - 25man loot'),
(16011, 40639, 0, 0, 0, 2, 1, 1, 1, 'Loatheb - Curse of Idiocy - 25man loot'),
(16011, 34149, 34149, 100, 0, 2, 0, 1, 2, 'Loatheb - Curse of Idiocy - 25man loot'),
(15928, 40634, 0, 0, 0, 2, 3, 1, 1, 'Thaddius - Curse of Idiocy - 25man loot'),
(15928, 40635, 0, 0, 0, 2, 3, 1, 1, 'Thaddius - Curse of Idiocy - 25man loot'),
(15928, 40636, 0, 0, 0, 2, 3, 1, 1, 'Thaddius - Curse of Idiocy - 25man loot'),
(15928, 34143, 34143, 100, 0, 2, 0, 1, 2, 'Thaddius - Curse of Idiocy - 25man loot'),
(15990, 40631, 0, 0, 0, 2, 1, 1, 1, 'Kel''Thuzad - Curse of Idiocy - 25man loot'),
(15990, 40632, 0, 0, 0, 2, 1, 1, 1, 'Kel''Thuzad - Curse of Idiocy - 25man loot'),
(15990, 40633, 0, 0, 0, 2, 1, 1, 1, 'Kel''Thuzad - Curse of Idiocy - 25man loot'),
(15990, 34136, 34136, 100, 0, 2, 0, 1, 3, 'Kel''Thuzad - Curse of Idiocy - 25man loot');
--
DELETE FROM `gameobject_loot_template` WHERE `Entry` = 25192 AND `LootMode` = 2;
INSERT INTO `gameobject_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(25192, 40625, 0, 0, 0, 2, 1, 1, 1, 'Four Horsemen Chest - Curse of Idiocy - 25man loot'),
(25192, 40626, 0, 0, 0, 2, 1, 1, 1, 'Four Horsemen Chest - Curse of Idiocy - 25man loot'),
(25192, 40627, 0, 0, 0, 2, 1, 1, 1, 'Four Horsemen Chest - Curse of Idiocy - 25man loot'),
(25192, 34146, 34146, 100, 0, 2, 0, 1, 2, 'Four Horsemen Chest - Curse of Idiocy - 25man loot');
