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
