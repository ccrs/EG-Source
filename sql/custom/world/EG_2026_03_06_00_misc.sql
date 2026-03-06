 -- Reanimated Crusader smart ai
SET @ENTRY := 30202;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 8, 0, 100, 1, 57806, 0, 0, 0, 11, 57808, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On spell  Sprinkle Holy Water (57806) hit - Self: Cast spell  Freed Crusader Soul (57808) on Caster'),
(@ENTRY, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Sprinkle Holy Water (57806) hit - Self: Despawn instantly'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 1000, 5000, 6500, 12000, 11, 32674, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 6.5 - 12 seconds (1 - 5s initially) (IC) - Self: Cast spell  Avenger\'s Shield (32674) on Victim'),
(@ENTRY, 0, 3, 0, 0, 0, 100, 0, 5000, 10000, 8500, 20000, 11, 58154, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 8.5 - 20 seconds (5 - 10s initially) (IC) - Self: Cast spell  Hammer of Injustice (58154) on Victim'),
(@ENTRY, 0, 4, 0, 0, 0, 100, 0, 0, 9000, 8000, 20000, 11, 58153, 0, 0, 0, 0, 0, 31, 40, 0, 1, 0, 0, 0, 0, 'Every 8 - 20 seconds (0 - 9s initially) (IC) - Self: Cast spell  Unholy Light (58153) on Lowest HP friendly in 40 yards including self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30202 AND `SourceId` = 0;
