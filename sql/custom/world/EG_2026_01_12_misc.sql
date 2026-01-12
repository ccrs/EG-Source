-- Stuck Jedoga
UPDATE `creature_template_movement` SET `Ground` = '1' WHERE (`CreatureId` = '29310');

 -- Mo'arg Extractor with guid 97839 smart ai
SET @ENTRY := -97839;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 22304;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 34, 0, 100, 0, 2, 19, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 19 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 1, 0, 34, 0, 100, 0, 2, 19, 0, 0, 11, 40454, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 19 - Self: Cast spell  40454 on Self'),
(@ENTRY, 0, 2, 0, 34, 0, 100, 0, 2, 20, 0, 0, 80, 2230400, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 20 - Self: Start timed action list id #Mo\'arg Extractor #0 (2230400) (update always)'),
(@ENTRY, 0, 3, 0, 34, 0, 100, 0, 2, 23, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 23 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 4, 0, 34, 0, 100, 0, 2, 23, 0, 0, 11, 40454, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 23 - Self: Cast spell  40454 on Self'),
(@ENTRY, 0, 5, 0, 34, 0, 100, 0, 2, 24, 0, 0, 80, 2230400, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 24 - Self: Start timed action list id #Mo\'arg Extractor #0 (2230400) (update always)'),
(@ENTRY, 0, 6, 7, 34, 0, 100, 0, 2, 47, 0, 0, 11, 40163, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 47 - Self: Cast spell  40163 on Self'),
(@ENTRY, 0, 7, 8, 61, 0, 100, 0, 0, 0, 0, 0, 85, 40163, 0, 0, 0, 0, 0, 11, 23174, 20, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 47 - Creature Crystalfused Miner (23174) in 20 yd: Cast spell 40163 on self'),
(@ENTRY, 0, 8, 9, 61, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 11, 23174, 20, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 47 - Creature Crystalfused Miner (23174) in 20 yd: Despawn instantly'),
(@ENTRY, 0, 9, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 47 - Self: Despawn in 1 s'),
(@ENTRY, 0, 10, 0, 0, 0, 100, 0, 2000, 4000, 6000, 8000, 11, 40818, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 6 - 8 seconds (2 - 4s initially) (IC) - Self: Cast spell  40818 on Victim'),
(@ENTRY, 0, 11, 0, 0, 0, 100, 0, 1000, 7000, 5000, 9000, 11, 40839, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 5 - 9 seconds (1 - 7s initially) (IC) - Self: Cast spell  40839 on Victim');

 -- Mo'arg Extractor with guid 97842 smart ai
SET @ENTRY := -97842;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 22304;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 34, 0, 100, 0, 2, 15, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 15 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 1, 0, 34, 0, 100, 0, 2, 15, 0, 0, 11, 40454, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 15 - Self: Cast spell  40454 on Self'),
(@ENTRY, 0, 2, 0, 34, 0, 100, 0, 2, 16, 0, 0, 80, 2230400, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 16 - Self: Start timed action list id #Mo\'arg Extractor #0 (2230400) (update always)'),
(@ENTRY, 0, 3, 0, 34, 0, 100, 0, 2, 28, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 28 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 4, 0, 34, 0, 100, 0, 2, 28, 0, 0, 11, 40454, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 28 - Self: Cast spell  40454 on Self'),
(@ENTRY, 0, 5, 0, 34, 0, 100, 0, 2, 29, 0, 0, 80, 2230400, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 29 - Self: Start timed action list id #Mo\'arg Extractor #0 (2230400) (update always)'),
(@ENTRY, 0, 6, 7, 34, 0, 100, 0, 2, 50, 0, 0, 11, 40163, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 50 - Self: Cast spell  40163 on Self'),
(@ENTRY, 0, 7, 8, 61, 0, 100, 0, 0, 0, 0, 0, 85, 40163, 0, 0, 0, 0, 0, 11, 23174, 20, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 50 - Creature Crystalfused Miner (23174) in 20 yd: Cast spell 40163 on self'),
(@ENTRY, 0, 8, 9, 61, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 11, 23174, 20, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 50 - Creature Crystalfused Miner (23174) in 20 yd: Despawn instantly'),
(@ENTRY, 0, 9, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 50 - Self: Despawn in 1 s'),
(@ENTRY, 0, 10, 0, 0, 0, 100, 0, 2000, 4000, 6000, 8000, 11, 40818, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 6 - 8 seconds (2 - 4s initially) (IC) - Self: Cast spell  40818 on Victim'),
(@ENTRY, 0, 11, 0, 0, 0, 100, 0, 1000, 7000, 5000, 9000, 11, 40839, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 5 - 9 seconds (1 - 7s initially) (IC) - Self: Cast spell  40839 on Victim');

 -- Mo'arg Extractor with guid 97836 smart ai
SET @ENTRY := -97836;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 22304;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 34, 0, 100, 0, 2, 28, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 28 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 1, 0, 34, 0, 100, 0, 2, 28, 0, 0, 11, 40454, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 28 - Self: Cast spell  40454 on Self'),
(@ENTRY, 0, 2, 0, 34, 0, 100, 0, 2, 29, 0, 0, 80, 2230400, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 29 - Self: Start timed action list id #Mo\'arg Extractor #0 (2230400) (update always)'),
(@ENTRY, 0, 3, 0, 34, 0, 100, 0, 2, 34, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 34 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 4, 0, 34, 0, 100, 0, 2, 34, 0, 0, 11, 40454, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 34 - Self: Cast spell  40454 on Self'),
(@ENTRY, 0, 5, 0, 34, 0, 100, 0, 2, 35, 0, 0, 80, 2230400, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 35 - Self: Start timed action list id #Mo\'arg Extractor #0 (2230400) (update always)'),
(@ENTRY, 0, 6, 7, 34, 0, 100, 0, 2, 67, 0, 0, 11, 40163, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 67 - Self: Cast spell  40163 on Self'),
(@ENTRY, 0, 7, 8, 61, 0, 100, 0, 0, 0, 0, 0, 85, 40163, 0, 0, 0, 0, 0, 11, 23174, 20, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 67 - Creature Crystalfused Miner (23174) in 20 yd: Cast spell 40163 on self'),
(@ENTRY, 0, 8, 9, 61, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 11, 23174, 20, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 67 - Creature Crystalfused Miner (23174) in 20 yd: Despawn instantly'),
(@ENTRY, 0, 9, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 67 - Self: Despawn in 1 s'),
(@ENTRY, 0, 10, 0, 0, 0, 100, 0, 2000, 4000, 6000, 8000, 11, 40818, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 6 - 8 seconds (2 - 4s initially) (IC) - Self: Cast spell  40818 on Victim'),
(@ENTRY, 0, 11, 0, 0, 0, 100, 0, 1000, 7000, 5000, 9000, 11, 40839, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 5 - 9 seconds (1 - 7s initially) (IC) - Self: Cast spell  40839 on Victim');

 -- Mo'arg Extractor with guid 97833 smart ai
SET @ENTRY := -97833;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 22304;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 34, 0, 100, 0, 2, 16, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 16 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 1, 0, 34, 0, 100, 0, 2, 16, 0, 0, 11, 40454, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 16 - Self: Cast spell  40454 on Self'),
(@ENTRY, 0, 2, 0, 34, 0, 100, 0, 2, 17, 0, 0, 80, 2230400, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 17 - Self: Start timed action list id #Mo\'arg Extractor #0 (2230400) (update always)'),
(@ENTRY, 0, 3, 0, 34, 0, 100, 0, 2, 24, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 24 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 4, 0, 34, 0, 100, 0, 2, 24, 0, 0, 11, 40454, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 24 - Self: Cast spell  40454 on Self'),
(@ENTRY, 0, 5, 0, 34, 0, 100, 0, 2, 25, 0, 0, 80, 2230400, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 25 - Self: Start timed action list id #Mo\'arg Extractor #0 (2230400) (update always)'),
(@ENTRY, 0, 6, 7, 34, 0, 100, 0, 2, 51, 0, 0, 11, 40163, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 51 - Self: Cast spell  40163 on Self'),
(@ENTRY, 0, 7, 8, 61, 0, 100, 0, 0, 0, 0, 0, 85, 40163, 0, 0, 0, 0, 0, 11, 23174, 20, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 51 - Creature Crystalfused Miner (23174) in 20 yd: Cast spell 40163 on self'),
(@ENTRY, 0, 8, 9, 61, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 11, 23174, 20, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 51 - Creature Crystalfused Miner (23174) in 20 yd: Despawn instantly'),
(@ENTRY, 0, 9, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type WAYPOINT_MOTION_TYPE inform, point 51 - Self: Despawn in 1 s'),
(@ENTRY, 0, 10, 0, 0, 0, 100, 0, 2000, 4000, 6000, 8000, 11, 40818, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 6 - 8 seconds (2 - 4s initially) (IC) - Self: Cast spell  40818 on Victim'),
(@ENTRY, 0, 11, 0, 0, 0, 100, 0, 1000, 7000, 5000, 9000, 11, 40839, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 5 - 9 seconds (1 - 7s initially) (IC) - Self: Cast spell  40839 on Victim');

 -- Karen "I Don't Caribou" the Culler smart ai
SET @ENTRY := 25803;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 8000, 10000, 10000, 15000, 11, 42724, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 10 - 15 seconds (8 - 10s initially) (IC) - Self: Cast spell  42724 on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 4000, 4000, 15000, 21000, 11, 48280, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 15 - 21 seconds (4 - 4s initially) (IC) - Self: Cast spell  48280 on Self'),
(@ENTRY, 0, 2, 0, 9, 0, 100, 0, 8, 25, 10000, 10000, 11, 27577, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 8 - 25 yards (cooldown 10000 - 10000 ms) - Self: Cast spell  27577 on Victim'),
(@ENTRY, 0, 3, 0, 25, 0, 100, 0, 0, 0, 0, 0, 53, 0, 206425, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Start path #206425, walk, repeat, Passive'),
(@ENTRY, 0, 4, 5, 40, 0, 100, 0, 15, 206425, 0, 0, 54, 26000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 15 of path 206425 reached - Self: Pause path for 26000 ms'),
(@ENTRY, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 46147, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 15 of path 206425 reached - Self: Cast spell  46147 on Self');

 -- Drostan smart ai
SET @ENTRY := 28857;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2885700, 2885701, 2885702, 2885703, 2885704, 2885705, 2885706, 2885707);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 11, 52573, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  52573 on Self'),
(@ENTRY, 0, 1, 0, 54, 0, 100, 0, 0, 0, 0, 0, 86, 52585, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  52585 at Self'),
(@ENTRY, 0, 2, 0, 54, 0, 100, 0, 0, 0, 0, 0, 86, 52725, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  52725 at Self'),
(@ENTRY, 0, 3, 0, 54, 0, 100, 0, 0, 0, 0, 0, 86, 52726, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  52726 at Self'),
(@ENTRY, 0, 4, 0, 54, 0, 100, 0, 0, 0, 0, 0, 86, 52727, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  52727 at Self'),
(@ENTRY, 0, 5, 0, 54, 0, 100, 0, 0, 0, 0, 0, 86, 52728, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  52728 at Self'),
(@ENTRY, 0, 6, 0, 54, 0, 100, 0, 0, 0, 0, 0, 86, 52729, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  52729 at Self'),
(@ENTRY, 0, 7, 0, 54, 0, 100, 0, 0, 0, 0, 0, 86, 52730, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  52730 at Self'),
(@ENTRY, 0, 8, 0, 54, 0, 100, 0, 0, 0, 0, 0, 86, 52731, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  52731 at Self'),
(@ENTRY, 0, 9, 0, 8, 0, 100, 0, 52585, 0, 0, 0, 80, 2885700, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  52585 hit - Self: Start timed action list id #Drostan #0 (2885700) (update out of combat) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Talk 0 to invoker'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 114, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, -20, 0, 0, 'After 3 seconds - Self: Move forward by 0, left by -20, up by 0 yards'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 10, 0, 8, 0, 100, 0, 52725, 0, 0, 0, 80, 2885701, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  52725 hit - Self: Start timed action list id #Drostan #1 (2885701) (update out of combat) // -inline'),
(@ENTRY * 100 + 1, 9, 0, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Talk 1 to invoker'),
(@ENTRY * 100 + 1, 9, 1, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 114, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, -20, 0, 0, 'After 3 seconds - Self: Move forward by 0, left by -20, up by 0 yards'),
(@ENTRY * 100 + 1, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 11, 0, 8, 0, 100, 0, 52726, 0, 0, 0, 80, 2885702, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  52726 hit - Self: Start timed action list id #Drostan #2 (2885702) (update out of combat) // -inline'),
(@ENTRY * 100 + 2, 9, 0, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 2, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Talk 2 to invoker'),
(@ENTRY * 100 + 2, 9, 1, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 114, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, -20, 0, 0, 'After 3 seconds - Self: Move forward by 0, left by -20, up by 0 yards'),
(@ENTRY * 100 + 2, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 12, 0, 8, 0, 100, 0, 52727, 0, 0, 0, 80, 2885703, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  52727 hit - Self: Start timed action list id #Drostan #3 (2885703) (update out of combat) // -inline'),
(@ENTRY * 100 + 3, 9, 0, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 3, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Talk 3 to invoker'),
(@ENTRY * 100 + 3, 9, 1, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 11, 55474, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, -20, 0, 0, 'After 3 seconds - Self: Cast spell  55474 on Self'),
(@ENTRY * 100 + 3, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 90, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set stand state to SLEEP'),
(@ENTRY * 100 + 3, 9, 3, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 13, 0, 8, 0, 100, 0, 52728, 0, 0, 0, 80, 2885704, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  52728 hit - Self: Start timed action list id #Drostan #4 (2885704) (update out of combat) // -inline'),
(@ENTRY * 100 + 4, 9, 0, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 4, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Talk 4 to invoker'),
(@ENTRY * 100 + 4, 9, 1, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 5, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Talk 5 to invoker'),
(@ENTRY * 100 + 4, 9, 2, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 114, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, -20, 0, 0, 'After 3 seconds - Self: Move forward by 0, left by -20, up by 0 yards'),
(@ENTRY * 100 + 4, 9, 3, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 14, 0, 8, 0, 100, 0, 52729, 0, 0, 0, 80, 2885705, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  52729 hit - Self: Start timed action list id #Drostan #5 (2885705) (update out of combat) // -inline'),
(@ENTRY * 100 + 5, 9, 0, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 6, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Talk 6 to invoker'),
(@ENTRY * 100 + 5, 9, 1, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 114, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, -20, 0, 0, 'After 3 seconds - Self: Move forward by 0, left by -20, up by 0 yards'),
(@ENTRY * 100 + 5, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 15, 0, 8, 0, 100, 0, 52730, 0, 0, 0, 80, 2885706, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  52730 hit - Self: Start timed action list id #Drostan #6 (2885706) (update out of combat) // -inline'),
(@ENTRY * 100 + 6, 9, 0, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 7, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Talk 7 to invoker'),
(@ENTRY * 100 + 6, 9, 1, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 114, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, -20, 0, 0, 'After 3 seconds - Self: Move forward by 0, left by -20, up by 0 yards'),
(@ENTRY * 100 + 6, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 16, 0, 8, 0, 100, 0, 52731, 0, 0, 0, 80, 2885707, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  52731 hit - Self: Start timed action list id #Drostan #7 (2885707) (update out of combat) // -inline'),
(@ENTRY * 100 + 7, 9, 0, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 8, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Talk 8 to invoker'),
(@ENTRY * 100 + 7, 9, 1, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 114, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, -20, 0, 0, 'After 3 seconds - Self: Move forward by 0, left by -20, up by 0 yards'),
(@ENTRY * 100 + 7, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Despawn instantly');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 28857 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 2, 28857, 0, 0, 48, 0, 12592, 0, 6, 0, 'Action invoker 0 quest The Great Hunter\'s Challenge (12592) objective == 6'),
(22, 3, 28857, 0, 0, 48, 0, 12592, 0, 11, 0, 'Action invoker 0 quest The Great Hunter\'s Challenge (12592) objective == 11'),
(22, 4, 28857, 0, 0, 48, 0, 12592, 0, 21, 0, 'Action invoker 0 quest The Great Hunter\'s Challenge (12592) objective == 21'),
(22, 5, 28857, 0, 0, 48, 0, 12592, 0, 28, 0, 'Action invoker 0 quest The Great Hunter\'s Challenge (12592) objective == 28'),
(22, 6, 28857, 0, 0, 48, 0, 12592, 0, 35, 0, 'Action invoker 0 quest The Great Hunter\'s Challenge (12592) objective == 35'),
(22, 7, 28857, 0, 0, 48, 0, 12592, 0, 41, 0, 'Action invoker 0 quest The Great Hunter\'s Challenge (12592) objective == 41'),
(22, 8, 28857, 0, 0, 48, 0, 12592, 0, 49, 0, 'Action invoker 0 quest The Great Hunter\'s Challenge (12592) objective == 49'),
(22, 9, 28857, 0, 0, 48, 0, 12592, 0, 56, 0, 'Action invoker 0 quest The Great Hunter\'s Challenge (12592) objective == 56');
