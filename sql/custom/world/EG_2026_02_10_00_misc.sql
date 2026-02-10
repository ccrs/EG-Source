 -- Grand Necrolord Antiok smart ai
SET @ENTRY := 28006;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2800600);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 37, 0, 100, 0, 0, 0, 0, 0, 116, 10, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On AI initialize - Self: Sets corpse delay to 0.01 s'),
(@ENTRY, 0, 1, 2, 11, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Set event phase to phase 1'),
(@ENTRY, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 159, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Set not attackable'),
(@ENTRY, 0, 3, 0, 1, 0, 100, 0, 90000, 90000, 90000, 90000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 90 seconds (OOC) - Self: Talk 0 to invoker'),
(@ENTRY, 0, 4, 0, 38, 0, 100, 0, 0, 1, 0, 0, 80, 2800600, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[0] set to 1 - Self: Start timed action list id #Grand Necrolord Antiok #0 (2800600) (update always) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set react state to Passive'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 2 seconds - Self: Talk 1 to invoker'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 5 seconds - Self: Talk 2 to invoker'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 11, 50501, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Cast spell  Flesh Harvest (50501) on Self'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 28, 50494, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Remove aura due to spell  Shroud of Lightning (50494)'),
(@ENTRY * 100, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Set event phase to phase 2'),
(@ENTRY * 100, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 159, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set attackable'),
(@ENTRY * 100, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 8, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set react state to Aggressive'),
(@ENTRY, 0, 5, 0, 0, 1, 100, 0, 0, 0, 2000, 2000, 11, 50455, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 2 seconds (0s initially) (IC) - Self: Cast spell  Shadow Bolt (50455) with flags combat move on Victim'),
(@ENTRY, 0, 6, 0, 0, 2, 100, 0, 0, 0, 5000, 5000, 11, 55984, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 5 seconds (0s initially) (IC) - Self: Cast spell  Shadow Bolt (55984) with flags combat move on Victim'),
(@ENTRY, 0, 7, 0, 0, 2, 100, 0, 5000, 15000, 25000, 35000, 11, 50497, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 25 - 35 seconds (5 - 15s initially) (IC) - Self: Cast spell  Scream of Chaos (50497) on Self'),
(@ENTRY, 0, 8, 0, 6, 0, 100, 0, 0, 0, 0, 0, 11, 50472, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Cast spell  Drop Scythe of Antiok (50472) on Self'),
(@ENTRY, 0, 9, 0, 6, 0, 100, 0, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Talk 3 to invoker');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 28006 AND `SourceId` = 0;
