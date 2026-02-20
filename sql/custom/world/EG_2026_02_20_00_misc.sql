 -- Frostbrood Destroyer smart ai
SET @ENTRY := 30575;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 5000, 10000, 10000, 15000, 11, 57477, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 10 - 15 seconds (5 - 10s initially) (IC) - Self: Cast spell  Freezing Breath (57477) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 10000, 15000, 15000, 25000, 11, 53363, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 15 - 25 seconds (10 - 15s initially) (IC) - Self: Cast spell  Wing Buffet (53363) on Victim'),
(@ENTRY, 0, 2, 0, 8, 0, 100, 0, 57387, 0, 0, 0, 117, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell  Argent Cannon Assault (57387) hit - Self: Disable evade'),
(@ENTRY, 0, 3, 0, 8, 0, 100, 0, 57414, 0, 0, 0, 117, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell  Reckoning Bomb (57414) hit - Self: Disable evade');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30575 AND `SourceId` = 0;
