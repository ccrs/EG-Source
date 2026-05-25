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

-- Flame Leviathan Seat (33114): subseat 1 = Defense Turret, subseat 2 = Overload Device
DELETE FROM `vehicle_template_accessory` WHERE `entry` = 33114 AND `seat_id` IN (1, 2);
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES
(33114, 33142, 1, 1, 'Flame Leviathan Seat - Leviathan Defense Turret', 6, 30000),
(33114, 33143, 2, 1, 'Flame Leviathan Seat - Overload Control Device', 6, 30000);

-- FL 25-man (34003) spellclick: Ride Vehicle (46598)
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 34003;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(34003, 46598, 1, 0);

-- FL 25-man (34003) accessories: mirror 33113 layout (2x Seat, 1x Static Cannon)
DELETE FROM `vehicle_template_accessory` WHERE `entry` = 34003;
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES
(34003, 33114, 2, 1, 'Flame Leviathan (25) - Seat L', 6, 30000),
(34003, 33114, 3, 1, 'Flame Leviathan (25) - Seat R', 6, 30000),
(34003, 33139, 7, 1, 'Flame Leviathan (25) - Static Cannon', 6, 30000);

-- Spawn Pyrite (62543)
DELETE FROM `spell_dbc` WHERE `Id` = 62543;
INSERT INTO `spell_dbc` (`Id`, `Attributes`, `CastingTimeIndex`, `DurationIndex`, `RangeIndex`, `Effect1`, `EffectImplicitTargetA1`, `EffectMiscValue1`, `EffectMiscValueB1`, `EffectBasePoints1`, `SpellName`, `SchoolMask`) VALUES
(62543, 256, 1, 3, 1, 28, 32, 33189, 64, 0, 'Spawn Pyrite', 1);

-- Liquid Pyrite (33189)
UPDATE `creature_template_addon` SET `auras` = '62494' WHERE `entry` = 33189;

-- Bronzebeard Radio (34054)
DELETE FROM `creature_text` WHERE `CreatureID` = 34054;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(34054, 0, 0, 12, 0, 100, 0, 0, 0, 34154, 3, 'Bronzebeard Radio SAY_BRANN_RADIO_LEVIATHAN'),
(34054, 1, 0, 12, 0, 100, 0, 0, 0, 34155, 3, 'Bronzebeard Radio SAY_BRANN_RADIO_LEVIATHAN2'),
(34054, 2, 0, 12, 0, 100, 0, 0, 0, 34156, 3, 'Bronzebeard Radio SAY_BRANN_RADIO_LEVIATHAN3');

-- Move 4 RX-214 Repair-o-matic Station
UPDATE `gameobject` SET `position_x` = 155.553 WHERE `guid` = 55106;
UPDATE `gameobject` SET `position_x` = 156.895 WHERE `guid` = 55153;
UPDATE `gameobject` SET `position_x` = 163.516 WHERE `guid` = 55126;
UPDATE `gameobject` SET `position_x` = 164.857 WHERE `guid` = 55193;

-- Brann's Flying Machine (34120)
UPDATE `creature_template` SET `ScriptName` = 'EG_npc_flame_leviathan_outro_flying_machine' WHERE `entry` = 34120;

-- Brann (34119) outro yells
DELETE FROM `creature_text` WHERE `CreatureID` = 34119;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(34119, 0, 0, '', 14, 0, 100, 0, 0, 0, 34225, 0, 'Brann FL outro 1 - What a battle'),
(34119, 1, 0, '', 14, 0, 100, 0, 0, 0, 34226, 0, 'Brann FL outro 3 - Perhaps so'),
(34119, 2, 0, '', 14, 0, 100, 0, 0, 0, 34229, 0, 'Brann FL outro 5 - Oi'),
(34119, 3, 0, '', 14, 0, 100, 0, 0, 0, 34231, 0, 'Brann FL outro 6 - What about plated proto-drake'),
(34119, 4, 0, '', 14, 0, 100, 0, 0, 0, 34233, 0, 'Brann FL outro 9 - Sneak marmots'),
(34119, 5, 0, '', 14, 0, 100, 0, 0, 0, 34235, 0, 'Brann FL outro 11 - Fine');

-- Rhydian (33696) outro yells
DELETE FROM `creature_text` WHERE `CreatureID` = 33696 AND `GroupID` IN (2, 3, 4, 5, 6);
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(33696, 2, 0, 'Our friends fought well, Brann, but we''re not done yet.', 14, 0, 100, 0, 0, 0, 0, 0, 'Rhydian FL outro 2'),
(33696, 3, 0, 'None at all. I suspect it has something to do with that giant mechanical construct that our scouts spotted in front of the gate.', 14, 0, 100, 0, 0, 0, 0, 0, 'Rhydian FL outro 4'),
(33696, 4, 0, 'The Kirin Tor can''t possibly spare any additional resources to take on anything that size. We may not have to though.', 14, 0, 100, 0, 0, 0, 0, 0, 'Rhydian FL outro 7'),
(33696, 5, 0, 'We can sneak past them. As long as we can take down that construct in front of the gate, we should be able to get inside.', 14, 0, 100, 0, 0, 0, 0, 0, 'Rhydian FL outro 8'),
(33696, 6, 0, 'We''re hunting an old god, Brann.', 14, 0, 100, 0, 0, 0, 0, 0, 'Rhydian FL outro 10');
