--
UPDATE `creature_template` SET `lootid` = 32927 WHERE `entry` = 32927;
UPDATE `creature_template` SET `lootid` = 32857 WHERE `entry` = 32857;
UPDATE `creature_template` SET `lootid` = 33692 WHERE `entry` = 33692;
UPDATE `creature_template` SET `lootid` = 33694 WHERE `entry` = 33694;

DELETE FROM `creature_loot_template` WHERE `Entry` = 32927;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(32927, 34359, 34359, 100, 0, 1, 0, 1, 2, 'Gear pool (ref)'),
(32927, 45087, 0,     75,  0, 1, 0, 1, 1, ''),
(32927, 45506, 0,     100, 0, 1, 0, 1, 1, ''),
(32927, 45624, 0,     100, 0, 1, 0, 2, 2, '');

DELETE FROM `creature_loot_template` WHERE `Entry` = 32857;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(32857, 34359, 34359, 100, 0, 1, 0, 1, 2, 'Gear pool (ref)'),
(32857, 45087, 0,     75,  0, 1, 0, 1, 1, ''),
(32857, 45506, 0,     100, 0, 1, 0, 1, 1, ''),
(32857, 45624, 0,     100, 0, 1, 0, 2, 2, '');

DELETE FROM `creature_loot_template` WHERE `Entry` = 33692;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(33692, 34360, 34360, 100, 0, 1, 0, 1, 3, 'Gear pool (ref)'),
(33692, 34154, 34154, 10,  0, 1, 0, 1, 1, 'Bonus ref'),
(33692, 45038, 0,     8,   0, 1, 0, 1, 1, ''),
(33692, 45087, 0,     75,  0, 1, 0, 1, 1, ''),
(33692, 45624, 0,     100, 0, 1, 0, 2, 2, ''),
(33692, 45857, 0,     100, 0, 1, 0, 1, 1, '');

DELETE FROM `creature_loot_template` WHERE `Entry` = 33694;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(33694, 34360, 34360, 100, 0, 1, 0, 1, 3, 'Gear pool (ref)'),
(33694, 34154, 34154, 10,  0, 1, 0, 1, 1, 'Bonus ref'),
(33694, 45038, 0,     8,   0, 1, 0, 1, 1, ''),
(33694, 45087, 0,     75,  0, 1, 0, 1, 1, ''),
(33694, 45624, 0,     100, 0, 1, 0, 2, 2, ''),
(33694, 45857, 0,     100, 0, 1, 0, 1, 1, '');

UPDATE `creature_template` SET `flags_extra` = `flags_extra` | 0x200 WHERE `entry` IN (32857, 33694);

DELETE FROM `spell_script_names` WHERE `ScriptName` = 'EG_spell_brundir_lightning_tendrils_visual';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(61884, 'EG_spell_brundir_lightning_tendrils_visual');
