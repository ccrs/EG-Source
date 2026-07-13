--
DELETE FROM `creature_loot_template` WHERE `Entry` = 33113 AND `Item` IN (4, 6, 8, 10);
UPDATE `creature_loot_template` SET `MinCount` = 1, `MaxCount` = 2 WHERE `Entry` = 33113 AND `Item` = 2;
UPDATE `creature_loot_template` SET `MinCount` = 1, `MaxCount` = 1 WHERE `Entry` = 33113 AND `Item` = 45087;

DELETE FROM `creature_loot_template` WHERE `Entry` = 34003 AND `Item` IN (4, 6, 8, 10);
UPDATE `creature_loot_template` SET `MinCount` = 1, `MaxCount` = 4 WHERE `Entry` = 34003 AND `Item` = 2;
UPDATE `creature_loot_template` SET `MinCount` = 1, `MaxCount` = 1 WHERE `Entry` = 34003 AND `Item` = 13;
