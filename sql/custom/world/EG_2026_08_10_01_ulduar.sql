--
DELETE FROM `reference_loot_template` WHERE `Entry` = 34111 AND `Item` = 45538;

--
UPDATE `creature_loot_template` SET `Reference` = 34112 WHERE `Entry` = 33528 AND `Item` = 1;

DELETE FROM `creature_loot_template` WHERE `Entry` = 33733 AND `Item` = 1;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(33733, 1, 34156, 100, 0, 1, 0, 1, 1, NULL);
