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

 -- Akali smart ai
SET @ENTRY := 28952;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2895200, 2895201);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 38, 0, 100, 0, 0, 1, 0, 0, 80, 2895200, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[0] set to 1 - Self: Start timed action list id #Akali #0 (2895200) (update always) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 2 seconds - Self: Talk 0 to invoker'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 15, 12721, 0, 0, 0, 0, 0, 18, 50, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Players in 50 yards: Call quest Rampage (12721) group event happened'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 107, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Summon creature group 1'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 101661, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 101661 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 101662, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 101662 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 101663, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 101663 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 101665, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 101665 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 101666, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 101666 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 101667, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 101667 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 9, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 101668, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 101668 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 10, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 101669, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 101669 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 11, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 203572, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 203572 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 12, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 203573, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 203573 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 13, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 203574, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 203574 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 14, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 203575, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 203575 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 15, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 203576, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 203576 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 16, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 10, 203577, 28988, 0, 0, 0, 0, 0, 'After 0 seconds - Creature Akali Subduer (28988) with guid 203577 (fetching): Set creature data #0 to 2'),
(@ENTRY * 100, 9, 17, 0, 0, 0, 100, 0, 4600, 4600, 0, 0, 145, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 4.6 seconds - Self: Disable Immune to NPC'),
(@ENTRY * 100, 9, 18, 0, 0, 0, 100, 0, 55000, 55000, 0, 0, 12, 28996, 8, 0, 0, 0, 0, 8, 0, 0, 0, 6882.03, -4571, 442.312, 2.37365, 'After 55 seconds - Self: Summon creature Prophet of Akali (28996) at (6882.03, -4571, 442.312, 2.37365) as summon type manual despawn'),
(@ENTRY, 0, 1, 2, 8, 0, 100, 0, 52859, 0, 0, 0, 117, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell  Submission (52859) hit - Self: Disable evade'),
(@ENTRY, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 145, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Submission (52859) hit - Self: Enable Immune to NPC'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 69, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, 6829.59, -4525.52, 442.068, 0, 'On spell  Submission (52859) hit - Self: Move to position (6829.59, -4525.52, 442.068, 0) (point id 1)'),
(@ENTRY, 0, 4, 5, 34, 0, 100, 0, 0, 1, 0, 0, 2, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement inform, point 1 - Self: Set faction to Friendly (35)'),
(@ENTRY, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 19, 28996, 100, 0, 0, 0, 0, 0, 'On movement inform, point 1 - Self: Look at Closest alive creature Prophet of Akali (28996) in 100 yards'),
(@ENTRY, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 19, 28996, 100, 0, 0, 0, 0, 0, 'On movement inform, point 1 - Closest alive creature Prophet of Akali (28996) in 100 yards: Set creature data #0 to 1'),
(@ENTRY, 0, 7, 0, 9, 0, 100, 0, 0, 80, 10000, 10000, 11, 52856, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 80 yards (cooldown 10000 - 10000 ms) - Self: Cast spell  Charge (52856) on Random hostile (not top)'),
(@ENTRY, 0, 8, 9, 11, 0, 100, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Reset faction'),
(@ENTRY, 0, 9, 10, 61, 0, 100, 0, 0, 0, 0, 0, 102, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Do not regenerate HP'),
(@ENTRY, 0, 10, 11, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 113548, 26298, 0, 0, 0, 0, 0, 'On respawn - Creature ELM General Purpose Bunny (scale x0.01) Large (26298) with guid 113548 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 11, 12, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 113549, 26298, 0, 0, 0, 0, 0, 'On respawn - Creature ELM General Purpose Bunny (scale x0.01) Large (26298) with guid 113549 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 12, 13, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 113550, 26298, 0, 0, 0, 0, 0, 'On respawn - Creature ELM General Purpose Bunny (scale x0.01) Large (26298) with guid 113550 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 13, 14, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 113551, 26298, 0, 0, 0, 0, 0, 'On respawn - Creature ELM General Purpose Bunny (scale x0.01) Large (26298) with guid 113551 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 14, 15, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 61994, 26298, 0, 0, 0, 0, 0, 'On respawn - Creature ELM General Purpose Bunny (scale x0.01) Large (26298) with guid 61994 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 15, 16, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 61995, 26298, 0, 0, 0, 0, 0, 'On respawn - Creature ELM General Purpose Bunny (scale x0.01) Large (26298) with guid 61995 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 16, 17, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 101661, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 101661 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 17, 18, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 101662, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 101662 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 18, 19, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 101663, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 101663 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 19, 20, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 101665, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 101665 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 20, 21, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 101666, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 101666 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 21, 22, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 101667, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 101667 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 22, 23, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 101668, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 101668 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 23, 24, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 101669, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 101669 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 24, 25, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 203572, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 203572 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 25, 26, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 203573, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 203573 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 26, 27, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 203574, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 203574 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 27, 28, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 203575, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 203575 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 28, 29, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 203576, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 203576 (fetching): Set creature data #0 to 1'),
(@ENTRY, 0, 29, 0, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 10, 203577, 28988, 0, 0, 0, 0, 0, 'On respawn - Creature Akali Subduer (28988) with guid 203577 (fetching): Set creature data #0 to 1');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 28952 AND `SourceId` = 0;
