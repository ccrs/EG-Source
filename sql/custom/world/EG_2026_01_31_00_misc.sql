 -- Wrath Master with guid 68311 smart ai
SET @ENTRY := -68311;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 19005;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (6831100);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 11, 0, 100, 0, 0, 0, 0, 0, 48, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Set active'),
(@ENTRY, 0, 1, 2, 11, 0, 100, 0, 0, 0, 0, 0, 11, 51347, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Cast spell  51347 on Self'),
(@ENTRY, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 67, 1, 6000, 6000, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Trigger timed event timedEvent[1] in 6000 - 6000 ms with 100% chance'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 6831100, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Start timed action list id #6831100 (update always) // -inline'),
(6831100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -278.63, 1534.43, 32.62, 0, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-278.63, 1534.43, 32.62, 0) as summon type follower despawn'),
(6831100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #1 to 1'),
(6831100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -280.33, 1541.21, 33.74, 0, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-280.33, 1541.21, 33.74, 0) as summon type follower despawn'),
(6831100, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 2, 2, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #2 to 2'),
(6831100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -277.73, 1523.79, 31.3, 0, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-277.73, 1523.79, 31.3, 0) as summon type follower despawn'),
(6831100, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #3 to 3'),
(6831100, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -279.23, 1518.37, 30.84, 0, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-279.23, 1518.37, 30.84, 0) as summon type follower despawn'),
(6831100, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 4, 4, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #4 to 4'),
(@ENTRY, 0, 4, 0, 59, 0, 100, 0, 1, 0, 0, 0, 53, 0, 546489, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On timed event timedEvent[1] triggered - Self: Start path #546489, walk, do not repeat, Passive'),
(@ENTRY, 0, 5, 0, 17, 0, 100, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On summoned anyunit - Self: storedTarget[1] = Summon'),
(@ENTRY, 0, 6, 0, 6, 0, 100, 0, 0, 0, 0, 0, 41, 15000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Despawn in 15 s'),
(@ENTRY, 0, 7, 0, 0, 0, 100, 0, 3000, 13000, 15000, 31000, 11, 29574, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 15 - 31 seconds (3 - 13s initially) (IC) - Self: Cast spell  29574 on Victim'),
(@ENTRY, 0, 8, 0, 0, 0, 100, 0, 6000, 19000, 21000, 36000, 11, 35871, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 21 - 36 seconds (6 - 19s initially) (IC) - Self: Cast spell  35871 on Victim');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = -68311 AND `SourceId` = 0;

 -- Wrath Master with guid 68312 smart ai
SET @ENTRY := -68312;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 19005;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (6831200);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 11, 0, 100, 0, 0, 0, 0, 0, 48, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Set active'),
(@ENTRY, 0, 1, 2, 11, 0, 100, 0, 0, 0, 0, 0, 11, 51347, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Cast spell  51347 on Self'),
(@ENTRY, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 67, 1, 1500, 1500, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Trigger timed event timedEvent[1] in 1500 - 1500 ms with 100% chance'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 6831200, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Start timed action list id #6831200 (update always) // -inline'),
(6831200, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -170.7, 1508.39, 27.5, 3.14, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-170.7, 1508.39, 27.5, 3.14) as summon type follower despawn'),
(6831200, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #1 to 1'),
(6831200, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -169.63, 1503.79, 27.3, 3.14, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-169.63, 1503.79, 27.3, 3.14) as summon type follower despawn'),
(6831200, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 2, 2, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #2 to 2'),
(6831200, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -169.62, 1519.02, 28.6, 3.14, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-169.62, 1519.02, 28.6, 3.14) as summon type follower despawn'),
(6831200, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #3 to 3'),
(6831200, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -166.86, 1524.43, 29.4, 3.14, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-166.86, 1524.43, 29.4, 3.14) as summon type follower despawn'),
(6831200, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 4, 4, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #4 to 4'),
(@ENTRY, 0, 4, 0, 59, 0, 100, 0, 1, 0, 0, 0, 53, 0, 546497, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On timed event timedEvent[1] triggered - Self: Start path #546497, walk, do not repeat, Passive'),
(@ENTRY, 0, 5, 0, 17, 0, 100, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On summoned anyunit - Self: storedTarget[1] = Summon'),
(@ENTRY, 0, 6, 0, 6, 0, 100, 0, 0, 0, 0, 0, 41, 15000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Despawn in 15 s'),
(@ENTRY, 0, 7, 0, 0, 0, 100, 0, 3000, 13000, 15000, 31000, 11, 29574, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 15 - 31 seconds (3 - 13s initially) (IC) - Self: Cast spell  29574 on Victim'),
(@ENTRY, 0, 8, 0, 0, 0, 100, 0, 6000, 19000, 21000, 36000, 11, 35871, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 21 - 36 seconds (6 - 19s initially) (IC) - Self: Cast spell  35871 on Victim');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = -68312 AND `SourceId` = 0;

 -- Wrath Master with guid 68313 smart ai
SET @ENTRY := -68313;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 19005;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (6831300);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 11, 0, 100, 0, 0, 0, 0, 0, 48, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Set active'),
(@ENTRY, 0, 1, 2, 11, 0, 100, 0, 0, 0, 0, 0, 11, 51347, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Cast spell  51347 on Self'),
(@ENTRY, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 67, 1, 4000, 4000, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Trigger timed event timedEvent[1] in 4000 - 4000 ms with 100% chance'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 6831300, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Start timed action list id #6831300 (update always) // -inline'),
(6831300, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -100.15, 1888.8, 77.4, 2.54, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-100.15, 1888.8, 77.4, 2.54) as summon type follower despawn'),
(6831300, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #1 to 1'),
(6831300, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -100.48, 1885.22, 77.35, 2.54, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-100.48, 1885.22, 77.35, 2.54) as summon type follower despawn'),
(6831300, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 2, 2, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #2 to 2'),
(6831300, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -95.23, 1895.04, 76.5, 2.54, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-95.23, 1895.04, 76.5, 2.54) as summon type follower despawn'),
(6831300, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #3 to 3'),
(6831300, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -91.67, 1896.62, 75.8, 2.54, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-91.67, 1896.62, 75.8, 2.54) as summon type follower despawn'),
(6831300, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 4, 4, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #4 to 4'),
(@ENTRY, 0, 4, 0, 59, 0, 100, 0, 1, 0, 0, 0, 53, 0, 546505, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On timed event timedEvent[1] triggered - Self: Start path #546505, walk, do not repeat, Passive'),
(@ENTRY, 0, 5, 0, 17, 0, 100, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On summoned anyunit - Self: storedTarget[1] = Summon'),
(@ENTRY, 0, 6, 0, 6, 0, 100, 0, 0, 0, 0, 0, 41, 15000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Despawn in 15 s'),
(@ENTRY, 0, 7, 0, 0, 0, 100, 0, 3000, 13000, 15000, 31000, 11, 29574, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 15 - 31 seconds (3 - 13s initially) (IC) - Self: Cast spell  29574 on Victim'),
(@ENTRY, 0, 8, 0, 0, 0, 100, 0, 6000, 19000, 21000, 36000, 11, 35871, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 21 - 36 seconds (6 - 19s initially) (IC) - Self: Cast spell  35871 on Victim');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = -68313 AND `SourceId` = 0;

 -- Wrath Master with guid 68314 smart ai
SET @ENTRY := -68314;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = 19005;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (6831400);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 11, 0, 100, 0, 0, 0, 0, 0, 48, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Set active'),
(@ENTRY, 0, 1, 2, 11, 0, 100, 0, 0, 0, 0, 0, 11, 51347, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Cast spell  51347 on Self'),
(@ENTRY, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 67, 1, 4000, 4000, 0, 0, 100, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Trigger timed event timedEvent[1] in 4000 - 4000 ms with 100% chance'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 6831400, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Start timed action list id #6831400 (update always) // -inline'),
(6831400, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -415.5, 1829.44, 74.8, 4.53, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-415.5, 1829.44, 74.8, 4.53) as summon type follower despawn'),
(6831400, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #1 to 1'),
(6831400, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -412.43, 1830.42, 75.4, 4.53, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-412.43, 1830.42, 75.4, 4.53) as summon type follower despawn'),
(6831400, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 2, 2, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #2 to 2'),
(6831400, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -423.48, 1831.24, 74.5, 4.53, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-423.48, 1831.24, 74.5, 4.53) as summon type follower despawn'),
(6831400, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #3 to 3'),
(6831400, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18944, 9, 5000, 0, 0, 0, 8, 0, 0, 0, -427.7, 1833.82, 75, 4.53, 'After 0 seconds - Self: Summon creature Fel Soldier (18944) at (-427.7, 1833.82, 75, 4.53) as summon type follower despawn'),
(6831400, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 4, 4, 0, 0, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 'After 0 seconds - storedTarget[1]: Set creature data #4 to 4'),
(@ENTRY, 0, 4, 0, 59, 0, 100, 0, 1, 0, 0, 0, 53, 0, 546513, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On timed event timedEvent[1] triggered - Self: Start path #546513, walk, do not repeat, Passive'),
(@ENTRY, 0, 5, 0, 17, 0, 100, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On summoned anyunit - Self: storedTarget[1] = Summon'),
(@ENTRY, 0, 6, 0, 6, 0, 100, 0, 0, 0, 0, 0, 41, 15000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Despawn in 15 s'),
(@ENTRY, 0, 7, 0, 0, 0, 100, 0, 3000, 13000, 15000, 31000, 11, 29574, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 15 - 31 seconds (3 - 13s initially) (IC) - Self: Cast spell  29574 on Victim'),
(@ENTRY, 0, 8, 0, 0, 0, 100, 0, 6000, 19000, 21000, 36000, 11, 35871, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 21 - 36 seconds (6 - 19s initially) (IC) - Self: Cast spell  35871 on Victim');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = -68314 AND `SourceId` = 0;

 -- Fel Soldier smart ai
SET @ENTRY := 18944;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 38, 0, 100, 0, 1, 1, 0, 0, 29, 1, 120, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Follow Owner/Summoner by distance 1, angle 120'),
(@ENTRY, 0, 1, 0, 38, 0, 100, 0, 2, 2, 0, 0, 29, 6, 120, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'On data[2] set to 2 - Self: Follow Owner/Summoner by distance 6, angle 120'),
(@ENTRY, 0, 2, 0, 38, 0, 100, 0, 3, 3, 0, 0, 29, 1, 240, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'On data[3] set to 3 - Self: Follow Owner/Summoner by distance 1, angle 240'),
(@ENTRY, 0, 3, 0, 38, 0, 100, 0, 4, 4, 0, 0, 29, 6, 240, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'On data[4] set to 4 - Self: Follow Owner/Summoner by distance 6, angle 240'),
(@ENTRY, 0, 4, 5, 54, 0, 100, 0, 0, 0, 0, 0, 11, 51347, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  51347 on Self'),
(@ENTRY, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 59, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Set walk'),
(@ENTRY, 0, 6, 0, 0, 0, 100, 0, 3000, 12000, 9000, 15000, 11, 15496, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 9 - 15 seconds (3 - 12s initially) (IC) - Self: Cast spell  15496 on Victim'),
(@ENTRY, 0, 7, 0, 0, 0, 100, 0, 6000, 20000, 16000, 33000, 11, 32009, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 16 - 33 seconds (6 - 20s initially) (IC) - Self: Cast spell  32009 on Victim'),
(@ENTRY, 0, 8, 0, 60, 0, 100, 0, 5000, 5000, 5000, 5000, 101, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 5 seconds - Self: Set home position to current position');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 18944 AND `SourceId` = 0;

UPDATE `waypoint_data` SET `position_x` = -241.178, `position_y` = 1841.39, `position_z` = 92.7873, `delay` = 0 WHERE (`id` = 546513) and (`point` = 12);
