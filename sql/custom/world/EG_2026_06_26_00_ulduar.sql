--
-- Iron Council: Archivum Data Disc must also drop on hard mode (Steelbreaker last), not only medium (Molgeim last)
DELETE FROM `creature_loot_template` WHERE `Entry` = 32867 AND `Item` = 45506;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(32867, 45506, 0, 100, 0, 1, 0, 1, 1, 'Archivum Data Disc - hard mode (Steelbreaker last)');

DELETE FROM `creature_loot_template` WHERE `Entry` = 33693 AND `Item` = 45857;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(33693, 45857, 0, 100, 0, 1, 0, 1, 1, 'Archivum Data Disc - hard mode (Steelbreaker last)');

UPDATE `creature_template` SET `mechanic_immune_mask` = 537082687 WHERE `entry` = 33715;
UPDATE `creature_template` SET `mechanic_immune_mask` = 537082687 WHERE `entry` = 33756;
