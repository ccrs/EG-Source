 -- Steelforged Defender smart ai
SET @ENTRY := 33572;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 33572 AND `SourceId` = 0;

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (62496));

DELETE FROM `spell_script_names` WHERE `ScriptName` IN('EG_spell_flame_leviathan_grab_crate_triggered');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(62482, 'EG_spell_flame_leviathan_grab_crate_triggered');

UPDATE `creature_template` SET `ScriptName` = '' WHERE `entry` = 33109;
UPDATE `creature_template` SET `ScriptName` = 'EG_npc_salvaged_demolisher_mechanic_seat' WHERE `entry` = 33167;

-- Pool of Tar
UPDATE `creature_template` SET `flags_extra` = 1073750080 WHERE `entry` = 33090;
-- Mimiron's inferno
UPDATE `creature_template` SET `faction` = 1965,`flags_extra` = 8320 WHERE `entry` = 33370;
UPDATE `creature_template` SET `faction` = 1965,`flags_extra` = 8384 WHERE `entry` = 33369;

DELETE FROM `vehicle_template_accessory` WHERE `entry` = 33369;
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `summontype`, `summontimer`, `description`) VALUES
(33369, 33370, 0, 1, 5, 0, 'Mimiron\'s Inferno Targetting Reticle seat 0: Mimiron\'s Inferno');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (62909));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 62909, 0, 0, 31, 0, 3, 33369, 0, 0, 'Potential target of the spell is creature, entry is Mimiron\'s Inferno Targetting Reticle (33369)');

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 33369;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(33369, 46598, 1, 0);
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 18) AND (`SourceGroup` IN (33369));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(18, 33369, 46598, 0, 0, 31, 0, 3, 0, 0, 0, 'Clicker is creature');

UPDATE `creature_template` SET `ScriptName` = 'npc_mimirons_inferno' WHERE `entry` = 33369;
UPDATE `creature_template` SET `ScriptName` = '' WHERE `entry` = 33370;

DELETE FROM `spell_script_names` WHERE `ScriptName` = 'EG_spell_flame_leviathan_mimirons_inferno';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(62910, 'EG_spell_flame_leviathan_mimirons_inferno');

-- Thorim's hammer
UPDATE `creature_template` SET `faction` = 1965,`flags_extra` = 8320 WHERE `entry` = 33365;
UPDATE `creature_template` SET `faction` = 1965,`flags_extra` = 8384 WHERE `entry` = 33364;

DELETE FROM `vehicle_template_accessory` WHERE `entry` = 33364;
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `summontype`, `summontimer`, `description`) VALUES
(33364, 33365, 0, 1, 5, 0, 'Thorim\'s Hammer Targetting Reticle seat 0: Thorim\'s Hammer');

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 33364;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(33364, 46598, 1, 0);
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 18) AND (`SourceGroup` IN (33364));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(18, 33364, 46598, 0, 0, 31, 0, 3, 0, 0, 0, 'Clicker is creature');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (62911));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 62911, 0, 0, 31, 0, 3, 33364, 0, 0, 'Potential target of the spell is creature, entry is Thorim\'s Hammer Targetting Reticle (33364)');

-- Hodir's fury
UPDATE `creature_template` SET `faction` = 1965,`flags_extra` = 8320 WHERE `entry` = 33212;
UPDATE `creature_template` SET `faction` = 1965,`flags_extra` = 8384 WHERE `entry` = 33108;

DELETE FROM `vehicle_template_accessory` WHERE `entry` = 33108;
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `summontype`, `summontimer`, `description`) VALUES
(33108, 33212, 0, 1, 5, 0, 'Hodir\'s Fury Targetting Reticle seat 0: Hodir\'s Fury');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (62533));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 62533, 0, 0, 31, 0, 3, 33108, 0, 0, 'Potential target of the spell is creature, entry is Hodir\'s Fury Targetting Reticle (33108)');

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 33108;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(33108, 46598, 1, 0);
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 18) AND (`SourceGroup` IN (33108));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(18, 33108, 46598, 0, 0, 31, 0, 3, 0, 0, 0, 'Clicker is creature');

UPDATE `creature_template` SET `ScriptName` = 'npc_hodirs_fury' WHERE `entry` = 33108;
UPDATE `creature_template` SET `ScriptName` = '' WHERE `entry` = 33212;

-- Freya's ward
UPDATE `creature_template` SET `faction` = 1965,`flags_extra` = 8320 WHERE `entry` = 33367;
UPDATE `creature_template` SET `faction` = 1965,`flags_extra` = 8384 WHERE `entry` = 33366;

DELETE FROM `vehicle_template_accessory` WHERE `entry` = 33366;
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `summontype`, `summontimer`, `description`) VALUES
(33366, 33367, 0, 1, 5, 0, 'Freya\'s Ward Targetting Reticle seat 0: Freya\'s Ward');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (62906));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 62906, 0, 0, 31, 0, 3, 33366, 0, 0, 'Potential target of the spell is creature, entry is Freya\'s Ward Targetting Reticle (33366)');

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 33366;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(33366, 46598, 1, 0);
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 18) AND (`SourceGroup` IN (33366));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(18, 33366, 46598, 0, 0, 31, 0, 3, 0, 0, 0, 'Clicker is creature');

-- Freya's adds
UPDATE `creature_template` SET `mechanic_immune_mask`= 
1 | 2 | 4 | 8 | 16 | 32 | 64 | 256 | 512 | 1024 | 2048 | 4096 | 8192 | 65536 | 131072 | 8388608 | 67108864 | 536870912 WHERE `entry` IN
(33385, 33397, 33396, 33088, 33168, 33228);

--
UPDATE `creature_template` SET `VehicleId` = 399 WHERE `entry` = 34214;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='EG_npc_arachnopod_destroyer' WHERE `entry`=34183;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid` IN (34183);

DELETE FROM `creature_template_spell` WHERE `CreatureID` IN(34183, 34214);
INSERT INTO `creature_template_spell` (`CreatureID`, `Index`, `Spell`, `VerifiedBuild`) VALUES
(34183, 0, 64717, 0),
(34183, 1, 64779, 0),
(34183, 2, 64776, 0),
(34214, 0, 64717, 0),
(34214, 1, 64779, 0),
(34214, 2, 64776, 0);

DELETE FROM `vehicle_template_accessory` WHERE `entry`IN(34183, 34214);
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES 
(34183, 34184, 1, 0, 'Arachnopod - Clockwork Mechanic', 6, 500),
(34214, 34184, 1, 0, 'Arachnopod - Clockwork Mechanic', 6, 500);

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 34183;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(34183, 46598, 0, 0),
(34183, 63313, 1, 1);
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 18) AND (`SourceGroup` IN (34183));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(18, 34183, 63313, 0, 0, 31, 0, 3, 0, 0, 0, 'Clicker is creature'),
(18, 34183, 46598, 0, 0, 31, 0, 4, 0, 0, 0, 'Clicker is player');

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 34214;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(34214, 46598, 0, 0),
(34214, 63313, 1, 1);
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 18) AND (`SourceGroup` IN (34214));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(18, 34214, 63313, 0, 0, 31, 0, 3, 0, 0, 0, 'Clicker is creature'),
(18, 34214, 46598, 0, 0, 31, 0, 4, 0, 0, 0, 'Clicker is player');

--
DELETE FROM `spelldifficulty_dbc` WHERE `id` IN (46763);
INSERT INTO `spelldifficulty_dbc` (`id`, `spellid0`, `spellid1`, `spellid2`, `spellid3`) VALUES 
(46763, 46763, 64062, 0, 0);

-- Enslaved Fire Elemental
UPDATE `creature` SET `spawntimesecs`=604800 WHERE `id`=33838 AND `guid` IN (137557, 137558, 137559);

--
UPDATE `creature_template` SET `ScriptName`='EG_npc_ulduar_tower_gauntlet_generator' WHERE `entry` IN (33571, 34159);

--
UPDATE `creature_template` SET `ScriptName`='EG_npc_storm_tempered_keeper' WHERE `entry` IN (33699, 33722);

UPDATE `creature_template` SET `mechanic_immune_mask`=1 | 2 | 4 | 8 | 16 | 32 | 64 | 256 | 512 | 2048 | 4096 | 8192 | 65536 | 131072 | 67108864 | 536870912, `flags_extra`=2 WHERE `entry` IN (33715, 33756);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=13 AND `SourceEntry`=63528;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES 
(13, 3, 63528, 0, 0, 31, 0, 3, 33699, 0, 0, 0, 0, '', 'Supercharged - Target Storm Tempered Keeper'),
(13, 3, 63528, 0, 1, 31, 0, 3, 33722, 0, 0, 0, 0, '', 'Supercharged - Target Storm Tempered Keeper');

--
DELETE FROM `achievement_criteria_data` WHERE `type`=18 AND `criteria_id` IN (10090, 10091, 10423, 10425, 10422, 10424);
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES 
(10090, 18, 0, 0, ''),
(10091, 18, 0, 0, ''),
(10423, 18, 0, 0, ''),
(10425, 18, 0, 0, ''),
(10422, 18, 0, 0, ''),
(10424, 18, 0, 0, '');

DELETE FROM `disables` WHERE `sourceType`=4 AND `entry` IN (10090, 10091, 10423, 10425, 10422, 10424);

UPDATE `creature_template` SET `flags_extra` = 8384 WHERE `entry` = 33705;

UPDATE `creature_template` SET `unit_flags` = 33554432, `flags_extra` = 8384 WHERE `entry` = 32866;
UPDATE `creature_template` SET `unit_flags` = 33554432, `flags_extra` = 8384 WHERE `entry` = 33690;
