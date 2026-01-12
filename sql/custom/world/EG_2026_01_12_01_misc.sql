 -- Commander Felstrom smart ai
SET @ENTRY := 771;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 2, 1, 100, 1, 0, 10, 0, 0, 11, 3488, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-10%% (once) - Self: Cast spell  3488 on Self'),
(@ENTRY, 0, 1, 0, 54, 0, 100, 0, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Set event phase to phase 2'),
(@ENTRY, 0, 2, 0, 37, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On AI initialize - Set event phase to phase 1');

 -- Jaedenar Adept smart ai
SET @ENTRY := 7115;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 3400, 4700, 5000, 9000, 11, 20823, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 5 - 9 seconds (3.4 - 4.7s initially) (IC) - Self: Cast spell  20823 with flags combat move on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 11000, 15000, 12000, 16000, 11, 20832, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 12 - 16 seconds (11 - 15s initially) (IC) - Self: Cast spell  20832 on Victim'),
(@ENTRY, 0, 2, 0, 9, 0, 100, 0, 0, 5, 16000, 21000, 11, 14514, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 5 yards (cooldown 16000 - 21000 ms) - Self: Cast spell  14514 on Self'),
(@ENTRY, 0, 3, 0, 2, 0, 100, 1, 0, 15, 0, 0, 25, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-15%% (once) - Self: Flee for assist');

 -- Defias Smuggler smart ai
SET @ENTRY := 95;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 4, 0, 15, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Talk 0 to Attacked unit'),
(@ENTRY, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Set event phase to phase 1'),
(@ENTRY, 0, 2, 0, 0, 1, 100, 0, 0, 0, 3500, 4100, 11, 10277, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 3.5 - 4.1 seconds (0 - 0s initially) (IC) - Self: Cast spell  10277 with flags combat move on Victim'),
(@ENTRY, 0, 3, 0, 0, 1, 100, 0, 1300, 7300, 4800, 4900, 11, 53, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 4.8 - 4.9 seconds (1.3 - 7.3s initially) (IC) - Self: Cast spell  53 on Victim'),
(@ENTRY, 0, 4, 5, 2, 1, 100, 1, 0, 15, 0, 0, 25, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-15%% (once) - Self: Flee for assist'),
(@ENTRY, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-15%% (once) - Set event phase to phase 2'),
(@ENTRY, 0, 6, 0, 34, 0, 100, 0, 13, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On movement of type TIMED_FLEEING_MOTION_TYPE inform - Set event phase to phase 1');
