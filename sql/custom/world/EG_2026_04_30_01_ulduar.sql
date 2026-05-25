--
-- Assembly of Iron 10/25
UPDATE `creature_template` SET `lootid` = 32927 WHERE `entry` = 32927;
UPDATE `creature_template` SET `lootid` = 32857 WHERE `entry` = 32857;
UPDATE `creature_template` SET `lootid` = 33692 WHERE `entry` = 33692;
UPDATE `creature_template` SET `lootid` = 33694 WHERE `entry` = 33694;
UPDATE `creature_template` SET `lootid` = 33693 WHERE `entry` = 33693;

-- 10-man Molgeim (32927) loot
DELETE FROM `creature_loot_template` WHERE `Entry` = 32927;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(32927, 34359, 34359, 100, 0, 1, 0, 1, 2, 'Gear pool (ref)'),
(32927, 45087, 0,     75,  0, 1, 0, 1, 1, ''),
(32927, 45506, 0,     100, 0, 1, 0, 1, 1, ''),
(32927, 45624, 0,     100, 0, 1, 0, 2, 2, '');

-- 10-man Brundir (32857) loot: no Archivum Data Disc
DELETE FROM `creature_loot_template` WHERE `Entry` = 32857;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(32857, 34359, 34359, 100, 0, 1, 0, 1, 2, 'Gear pool (ref)'),
(32857, 45087, 0,     75,  0, 1, 0, 1, 1, ''),
(32857, 45624, 0,     100, 0, 1, 0, 2, 2, '');

-- 25-man Molgeim (33692) loot
DELETE FROM `creature_loot_template` WHERE `Entry` = 33692;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(33692, 34360, 34360, 100, 0, 1, 0, 1, 3, 'Gear pool (ref)'),
(33692, 34154, 34154, 10,  0, 1, 0, 1, 1, 'Bonus ref'),
(33692, 45038, 0,     8,   0, 1, 0, 1, 1, ''),
(33692, 45087, 0,     75,  0, 1, 0, 1, 1, ''),
(33692, 45624, 0,     100, 0, 1, 0, 2, 2, ''),
(33692, 45857, 0,     100, 0, 1, 0, 1, 1, '');

-- 25-man Brundir (33694) loot: no Archivum Data Disc
DELETE FROM `creature_loot_template` WHERE `Entry` = 33694;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(33694, 34360, 34360, 100, 0, 1, 0, 1, 3, 'Gear pool (ref)'),
(33694, 34154, 34154, 10,  0, 1, 0, 1, 1, 'Bonus ref'),
(33694, 45038, 0,     8,   0, 1, 0, 1, 1, ''),
(33694, 45087, 0,     75,  0, 1, 0, 1, 1, ''),
(33694, 45624, 0,     100, 0, 1, 0, 2, 2, '');

-- 10-man Steelbreaker (32867) base: remove Archivum Data Disc
DELETE FROM `creature_loot_template` WHERE `Entry` = 32867 AND `Item` = 45506;

-- 25-man Steelbreaker (33693) loot
DELETE FROM `creature_loot_template` WHERE `Entry` = 33693;
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(33693, 34360, 34360, 100, 0, 1, 0, 1, 3, 'Gear pool (ref)'),
(33693, 34154, 34154, 10,  0, 1, 0, 1, 1, 'Bonus ref'),
(33693, 45038, 0,     8,   0, 1, 0, 1, 1, ''),
(33693, 45087, 0,     75,  0, 1, 0, 1, 1, ''),
(33693, 45624, 0,     100, 0, 1, 0, 2, 2, ''),
(33693, 45241, 0,     0,   0, 1, 1, 1, 1, 'Hardmode unique pool: Belt of Colossal Rage'),
(33693, 45242, 0,     0,   0, 1, 1, 1, 1, 'Hardmode unique pool: Drape of Mortal Downfall'),
(33693, 45243, 0,     0,   0, 1, 1, 1, 1, 'Hardmode unique pool: Sapphire Amulet of Renewal'),
(33693, 45244, 0,     0,   0, 1, 1, 1, 1, 'Hardmode unique pool: Greaves of Swift Vengeance'),
(33693, 45245, 0,     0,   0, 1, 1, 1, 1, 'Hardmode unique pool: Shoulderpads of the Intruder'),
(33693, 45607, 0,     0,   0, 1, 1, 1, 1, 'Hardmode unique pool: Fang of Oblivion');

-- Brundir 10/25 (32857/33694): add CREATURE_FLAG_EXTRA_NO_MOVE_FLAGS_UPDATE
UPDATE `creature_template` SET `flags_extra` = `flags_extra` | 0x200 WHERE `entry` IN (32857, 33694);

-- Lightning Tendrils visual aura script (61884)
DELETE FROM `spell_script_names` WHERE `ScriptName` = 'EG_spell_brundir_lightning_tendrils_visual';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(61884, 'EG_spell_brundir_lightning_tendrils_visual');
