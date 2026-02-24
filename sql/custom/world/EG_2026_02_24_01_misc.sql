 -- Reanimated Captain smart ai
SET @ENTRY := 30986;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 32674, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Cast spell  Avenger\'s Shield (32674) on Attacked unit'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 5000, 5000, 17800, 19800, 11, 58154, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 17.8 - 19.8 seconds (5 - 5s initially) (IC) - Self: Cast spell  Hammer of Injustice (58154) on Victim'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 5000, 15000, 17800, 19800, 11, 58153, 0, 0, 0, 0, 0, 31, 40, 0, 1, 0, 0, 0, 0, 'Every 17.8 - 19.8 seconds (5 - 15s initially) (IC) - Self: Cast spell  Unholy Light (58153) on Lowest HP friendly in 40 yards including self');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30986 AND `SourceId` = 0;
