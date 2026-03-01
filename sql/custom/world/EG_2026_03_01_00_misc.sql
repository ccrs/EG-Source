 -- Har'koa's Kitten smart ai
SET @ENTRY := 28665;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2866500);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 54, 0, 100, 512, 0, 0, 0, 0, 11, 52188, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  Stealth (52188) on Self'),
(@ENTRY, 0, 1, 0, 60, 0, 100, 513, 1000, 1000, 0, 0, 53, 1, 229321, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Time = 1 seconds - Self: Start path #229321, run, do not repeat, Passive'),
(@ENTRY, 0, 2, 0, 40, 0, 100, 512, 1, 229321, 0, 0, 1, 0, 0, 1, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 1 of path 229321 reached - Self: Talk 0 to Owner/Summoner'),
(@ENTRY, 0, 3, 4, 40, 0, 100, 512, 10, 229321, 0, 0, 54, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 10 of path 229321 reached - Self: Pause path for 1000 ms'),
(@ENTRY, 0, 4, 0, 61, 0, 100, 512, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 10 of path 229321 reached - Self: Talk 1 to Owner/Summoner'),
(@ENTRY, 0, 5, 6, 40, 0, 100, 512, 18, 229321, 0, 0, 54, 1000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 18 of path 229321 reached - Self: Pause path for 1000 ms'),
(@ENTRY, 0, 6, 7, 61, 0, 100, 512, 0, 0, 0, 0, 59, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 18 of path 229321 reached - Self: Set walk'),
(@ENTRY, 0, 7, 0, 61, 0, 100, 512, 0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 18 of path 229321 reached - Self: Talk 2 to Owner/Summoner'),
(@ENTRY, 0, 8, 0, 40, 0, 100, 512, 21, 229321, 0, 0, 1, 0, 0, 0, 0, 0, 0, 19, 28671, 0, 0, 0, 0, 0, 0, 'On wapoint 21 of path 229321 reached - Closest alive creature Prophet of Quetz\'lun (28671) in 100 yards: Talk 0 to invoker'),
(@ENTRY, 0, 9, 10, 40, 0, 100, 512, 22, 229321, 0, 0, 59, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 22 of path 229321 reached - Self: Set run'),
(@ENTRY, 0, 10, 11, 61, 0, 100, 512, 0, 0, 0, 0, 1, 3, 0, 1, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 22 of path 229321 reached - Self: Talk 3 to Owner/Summoner'),
(@ENTRY, 0, 11, 0, 61, 0, 100, 512, 0, 0, 0, 0, 11, 39870, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 22 of path 229321 reached - Self: Cast spell  Speed Burst (39870) on Self'),
(@ENTRY, 0, 12, 0, 40, 0, 100, 512, 34, 229321, 0, 0, 97, 30, 10, 0, 0, 0, 0, 1, 0, 0, 0, 5651.19, -3790.46, 361.974, 0, 'On wapoint 34 of path 229321 reached - Self: Jump to pos (5651.19, -3790.46, 361.974, 0) with speed XY 30 and speed Z 10'),
(@ENTRY, 0, 13, 14, 40, 0, 100, 512, 11, 2293201, 0, 0, 1, 5, 0, 1, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 11 of path 2293201 reached - Self: Talk 5 to Owner/Summoner'),
(@ENTRY, 0, 14, 15, 61, 0, 100, 512, 0, 0, 0, 0, 15, 12665, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 11 of path 2293201 reached - Self: Call quest I Sense a Disturbance (12665) group event happened'),
(@ENTRY, 0, 15, 16, 61, 0, 100, 512, 0, 0, 0, 0, 28, 25673, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 11 of path 2293201 reached - Self: Remove aura due to spell  Riding Har\'koa\'s Kitten (25673)'),
(@ENTRY, 0, 16, 0, 61, 0, 100, 512, 0, 0, 0, 0, 41, 2000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 11 of path 2293201 reached - Self: Despawn in 2 s'),
(@ENTRY, 0, 17, 18, 34, 0, 100, 512, 16, 1004, 0, 0, 28, 39870, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type EFFECT_MOTION_TYPE inform, point 1004 - Self: Remove aura due to spell  Speed Burst (39870)'),
(@ENTRY, 0, 18, 0, 61, 0, 100, 512, 0, 0, 0, 0, 80, 2866500, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type EFFECT_MOTION_TYPE inform, point 1004 - Self: Start timed action list id #Har\'koa\'s Kitten #0 (2866500) (update always) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 512, 0, 0, 0, 0, 1, 4, 0, 1, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Talk 4 to Owner/Summoner'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 512, 1000, 1000, 0, 0, 53, 1, 2293201, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Start path #2293201, run, do not repeat, Passive');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 28665 AND `SourceId` = 0;
