 -- Dreghood Brute smart ai
SET @ENTRY := 16938;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (1693800, 1693801);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 75, 0, 8000, 8000, 10000, 10000, 11, 31553, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 10 seconds (8s initially) (IC) - Self: Cast spell  Hamstring (31553) on Victim'),
(@ENTRY, 0, 1, 0, 38, 0, 100, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 2, 3, 38, 0, 100, 0, 2, 2, 0, 0, 117, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Disable evade'),
(@ENTRY, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 36, 19477, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Update template as if it was creature Fleeing Dreghood Warrior (19477)'),
(@ENTRY, 0, 4, 5, 61, 0, 100, 0, 0, 0, 0, 0, 8, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Set react state to Defensive'),
(@ENTRY, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Play emote ONESHOT_NONE (0)'),
(@ENTRY, 0, 6, 7, 61, 0, 100, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Stop combat'),
(@ENTRY, 0, 7, 8, 61, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 9, 16938, 0, 20, 0, 0, 0, 0, 'On data[2] set to 2 - Creature Dreghood Brute (16938) in 0 - 20 yards: Set creature data #3 to 3'),
(@ENTRY, 0, 8, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 1693800, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Start timed action list id #Dreghood Brute #0 (1693800) (update always) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 2 seconds - Self: Talk 0 to invoker'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 89, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Move randomly in radius 5 yards'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, -399.59, 4792.81, 19.6, 6.18, 'After 2 seconds - Self: Move to position (-399.59, 4792.81, 19.6, 6.18)'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 5 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 9, 10, 38, 0, 100, 0, 3, 3, 0, 0, 117, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Disable evade'),
(@ENTRY, 0, 10, 11, 61, 0, 100, 0, 0, 0, 0, 0, 36, 19477, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Update template as if it was creature Fleeing Dreghood Warrior (19477)'),
(@ENTRY, 0, 11, 12, 61, 0, 100, 0, 0, 0, 0, 0, 8, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Set react state to Defensive'),
(@ENTRY, 0, 12, 13, 61, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Play emote ONESHOT_NONE (0)'),
(@ENTRY, 0, 13, 14, 61, 0, 100, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Stop combat'),
(@ENTRY, 0, 14, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 1693801, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Start timed action list id #Dreghood Brute #1 (1693801) (update always) // -inline'),
(@ENTRY * 100 + 1, 9, 0, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 89, 10, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 2 seconds - Self: Move randomly in radius 10 yards'),
(@ENTRY * 100 + 1, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, -399.59, 4792.81, 19.6, 6.18, 'After 2 seconds - Self: Move to position (-399.59, 4792.81, 19.6, 6.18)'),
(@ENTRY * 100 + 1, 9, 2, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 5 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 15, 0, 38, 0, 100, 0, 1, 1, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 50, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Attack Closest player in 50 yards');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 16938 AND `SourceId` = 0;

DELETE FROM `creature_text` WHERE `CreatureID` = 16937;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(16937, 0, 0, 'Put me out of this misery!', 12, 0, 100, 0, 0, 0, 16471, 0, 'Dreghood Geomancer'),
(16937, 0, 1, 'Forgive me... I have no choice.', 12, 0, 100, 0, 0, 0, 16469, 0, 'Dreghood Geomancer'),
(16937, 0, 2, 'I will do as you ask, demon.  At least for now.', 12, 0, 100, 0, 0, 0, 16470, 0, 'Dreghood Geomancer');

DELETE FROM `creature_text` WHERE `CreatureID` = 20157;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(20157, 0, 0, 'The taskmaster\'s dead!  Quick, Dreghoods!  Run!', 12, 0, 100, 0, 0, 0, 13372, 0, 'Fleeing Dreghood Geomancer'),
(20157, 0, 1, 'Flee!  Flee!', 12, 0, 100, 0, 0, 0, 13373, 0, 'Fleeing Dreghood Geomancer'),
(20157, 0, 2, 'This is our chance!  Run for it!', 12, 0, 100, 0, 0, 0, 13374, 0, 'Fleeing Dreghood Geomancer');

 -- Dreghood Geomancer smart ai
SET @ENTRY := 16937;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (1693700, 1693701);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 2, 0, 100, 1, 0, 50, 0, 0, 11, 32734, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-50%% (once) - Self: Cast spell  Earth Shield (32734) on Self'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 0, 0, 3000, 5000, 11, 34083, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 3 - 5 seconds (0 - 0s initially) (IC) - Self: Cast spell  Fireball (34083) with flags combat move on Victim'),
(@ENTRY, 0, 2, 0, 38, 0, 100, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 3, 4, 38, 0, 100, 0, 2, 2, 0, 0, 117, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Disable evade'),
(@ENTRY, 0, 4, 5, 61, 0, 100, 0, 0, 0, 0, 0, 36, 20157, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Update template as if it was creature Fleeing Dreghood Geomancer (20157)'),
(@ENTRY, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 8, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Set react state to Defensive'),
(@ENTRY, 0, 6, 7, 61, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Play emote ONESHOT_NONE (0)'),
(@ENTRY, 0, 7, 8, 61, 0, 100, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Stop combat'),
(@ENTRY, 0, 8, 9, 61, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 9, 16938, 0, 20, 0, 0, 0, 0, 'On data[2] set to 2 - Creature Dreghood Brute (16938) in 0 - 20 yards: Set creature data #3 to 3'),
(@ENTRY, 0, 9, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 1693700, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Start timed action list id #Dreghood Geomancer #0 (1693700) (update always) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 2 seconds - Self: Talk 0 to invoker'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 89, 5, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Move randomly in radius 5 yards'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, -399.59, 4792.81, 19.6, 6.18, 'After 2 seconds - Self: Move to position (-399.59, 4792.81, 19.6, 6.18)'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 10, 11, 38, 0, 100, 0, 3, 3, 0, 0, 117, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Disable evade'),
(@ENTRY, 0, 11, 12, 61, 0, 100, 0, 0, 0, 0, 0, 36, 20157, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Update template as if it was creature Fleeing Dreghood Geomancer (20157)'),
(@ENTRY, 0, 12, 13, 61, 0, 100, 0, 0, 0, 0, 0, 8, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Set react state to Defensive'),
(@ENTRY, 0, 13, 14, 61, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Play emote ONESHOT_NONE (0)'),
(@ENTRY, 0, 14, 15, 61, 0, 100, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Stop combat'),
(@ENTRY, 0, 15, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 1693701, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Start timed action list id #Dreghood Geomancer #1 (1693701) (update always) // -inline'),
(@ENTRY * 100 + 1, 9, 0, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 89, 10, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 2 seconds - Self: Move randomly in radius 10 yards'),
(@ENTRY * 100 + 1, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 69, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, -399.59, 4792.81, 19.6, 6.18, 'After 2 seconds - Self: Move to position (-399.59, 4792.81, 19.6, 6.18)'),
(@ENTRY * 100 + 1, 9, 2, 0, 0, 0, 100, 0, 3000, 3000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 3 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 16, 0, 38, 0, 100, 0, 1, 1, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 50, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Attack Closest player in 50 yards');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 16937 AND `SourceId` = 0;

 -- Illidari Taskmaster smart ai
SET @ENTRY := 17058;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 2, 0, 100, 1, 0, 20, 0, 0, 11, 34186, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-20%% (once) - Self: Cast spell  Haste Other (34186) on Self'),
(@ENTRY, 0, 1, 0, 0, 0, 75, 0, 2000, 2000, 16000, 16000, 11, 16244, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 16 seconds (2s initially) (IC) - Self: Cast spell  Demoralizing Shout (16244) on Victim'),
(@ENTRY, 0, 2, 3, 4, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Talk 1 to Attacked unit'),
(@ENTRY, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 16938, 20, 0, 0, 0, 0, 0, 'On aggro - Closest alive creature Dreghood Brute (16938) in 20 yards: Set creature data #1 to 1'),
(@ENTRY, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 16937, 20, 0, 0, 0, 0, 0, 'On aggro - Closest alive creature Dreghood Geomancer (16937) in 20 yards: Set creature data #1 to 1'),
(@ENTRY, 0, 5, 0, 1, 0, 100, 0, 15000, 60000, 45000, 90000, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 45 - 90 seconds (15 - 60s initially) (OOC) - Self: Talk 0 to invoker'),
(@ENTRY, 0, 6, 7, 6, 0, 100, 0, 0, 0, 0, 0, 45, 2, 2, 0, 0, 0, 0, 19, 16938, 20, 0, 0, 0, 0, 0, 'On death - Closest alive creature Dreghood Brute (16938) in 20 yards: Set creature data #2 to 2'),
(@ENTRY, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 45, 2, 2, 0, 0, 0, 0, 19, 16937, 20, 0, 0, 0, 0, 0, 'On death - Closest alive creature Dreghood Geomancer (16937) in 20 yards: Set creature data #2 to 2');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 17058 AND `SourceId` = 0;

-- Fear (33924)
DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (33924));
