 -- Feero Ironhand smart ai
SET @ENTRY := 4484;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (448400, 448401, 448402, 448403, 448404, 448405, 448406);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 19, 0, 100, 0, 976, 0, 0, 0, 80, 448400, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On player accepted quest Supplies to Auberdine (976) - Self: Start timed action list id #Feero Ironhand #0 (448400) (update out of combat) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 83, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Remove npc flags QUESTGIVER'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 63, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set script counter_1 to 0'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 63, 2, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set script counter_2 to 0'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 63, 3, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set script counter_3 to 0'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: storedTarget[1] = Last action invoker\'s party'),
(@ENTRY * 100, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 2, 774, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set faction to Escortee (774)'),
(@ENTRY * 100, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 145, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Disable Immune to NPC'),
(@ENTRY * 100, 9, 7, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Talk 0 to invoker'),
(@ENTRY * 100, 9, 8, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 53, 1, 35873, 0, 976, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Start path #35873, run, do not repeat, Passive'),
(@ENTRY, 0, 1, 2, 40, 0, 100, 0, 19, 35873, 0, 0, 54, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 19 of path 35873 reached - Self: Pause path for 0 ms'),
(@ENTRY, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 448401, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 19 of path 35873 reached - Self: Start timed action list id #Feero Ironhand #1 (448401) (update out of combat) // -inline'),
(@ENTRY * 100 + 1, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Talk 1 to invoker'),
(@ENTRY * 100 + 1, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 107, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Summon creature group 0'),
(@ENTRY, 0, 3, 0, 77, 0, 100, 0, 1, 4, 0, 0, 80, 448402, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On counter 1 set to 4 - Self: Start timed action list id #Feero Ironhand #2 (448402) (update out of combat) override existing // -inline'),
(@ENTRY * 100 + 2, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.0491384, 'After 0 seconds - Self: Set orientation to 0.0491384'),
(@ENTRY * 100 + 2, 9, 1, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Talk 2 to invoker'),
(@ENTRY * 100 + 2, 9, 2, 0, 0, 0, 100, 0, 11000, 11000, 0, 0, 65, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 11 seconds - Self: Resume path'),
(@ENTRY, 0, 4, 5, 40, 0, 100, 0, 27, 35873, 0, 0, 54, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 27 of path 35873 reached - Self: Pause path for 0 ms'),
(@ENTRY, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 448403, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 27 of path 35873 reached - Self: Start timed action list id #Feero Ironhand #3 (448403) (update always) // -inline'),
(@ENTRY * 100 + 3, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Talk 3 to invoker'),
(@ENTRY * 100 + 3, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 107, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Summon creature group 1'),
(@ENTRY * 100 + 3, 9, 2, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 19, 3893, 50, 0, 0, 0, 0, 0, 'After 5 seconds - Closest alive creature Forsaken Scout (3893) in 50 yards: Talk 0 to invoker'),
(@ENTRY, 0, 6, 0, 77, 0, 100, 0, 2, 3, 0, 0, 80, 448404, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On counter 2 set to 3 - Self: Start timed action list id #Feero Ironhand #4 (448404) (update out of combat) override existing // -inline'),
(@ENTRY * 100 + 4, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5.71721, 'After 0 seconds - Self: Set orientation to 5.71721'),
(@ENTRY * 100 + 4, 9, 1, 0, 0, 0, 100, 0, 200, 200, 0, 0, 5, 15, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0.2 seconds - Self: Play emote ONESHOT_ROAR(DNR) (15)'),
(@ENTRY * 100 + 4, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 1, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Talk 4 to invoker'),
(@ENTRY * 100 + 4, 9, 3, 0, 0, 0, 100, 0, 7000, 7000, 0, 0, 65, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 7 seconds - Self: Resume path'),
(@ENTRY, 0, 7, 8, 40, 0, 100, 0, 43, 35873, 0, 0, 54, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 43 of path 35873 reached - Self: Pause path for 0 ms'),
(@ENTRY, 0, 8, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 448405, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 43 of path 35873 reached - Self: Start timed action list id #Feero Ironhand #5 (448405) (update always) // -inline'),
(@ENTRY * 100 + 5, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Talk 5 to invoker'),
(@ENTRY * 100 + 5, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 107, 2, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Summon creature group 2'),
(@ENTRY * 100 + 5, 9, 2, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 19, 3899, 50, 0, 0, 0, 0, 0, 'After 5 seconds - Closest alive creature Balizar the Umbrage (3899) in 50 yards: Talk 0 to invoker'),
(@ENTRY * 100 + 5, 9, 3, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 6, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 5 seconds - Self: Talk 6 to invoker'),
(@ENTRY, 0, 9, 0, 77, 0, 100, 0, 3, 3, 0, 0, 80, 448406, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On counter 3 set to 3 - Self: Start timed action list id #Feero Ironhand #6 (448406) (update out of combat) override existing // -inline'),
(@ENTRY * 100 + 6, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0.64463, 'After 0 seconds - Self: Set orientation to 0.64463'),
(@ENTRY * 100 + 6, 9, 1, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 1, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Talk 7 to invoker'),
(@ENTRY * 100 + 6, 9, 2, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 15, 976, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 3 seconds - storedTarget[1]: Call quest Supplies to Auberdine (976) group event happened'),
(@ENTRY * 100 + 6, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 65, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Resume path'),
(@ENTRY, 0, 10, 0, 40, 0, 100, 0, 48, 35873, 0, 0, 41, 2000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 48 of path 35873 reached - Self: Despawn in 2 s'),
(@ENTRY, 0, 11, 0, 6, 0, 100, 0, 0, 0, 0, 0, 6, 976, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'On death - storedTarget[1]: Fail quest Supplies to Auberdine (976)'),
(@ENTRY, 0, 12, 0, 35, 0, 100, 0, 3879, 0, 0, 0, 63, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature Dark Strand Assassin (3879) despawn - Self: Add 1 to counter_1'),
(@ENTRY, 0, 13, 0, 35, 0, 100, 0, 3893, 0, 0, 0, 63, 2, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature Forsaken Scout (3893) despawn - Self: Add 1 to counter_2'),
(@ENTRY, 0, 14, 0, 35, 0, 100, 0, 3898, 0, 0, 0, 63, 3, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature Aligar the Tormentor (3898) despawn - Self: Add 1 to counter_3'),
(@ENTRY, 0, 15, 0, 35, 0, 100, 0, 3899, 0, 0, 0, 63, 3, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature Balizar the Umbrage (3899) despawn - Self: Add 1 to counter_3'),
(@ENTRY, 0, 16, 0, 35, 0, 100, 0, 3900, 0, 0, 0, 63, 3, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature Caedakar the Vicious (3900) despawn - Self: Add 1 to counter_3');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 4484 AND `SourceId` = 0;

 -- Dark Strand Assassin smart ai
SET @ENTRY := 3879;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 19, 4484, 100, 0, 0, 0, 0, 0, 'On reset - Self: Attack Closest alive creature Feero Ironhand (4484) in 100 yards');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 3879 AND `SourceId` = 0;

 -- Forsaken Scout smart ai
SET @ENTRY := 3893;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 25, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 19, 4484, 100, 0, 0, 0, 0, 0, 'On reset - Self: Attack Closest alive creature Feero Ironhand (4484) in 100 yards');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 3893 AND `SourceId` = 0;

 -- Aligar the Tormentor smart ai
SET @ENTRY := 3898;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 3000, 4000, 5000, 6000, 11, 9080, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 5 - 6 seconds (3 - 4s initially) (IC) - Self: Cast spell  Hamstring (9080) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 6000, 7000, 6000, 7000, 11, 25712, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 6 - 7 seconds (IC) - Self: Cast spell  Heroic Strike (25712) on Victim'),
(@ENTRY, 0, 2, 3, 25, 0, 100, 0, 0, 0, 0, 0, 11, 7165, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Cast spell  Battle Stance (7165) on Self'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 19, 4484, 100, 0, 0, 0, 0, 0, 'On reset - Self: Attack Closest alive creature Feero Ironhand (4484) in 100 yards');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 3898 AND `SourceId` = 0;

 -- Balizar the Umbrage smart ai
SET @ENTRY := 3899;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 1000, 2000, 10000, 15000, 11, 14868, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 10 - 15 seconds (1 - 2s initially) (IC) - Self: Cast spell  Curse of Agony (14868) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 3000, 4000, 11000, 16000, 11, 11980, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 11 - 16 seconds (3 - 4s initially) (IC) - Self: Cast spell  Curse of Weakness (11980) on Victim'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 0, 0, 3400, 4800, 11, 20791, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 3.4 - 4.8 seconds (0 - 0s initially) (IC) - Self: Cast spell  Shadow Bolt (20791) with flags combat move on Victim'),
(@ENTRY, 0, 3, 0, 25, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 19, 4484, 100, 0, 0, 0, 0, 0, 'On reset - Self: Attack Closest alive creature Feero Ironhand (4484) in 100 yards');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 3899 AND `SourceId` = 0;

 -- Caedakar the Vicious smart ai
SET @ENTRY := 3900;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 0, 0, 3400, 4800, 11, 9532, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 3.4 - 4.8 seconds (0 - 0s initially) (IC) - Self: Cast spell  Lightning Bolt (9532) with flags combat move on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 3000, 7000, 20000, 40000, 11, 913, 0, 0, 0, 0, 0, 31, 40, 0, 1, 0, 0, 0, 0, 'Every 8 - 12 seconds (3 - 7s initially) (IC) - Self: Cast spell  Healing Wave (913) on Lowest HP friendly within 40 yards including self'),
(@ENTRY, 0, 2, 3, 25, 0, 100, 0, 0, 0, 0, 0, 11, 905, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Cast spell  Lightning Shield (905) with flags triggered on Self'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 19, 4484, 100, 0, 0, 0, 0, 0, 'On reset - Self: Attack Closest alive creature Feero Ironhand (4484) in 100 yards');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 3900 AND `SourceId` = 0;

UPDATE `creature_summon_groups` SET `summonTime` = 20000 WHERE (((`summonerId` = 4484) AND (`summonerType` = 0)) AND (`groupId` = 0)) AND (`entry` = 3879);
UPDATE `creature_summon_groups` SET `summonTime` = 20000 WHERE (((`summonerId` = 4484) AND (`summonerType` = 0)) AND (`groupId` = 0)) AND (`entry` = 3879);
UPDATE `creature_summon_groups` SET `summonTime` = 20000 WHERE (((`summonerId` = 4484) AND (`summonerType` = 0)) AND (`groupId` = 0)) AND (`entry` = 3879);
UPDATE `creature_summon_groups` SET `summonTime` = 20000 WHERE (((`summonerId` = 4484) AND (`summonerType` = 0)) AND (`groupId` = 0)) AND (`entry` = 3879);
UPDATE `creature_summon_groups` SET `summonTime` = 20000 WHERE (((`summonerId` = 4484) AND (`summonerType` = 0)) AND (`groupId` = 1)) AND (`entry` = 3893);
UPDATE `creature_summon_groups` SET `summonTime` = 20000 WHERE (((`summonerId` = 4484) AND (`summonerType` = 0)) AND (`groupId` = 1)) AND (`entry` = 3893);
UPDATE `creature_summon_groups` SET `summonTime` = 20000 WHERE (((`summonerId` = 4484) AND (`summonerType` = 0)) AND (`groupId` = 1)) AND (`entry` = 3893);
UPDATE `creature_summon_groups` SET `summonTime` = 20000 WHERE (((`summonerId` = 4484) AND (`summonerType` = 0)) AND (`groupId` = 2)) AND (`entry` = 3898);
UPDATE `creature_summon_groups` SET `summonTime` = 20000 WHERE (((`summonerId` = 4484) AND (`summonerType` = 0)) AND (`groupId` = 2)) AND (`entry` = 3899);
UPDATE `creature_summon_groups` SET `summonTime` = 20000 WHERE (((`summonerId` = 4484) AND (`summonerType` = 0)) AND (`groupId` = 2)) AND (`entry` = 3900);
