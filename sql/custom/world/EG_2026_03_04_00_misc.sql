 -- Clientside area trigger 1104 smart ai
SET @ENTRY := 1104;
DELETE FROM `areatrigger_scripts` WHERE `entry` = @ENTRY;
INSERT INTO `areatrigger_scripts` (`entry`, `ScriptName`) VALUES (@ENTRY, 'SmartTrigger');
DELETE FROM `smart_scripts` WHERE `source_type` = 2 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 2, 0, 0, 46, 0, 100, 0, 0, 0, 0, 0, 62, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, -14461.9, 458.295, 15.1639, 3.48, 'On trigger - Triggering player: Teleport to (-14461.9, 458.295, 15.1639, 3.48) on map Eastern Kingdoms (0)');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 1104 AND `SourceId` = 2;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 1, 1104, 2, 0, 6, 0, 67, 0, 0, 0, 'Action invoker belongs to Horde'),
(22, 1, 1104, 2, 0, 2, 0, 9173, 1, 0, 0, 'Action invoker has 1 of item Goblin Transponder (9173) in backpack');

 -- Clientside area trigger 1103 smart ai
SET @ENTRY := 1103;
DELETE FROM `areatrigger_scripts` WHERE `entry` = @ENTRY;
INSERT INTO `areatrigger_scripts` (`entry`, `ScriptName`) VALUES (@ENTRY, 'SmartTrigger');
DELETE FROM `smart_scripts` WHERE `source_type` = 2 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 2, 0, 0, 46, 0, 100, 0, 0, 0, 0, 0, 62, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, -5100.92, 754.567, 260.55, 5.49, 'On trigger - Triggering player: Teleport to (-5100.92, 754.567, 260.55, 5.49) on map Eastern Kingdoms (0)');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 1103 AND `SourceId` = 2;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 1, 1103, 2, 0, 6, 0, 67, 0, 0, 0, 'Action invoker belongs to Horde'),
(22, 1, 1103, 2, 0, 2, 0, 9173, 1, 0, 0, 'Action invoker has 1 of item Goblin Transponder (9173) in backpack');
