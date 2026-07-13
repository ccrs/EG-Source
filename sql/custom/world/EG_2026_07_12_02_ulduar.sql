--
UPDATE `creature_loot_template` SET `MinCount` = 1, `MaxCount` = 1 WHERE `Entry` IN (33113, 34003) AND `Item` IN (1, 3, 5, 7);
DELETE FROM `creature_loot_template` WHERE `Entry` IN (33113, 34003) AND `Item` = 9;
