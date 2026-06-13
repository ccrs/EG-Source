 -- Death Knight Cavalier smart ai
SET @ENTRY := 16163;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 2, 0, 0, 4600, 7300, 11, 55314, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 'Every 4.6 - 7.3 seconds (0 - 0s initially) (IC) - Self: Cast spell  Strangulate (55314) on Random hostile (not top)'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 4, 0, 0, 4600, 7300, 11, 55334, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 'Every 4.6 - 7.3 seconds (0 - 0s initially) (IC) - Self: Cast spell  Strangulate (55334) on Random hostile (not top)'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 2, 1100, 2200, 4100, 6200, 11, 55313, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 4.1 - 6.2 seconds (1.1 - 2.2s initially) (IC) - Self: Cast spell  Icy Touch (55313) on Random hostile'),
(@ENTRY, 0, 3, 0, 0, 0, 100, 4, 1100, 2200, 4100, 6200, 11, 55331, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 4.1 - 6.2 seconds (1.1 - 2.2s initially) (IC) - Self: Cast spell  Icy Touch (55331) on Random hostile'),
(@ENTRY, 0, 4, 0, 0, 0, 100, 2, 4700, 6100, 15700, 19800, 11, 55315, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 15.7 - 19.8 seconds (4.7 - 6.1s initially) (IC) - Self: Cast spell  Bone Armor (55315) on Self'),
(@ENTRY, 0, 5, 0, 0, 0, 100, 4, 4700, 6100, 15700, 19800, 11, 55336, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 15.7 - 19.8 seconds (4.7 - 6.1s initially) (IC) - Self: Cast spell  Bone Armor (55336) on Self'),
(@ENTRY, 0, 6, 7, 2, 0, 100, 7, 0, 30, 0, 0, 11, 55294, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-30%% (once) - Self: Cast spell  Dismount Deathcharger (55294) with flags interrupt previous on Self'),
(@ENTRY, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 43, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-30%% (once) - Self: Dismount');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 16163 AND `SourceId` = 0;
