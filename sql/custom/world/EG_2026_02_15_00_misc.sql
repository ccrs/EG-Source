 -- Wounded Blood Elf Pilgrim smart ai
SET @ENTRY := 16993;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (1699300, 1699301, 1699302, 1699303);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 19, 0, 100, 0, 9375, 0, 0, 0, 80, 1699300, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On player accepted quest The Road to Falcon Watch (9375) - Self: Start timed action list id #Wounded Blood Elf Pilgrim #0 (1699300) (update out of combat) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: storedTarget[1] = Last action invoker\'s party'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Last action invoker: Talk 0 to invoker'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 2, 33, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set faction to Escortee (33)'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 144, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Disable Immune to PC'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 145, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Disable Immune to NPC'),
(@ENTRY * 100, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 83, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Remove npc flags QUESTGIVER'),
(@ENTRY * 100, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 20, 181664, 5, 0, 0, 0, 0, 0, 'After 0 seconds - Closest gameobject Arakkoa Cage (181664) in 5 yards: Activate gameobject'),
(@ENTRY * 100, 9, 7, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 53, 0, 135945, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 4 seconds - Self: Start path #135945, walk, do not repeat, Passive'),
(@ENTRY, 0, 1, 0, 40, 0, 100, 0, 12, 0, 0, 0, 80, 1699301, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 12 of any path reached - Self: Start timed action list id #Wounded Blood Elf Pilgrim #1 (1699301) (update out of combat) // -inline'),
(@ENTRY * 100 + 1, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Pause path for 0 ms'),
(@ENTRY * 100 + 1, 9, 1, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 1 seconds - storedTarget[1]: Talk 1 to invoker'),
(@ENTRY * 100 + 1, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 2.79253, 'After 0 seconds - Self: Set orientation to 2.79253'),
(@ENTRY * 100 + 1, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 107, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Summon creature group 0'),
(@ENTRY, 0, 2, 0, 40, 0, 100, 0, 18, 0, 0, 0, 80, 1699302, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 18 of any path reached - Self: Start timed action list id #Wounded Blood Elf Pilgrim #2 (1699302) (update out of combat) // -inline'),
(@ENTRY * 100 + 2, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Pause path for 0 ms'),
(@ENTRY * 100 + 2, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 90, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set stand state to KNEEL'),
(@ENTRY * 100 + 2, 9, 2, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 1, 2, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 2 seconds - storedTarget[1]: Talk 2 to invoker'),
(@ENTRY * 100 + 2, 9, 3, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 1, 3, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 6 seconds - storedTarget[1]: Talk 3 to invoker'),
(@ENTRY * 100 + 2, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 91, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Remove stand state KNEEL'),
(@ENTRY * 100 + 2, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 107, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Summon creature group 1'),
(@ENTRY, 0, 3, 0, 40, 0, 100, 0, 36, 0, 0, 0, 80, 1699303, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 36 of any path reached - Self: Start timed action list id #Wounded Blood Elf Pilgrim #3 (1699303) (update out of combat) // -inline'),
(@ENTRY * 100 + 3, 9, 0, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 1, 4, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 3 seconds - storedTarget[1]: Talk 4 to invoker'),
(@ENTRY * 100 + 3, 9, 1, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 90, 7, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Set stand state to DEAD'),
(@ENTRY * 100 + 3, 9, 2, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 15, 9375, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 2 seconds - storedTarget[1]: Call quest The Road to Falcon Watch (9375) group event happened'),
(@ENTRY * 100 + 3, 9, 3, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 90, 8, 0, 0, 0, 0, 0, 19, 17015, 5, 0, 0, 0, 0, 0, 'After 1 seconds - Closest alive creature Taleris Dawngazer (17015) in 5 yards: Set stand state to KNEEL'),
(@ENTRY * 100 + 3, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 41, 15000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Despawn in 15 s'),
(@ENTRY * 100 + 3, 9, 5, 0, 0, 0, 100, 0, 3000, 10000, 0, 0, 1, 4, 0, 0, 0, 0, 0, 19, 17015, 5, 0, 0, 0, 0, 0, 'After 3 - 10 seconds - Closest alive creature Taleris Dawngazer (17015) in 5 yards: Talk 4 to invoker'),
(@ENTRY * 100 + 3, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 91, 8, 0, 0, 0, 0, 0, 19, 17015, 5, 0, 0, 0, 0, 0, 'After 0 seconds - Closest alive creature Taleris Dawngazer (17015) in 5 yards: Remove stand state KNEEL'),
(@ENTRY, 0, 4, 0, 82, 0, 100, 0, 0, 0, 0, 0, 63, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature died - Self: Add 1 to counter_1'),
(@ENTRY, 0, 5, 0, 77, 0, 100, 0, 1, 2, 0, 0, 65, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On counter 1 set to 2 - Self: Resume path'),
(@ENTRY, 0, 6, 0, 77, 0, 100, 0, 1, 3, 0, 0, 65, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On counter 1 set to 3 - Self: Resume path'),
(@ENTRY, 0, 7, 0, 7, 0, 100, 0, 0, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Talk 5 to invoker'),
(@ENTRY, 0, 8, 0, 6, 0, 100, 0, 0, 0, 0, 0, 6, 9375, 0, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'On death - storedTarget[1]: Fail quest The Road to Falcon Watch (9375)'),
(@ENTRY, 0, 9, 10, 11, 0, 100, 0, 0, 0, 0, 0, 144, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Enable Immune to PC'),
(@ENTRY, 0, 10, 0, 61, 0, 100, 0, 0, 0, 0, 0, 145, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Enable Immune to NPC');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 16993 AND `SourceId` = 0;
