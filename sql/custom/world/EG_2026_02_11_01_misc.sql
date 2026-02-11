 -- Persistence smart ai
SET @ENTRY := 29863;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2986300);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Set event phase to phase 1'),
(@ENTRY, 0, 1, 0, 25, 1, 100, 0, 0, 0, 0, 0, 80, 2986300, 2, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Start timed action list id #Persistence #0 (2986300) (update always) override existing // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 69, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 2, 2, 0, 0, 'After 1 seconds - Self: Move to Owner/Summoner with offset (2, 2, 0, 0)');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 29863 AND `SourceId` = 0;
