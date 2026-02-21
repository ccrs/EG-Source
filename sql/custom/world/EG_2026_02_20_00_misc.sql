 -- Frostbrood Destroyer smart ai
SET @ENTRY := 30575;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 5000, 10000, 10000, 15000, 11, 57477, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 10 - 15 seconds (5 - 10s initially) (IC) - Self: Cast spell  Freezing Breath (57477) on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 10000, 15000, 15000, 25000, 11, 53363, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 15 - 25 seconds (10 - 15s initially) (IC) - Self: Cast spell  Wing Buffet (53363) on Victim'),
(@ENTRY, 0, 2, 3, 8, 0, 100, 0, 57542, 0, 0, 0, 117, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell  Argent Cannon Assault (57542) hit - Self: Disable evade'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 102, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Argent Cannon Assault (57542) hit - Self: Do not regenerate HP'),
(@ENTRY, 0, 4, 5, 8, 0, 100, 0, 57415, 0, 0, 0, 117, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell  The Reckoning (57415) hit - Self: Disable evade'),
(@ENTRY, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 102, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  The Reckoning (57415) hit - Self: Do not regenerate HP');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30575 AND `SourceId` = 0;

UPDATE `creature_template` SET `RegenHealth` = 1 WHERE (`entry` = 30575);

DELETE FROM `creature_template_movement` WHERE `CreatureId`= 30575;
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`, `InteractionPauseTimer`) VALUES
(30575, 2, 0, 0, 0, 0, 1, 0);

DELETE FROM `creature_template_movement` WHERE `CreatureId`= 29754;
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`, `InteractionPauseTimer`) VALUES
(29754, 0, 0, 1, 1, 0, 0, 0);

 -- Kristen Dipswitch smart ai
SET @ENTRY := 18294;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (1829400, 1829401);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 11, 0, 100, 0, 0, 0, 0, 0, 83, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Remove npc flags GOSSIP, QUESTGIVER'),
(@ENTRY, 0, 1, 0, 11, 0, 100, 0, 0, 0, 0, 0, 53, 1, 146353, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Start path #146353, run, do not repeat, Passive'),
(@ENTRY, 0, 2, 0, 40, 0, 100, 0, 19, 146353, 0, 0, 80, 1829400, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 19 of path 146353 reached - Self: Start timed action list id #Kristen Dipswitch #0 (1829400) (update out of combat) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 12, 18297, 4, 180000, 0, 0, 0, 8, 0, 0, 0, -1463.63, 6363.4, 36.9237, 0, 'After 0 seconds - Self: Summon creature Gankly Rottenfist (18297) at (-1463.63, 6363.4, 36.9237, 0) as summon type timed despawn out of combat with duration 180 seconds'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 43, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 2 seconds - Self: Dismount'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 2.97859, 'After 1 seconds - Self: Set orientation to 2.97859'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 2, 250, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Set faction to Escortee (250)'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Talk 0 to invoker'),
(@ENTRY, 0, 3, 0, 0, 0, 100, 1, 1000, 1000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Time = 1 seconds (IC) - Self: Talk 1 to invoker'),
(@ENTRY, 0, 4, 0, 6, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 18297, 0, 0, 0, 0, 0, 0, 'On death - Closest alive creature Gankly Rottenfist (18297) in 100 yards: Set creature data #1 to 1'),
(@ENTRY, 0, 5, 0, 38, 0, 100, 0, 1, 2, 0, 0, 80, 1829401, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Self: Start timed action list id #Kristen Dipswitch #1 (1829401) (update out of combat) // -inline'),
(@ENTRY * 100 + 1, 9, 0, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 2, 35, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 2 seconds - Self: Set faction to Friendly (35)'),
(@ENTRY * 100 + 1, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 82, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Add npc flags GOSSIP, QUESTGIVER'),
(@ENTRY * 100 + 1, 9, 2, 0, 0, 0, 100, 0, 300000, 300000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 300 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 6, 0, 11, 0, 100, 0, 0, 0, 0, 0, 43, 0, 14375, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Mount to model 14375'),
(@ENTRY, 0, 7, 0, 11, 0, 100, 0, 0, 0, 0, 0, 48, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Set active');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 18294 AND `SourceId` = 0;

 -- Gankly Rottenfist smart ai
SET @ENTRY := 18297;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (1829700, 1829701);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 11, 32199, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  Stealth (32199) on Self'),
(@ENTRY, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 53, 0, 146377, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Start path #146377, walk, do not repeat, Passive'),
(@ENTRY, 0, 2, 0, 40, 0, 100, 0, 10, 146377, 0, 0, 80, 1829700, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 10 of path 146377 reached - Self: Start timed action list id #Gankly Rottenfist #0 (1829700) (update out of combat) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 500, 500, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.42229, 'After 0.5 seconds - Self: Set orientation to 3.42229'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 500, 500, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0.5 seconds - Self: Talk 0 to invoker'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 144, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 2 seconds - Self: Disable Immune to PC'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 145, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Disable Immune to NPC'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 19, 18294, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Attack Closest alive creature Kristen Dipswitch (18294) in 100 yards'),
(@ENTRY, 0, 3, 0, 38, 0, 100, 0, 1, 1, 0, 0, 80, 1829701, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Start timed action list id #Gankly Rottenfist #1 (1829701) (update out of combat) // -inline'),
(@ENTRY * 100 + 1, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 144, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Enable Immune to PC'),
(@ENTRY * 100 + 1, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set react state to Passive'),
(@ENTRY * 100 + 1, 9, 2, 0, 0, 0, 100, 0, 2000, 2000, 0, 0, 28, 32199, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 2 seconds - Self: Remove aura due to spell  Stealth (32199)'),
(@ENTRY * 100 + 1, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Talk 1 to invoker'),
(@ENTRY * 100 + 1, 9, 4, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 66, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 3.13852, 'After 5 seconds - Self: Set orientation to 3.13852'),
(@ENTRY * 100 + 1, 9, 5, 0, 0, 0, 100, 0, 500, 500, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0.5 seconds - Self: Talk 2 to invoker'),
(@ENTRY * 100 + 1, 9, 6, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 19, 18218, 0, 0, 0, 0, 0, 0, 'After 5 seconds - Closest alive creature Harold Lane (18218) in 100 yards: Talk 0 to invoker'),
(@ENTRY * 100 + 1, 9, 7, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 45, 1, 3, 0, 0, 0, 0, 19, 18200, 0, 0, 0, 0, 0, 0, 'After 5 seconds - Closest alive creature Shado \'Fitz\' Farstrider (18200) in 100 yards: Set creature data #1 to 3'),
(@ENTRY * 100 + 1, 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 1, 3, 0, 0, 0, 0, 19, 18180, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Closest alive creature Hemet Nesingwary (18180) in 100 yards: Set creature data #1 to 3'),
(@ENTRY, 0, 4, 0, 6, 0, 100, 0, 0, 0, 0, 0, 45, 1, 2, 0, 0, 0, 0, 19, 18294, 0, 0, 0, 0, 0, 0, 'On death - Closest alive creature Kristen Dipswitch (18294) in 100 yards: Set creature data #1 to 2'),
(@ENTRY, 0, 5, 0, 0, 0, 100, 0, 5000, 7000, 11000, 13000, 11, 30981, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 11 - 13 seconds (5 - 7s initially) (IC) - Self: Cast spell  Crippling Poison (30981) on Victim'),
(@ENTRY, 0, 6, 0, 0, 0, 100, 0, 9000, 11000, 15000, 17000, 11, 41176, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 15 - 17 seconds (9 - 11s initially) (IC) - Self: Cast spell  Shadowstep (41176) on Victim'),
(@ENTRY, 0, 7, 0, 0, 0, 100, 0, 3000, 5000, 9000, 11000, 11, 15581, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 9 - 11 seconds (3 - 5s initially) (IC) - Self: Cast spell  Sinister Strike (15581) on Victim'),
(@ENTRY, 0, 8, 0, 8, 0, 100, 0, 32190, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell  Shoot (32190) hit - Self: Die');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 18297 AND `SourceId` = 0;

 -- Valduran the Stormborn smart ai
SET @ENTRY := 29368;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2936800);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 100, 0, 0, 1500, 3400, 4700, 11, 56326, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 3.4 - 4.7 seconds (0 - 1.5s initially) (IC) - Self: Cast spell  Lightning Bolt (56326) with flags combat move on Victim'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 15000, 18000, 29000, 33000, 11, 56322, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 29 - 33 seconds (15 - 18s initially) (IC) - Self: Cast spell  Spark Frenzy (56322) on Self'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 3000, 8000, 23000, 26000, 11, 56319, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 23 - 26 seconds (3 - 8s initially) (IC) - Self: Cast spell  Ball Lightning (56319) on Random hostile'),
(@ENTRY, 0, 3, 4, 8, 0, 100, 0, 56189, 0, 0, 0, 11, 44762, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On spell  Sound War Horn (56189) hit - Self: Cast spell  Camera Shake - Med (44762) on Caster'),
(@ENTRY, 0, 4, 5, 61, 0, 100, 0, 0, 0, 0, 0, 28, 56220, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Sound War Horn (56189) hit - Self: Remove aura due to spell  Valduran\'s Channel (56220)'),
(@ENTRY, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Sound War Horn (56189) hit - Self: Set emote state to 0'),
(@ENTRY, 0, 6, 0, 38, 0, 100, 0, 0, 1, 0, 0, 80, 2936800, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[0] set to 1 - Self: Start timed action list id #Valduran the Stormborn #0 (2936800) (update out of combat) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Talk 0 to invoker'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 4000, 4000, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 4 seconds - Self: Talk 1 to invoker'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 11, 61361, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 5 seconds - Self: Cast spell  Lightning Teleport (61361) on Self'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 60, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Enable gravity'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 99, 3, 0, 0, 0, 0, 0, 20, 191510, 10, 0, 0, 0, 0, 0, 'After 0 seconds - Closest gameobject Valduran\'s Shield (191510) in 10 yards: Set gameobjects loot state GO_JUST_DEACTIVATED'),
(@ENTRY * 100, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 2, 2102, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set faction to Actor Evil (2102)'),
(@ENTRY * 100, 9, 6, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 144, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Self: Disable Immune to PC'),
(@ENTRY * 100, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 145, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Disable Immune to NPC'),
(@ENTRY, 0, 7, 0, 25, 0, 100, 0, 0, 0, 0, 0, 60, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Disable gravity'),
(@ENTRY, 0, 8, 9, 6, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 11, 29801, 20, 0, 0, 0, 0, 0, 'On death - Creature Bouldercrag the Rockshaper (29801) in 20 yd: Set creature data #1 to 1'),
(@ENTRY, 0, 9, 0, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 11, 30152, 20, 0, 0, 0, 0, 0, 'On death - Creature Bruor Ironbane (30152) in 20 yd: Set creature data #1 to 1'),
(@ENTRY, 0, 10, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 10, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly respawn in 10 seconds');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 29368 AND `SourceId` = 0;

DELETE FROM `creature_template_movement` WHERE `CreatureId`= 29368;
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`, `InteractionPauseTimer`) VALUES
(29368, 1, 0, 0, 0, 0, 0, 0);
