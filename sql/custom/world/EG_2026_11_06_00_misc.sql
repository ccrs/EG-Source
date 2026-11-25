--
DELETE FROM `spell_script_names` WHERE `spell_id` = 64677 AND `ScriptName` = 'EG_spell_flame_leviathan_shield_generator';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(64677, 'EG_spell_flame_leviathan_shield_generator');

--
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryorguid` = 33572;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(33572, 0, 0, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Steelforged Defender - On Evade - Despawn Instantly'),
(33572, 0, 1, 0, 0, 0, 100, 0, 0, 2500, 9000, 12000, 11, 62845, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Steelforged Defender - In Combat - Cast Hamstring (62845)'),
(33572, 0, 2, 0, 0, 0, 100, 0, 0, 2600, 13000, 14000, 11, 50370, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Steelforged Defender - In Combat - Cast Sunder Armor (50370)');

UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 34164;
UPDATE `creature_template` SET `ScriptName` = 'EG_npc_ulduar_focused_laser' WHERE `entry` = 34181;

DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryorguid` = 34164;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(34164, 0, 0, 0, 0, 0, 100, 0, 1000, 2000, 2000, 3000, 11, 64692, 0, 0, 0, 0, 0, 5, 200, 0, 0, 0, 0, 0, 0, 'Mechagnome Battletank - In Combat - Cast Flame Cannon (64692) on Random Hostile'),
(34164, 0, 1, 0, 0, 0, 100, 0, 9000, 10000, 10000, 10000, 11, 64687, 0, 0, 0, 0, 0, 5, 200, 0, 0, 0, 0, 0, 0, 'Mechagnome Battletank - In Combat - Cast Focused Laser (64687) on Random Hostile');

UPDATE `creature_template` SET `flags_extra` = `flags_extra` | 128 WHERE `entry` = 34181;

UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 34161;

DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryorguid` = 34161;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(34161, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mechanostriker 54-A - On Just Summoned - Disable Auto Attack'),
(34161, 0, 1, 0, 0, 0, 100, 0, 1000, 1000, 21000, 21000, 11, 64766, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Mechanostriker 54-A - In Combat - Cast Laser Barrage (64766)'),
(34161, 0, 2, 0, 6, 0, 100, 0, 0, 0, 0, 0, 11, 62987, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mechanostriker 54-A - On Just Died - Cast Explosion (62987)'),
(34161, 0, 3, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Mechanostriker 54-A - On Evade - Despawn Instantly');

DELETE FROM `vehicle_template_accessory` WHERE `entry` IN (34161, 34162);
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES
(34161, 33216, 0, 1, 'Mechanostriker 54-A', 6, 30000),
(34162, 33216, 0, 1, 'Mechanostriker 54-A (25)', 6, 30000);

DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (34161, 34162);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`) VALUES
(34161, 0, 0, 1, 0, 0, 0),
(34162, 0, 0, 1, 0, 0, 0);

--
UPDATE `creature_template` SET `unit_flags2` = `unit_flags2` | 65536 WHERE `entry` IN (33113, 34003, 33114, 33139, 34111, 33142, 33143);

-- Flame Leviathan (25)
DELETE FROM `vehicle_template_accessory` WHERE `entry` = 34003 AND `seat_id` IN (0, 1);
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES
(34003, 33114, 0, 1, 'Flame Leviathan (25) - Seat Front L', 6, 30000),
(34003, 33114, 1, 1, 'Flame Leviathan (25) - Seat Front R', 6, 30000);

--
UPDATE `creature_template_addon` SET `visibilityDistanceType` = 5 WHERE `entry` IN (33113, 34003, 33114, 33139, 34111, 33142, 33143);

-- Flame Leviathan Seat / Overload Control Device
UPDATE `creature_template` SET `modelid1` = 11686, `modelid2` = 0, `modelid3` = 0, `modelid4` = 0 WHERE `entry` = 33114;
UPDATE `creature_template` SET `modelid1` = 28469, `modelid2` = 0, `modelid3` = 0, `modelid4` = 0 WHERE `entry` = 33143;

-- Ulduar Expedition Base Camp: invisible collision boxes
DELETE FROM `gameobject` WHERE `guid` IN (300001, 300002, 300003);
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(300001, 201612, 603, 0, 0, 3, 1, -664.489, -122.057, 408, 0.0390664, 0, 0, 0.019532, 0.999809, 604800, 255, 1),
(300002, 201612, 603, 0, 0, 3, 1, -667.401, -47.544, 418, 0.0390664, 0, 0, 0.019532, 0.999809, 604800, 255, 1),
(300003, 201612, 603, 0, 0, 3, 1, -670.314, 26.969, 408, 0.0390664, 0, 0, 0.019532, 0.999809, 604800, 255, 1);

DELETE FROM `smart_scripts` WHERE `entryorguid` = 28239 AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(28239, 0, 0, 0, 11, 0, 100, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Arcane Beam - On Respawn - Disable Auto Attack'),
(28239, 0, 1, 0, 11, 0, 100, 0, 0, 0, 0, 0, 0, 11, 51022, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Arcane Beam - On Respawn - Cast Arcane Beam Spawn Trigger'),
(28239, 0, 2, 0, 11, 0, 100, 0, 0, 0, 0, 0, 0, 123, 10000, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 'Arcane Beam - On Respawn - Add Threat To Summoner'),
(28239, 0, 3, 0, 23, 0, 100, 1, 51024, 1, 1000, 1000, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Arcane Beam - On Aura Arcane Beam Visual - Set Event Phase 1'),
(28239, 0, 4, 0, 23, 1, 100, 0, 51024, 0, 1000, 1000, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Arcane Beam - On Missing Aura Arcane Beam Visual - Despawn Instantly (Phase 1)');
