 -- Mjordin Combatant smart ai
SET @ENTRY := 30037;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (3003700, 3003701);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 25, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Set event phase to phase 1'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 17, 333, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set emote state to STATE_READY1H (333)'),
(@ENTRY, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 8, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Set react state to Defensive'),
(@ENTRY, 0, 3, 4, 1, 1, 100, 0, 3000, 3000, 3000, 3000, 5, 36, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 3 seconds (OOC) - Self: Play emote ONESHOT_ATTACK1H (36)'),
(@ENTRY, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 8, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 3 seconds (OOC) - Self: Set react state to Defensive'),
(@ENTRY, 0, 5, 0, 1, 1, 100, 0, 3000, 3000, 3000, 3000, 8, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 3 seconds (OOC) - Self: Set react state to Aggressive'),
(@ENTRY, 0, 6, 0, 1, 1, 20, 0, 10000, 90000, 90000, 120000, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 90 - 120 seconds (10 - 90s initially) (OOC) - Self: Talk 1 to invoker'),
(@ENTRY, 0, 7, 0, 8, 1, 100, 0, 21855, 0, 0, 0, 80, 3003700, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Challenge Flag (21855) hit - Self: Start timed action list id #Mjordin Combatant #0 (3003700) (update out of combat) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set emote state to 0'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Look at Last action invoker'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 19, 30037, 8, 0, 0, 0, 0, 0, 'After 0 seconds - Closest alive creature Mjordin Combatant (30037) in 8 yards: Set creature data #1 to 1'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 0, 1000, 1000, 0, 0, 1, 2, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'After 1 seconds - Last action invoker: Talk 2 to invoker'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 52991, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Cast spell  Duel Flag (52991) on Self'),
(@ENTRY * 100, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 8, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set react state to Defensive'),
(@ENTRY * 100, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 22, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Set event phase to phase 3'),
(@ENTRY * 100, 9, 7, 0, 0, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Attack Last action invoker'),
(@ENTRY, 0, 8, 0, 38, 1, 100, 0, 1, 1, 0, 0, 80, 3003701, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Start timed action list id #Mjordin Combatant #1 (3003701) (update always) // -inline'),
(@ENTRY * 100 + 1, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Set event phase to phase 2'),
(@ENTRY * 100 + 1, 9, 1, 0, 0, 0, 100, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set emote state to 0'),
(@ENTRY * 100 + 1, 9, 2, 0, 0, 0, 100, 0, 5000, 5000, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 5 seconds - Self: Despawn instantly'),
(@ENTRY, 0, 9, 0, 4, 1, 100, 0, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Set event phase to phase 2'),
(@ENTRY, 0, 10, 0, 4, 2, 30, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Talk 0 to Attacked unit'),
(@ENTRY, 0, 11, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 61227, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Cast spell  Jump Attack (61227) on Attacked unit'),
(@ENTRY, 0, 12, 0, 0, 0, 100, 0, 1000, 2600, 13000, 14000, 11, 50370, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 13 - 14 seconds (1 - 2.6s initially) (IC) - Self: Cast spell  Sunder Armor (50370) on Victim'),
(@ENTRY, 0, 13, 0, 0, 0, 100, 0, 7000, 9000, 9000, 11000, 11, 32736, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 9 - 11 seconds (7 - 9s initially) (IC) - Self: Cast spell  Mortal Strike (32736) on Victim'),
(@ENTRY, 0, 14, 0, 0, 0, 100, 0, 6000, 8000, 20000, 25000, 11, 49807, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 20 - 25 seconds (6 - 8s initially) (IC) - Self: Cast spell  Whirlwind (49807) on Self'),
(@ENTRY, 0, 15, 0, 6, 4, 100, 0, 0, 0, 0, 0, 33, 30038, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On death - Killer: Give kill credit Mjordin Combatant Kill Credit Bunny (30038)');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30037 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 4, 30037, 0, 0, 29, 1, 30037, 8, 0, 0, 'There is creature Mjordin Combatant (30037) within range 8 yards to Object'),
(22, 6, 30037, 0, 0, 29, 1, 30037, 8, 0, 1, 'There is no creature Mjordin Combatant (30037) within range 8 yards to Object');

 -- Sigrid Iceborn smart ai
SET @ENTRY := 31242;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 1, 0, 100, 1, 0, 0, 0, 0, 11, 61165, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Once (OOC) - Self: Cast spell  Frostbite Weapon (61165) on Self'),
(@ENTRY, 0, 1, 0, 9, 0, 100, 0, 0, 2, 10000, 15000, 11, 57635, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 2 yards (cooldown 10000 - 15000 ms) - Self: Cast spell  Disengage (57635) on Victim'),
(@ENTRY, 0, 2, 0, 9, 0, 100, 0, 0, 3, 8000, 20000, 11, 61170, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 3 yards (cooldown 8000 - 20000 ms) - Self: Cast spell  Flash Freeze (61170) on Victim'),
(@ENTRY, 0, 3, 0, 9, 0, 100, 0, 5, 30, 5000, 10000, 11, 61168, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 5 - 30 yards (cooldown 5000 - 10000 ms) - Self: Cast spell  Throw (61168) on Random hostile'),
(@ENTRY, 0, 4, 0, 60, 0, 100, 1, 2000, 2000, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Time = 2 seconds - Self: Talk 0 to invoker'),
(@ENTRY, 0, 5, 0, 5, 0, 100, 0, 0, 0, 0, 0, 45, 20, 20, 0, 0, 0, 0, 10, 123494, 31135, 0, 0, 0, 0, 0, 'On kill creature 0 - Creature Geirrvif (31135) with guid 123494 (fetching): Set creature data #20 to 20'),
(@ENTRY, 0, 6, 0, 6, 0, 100, 0, 0, 0, 0, 0, 45, 3, 3, 0, 0, 0, 0, 10, 123494, 31135, 0, 0, 0, 0, 0, 'On death - Creature Geirrvif (31135) with guid 123494 (fetching): Set creature data #3 to 3'),
(@ENTRY, 0, 7, 8, 1, 0, 100, 1, 60000, 60000, 0, 0, 45, 19, 19, 0, 0, 0, 0, 10, 123494, 31135, 0, 0, 0, 0, 0, 'Time = 60 seconds (OOC) - Creature Geirrvif (31135) with guid 123494 (fetching): Set creature data #19 to 19'),
(@ENTRY, 0, 8, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Time = 60 seconds (OOC) - Self: Despawn instantly'),
(@ENTRY, 0, 9, 10, 54, 0, 100, 0, 0, 0, 0, 0, 12, 30159, 7, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Summon creature Sigrid Iceborn\'s Proto-Drake (30159) at Self\'s position, moved by offset (0, 0, 0, 0) as summon type dead despawn with duration 0 seconds'),
(@ENTRY, 0, 10, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 46598, 0, 0, 0, 0, 0, 19, 30159, 100, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  Ride Vehicle Hardcoded (46598) on Closest alive creature Sigrid Iceborn\'s Proto-Drake (30159) in 100 yards'),
(@ENTRY, 0, 11, 12, 38, 0, 100, 1, 1, 1, 0, 0, 101, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Set home position to current position'),
(@ENTRY, 0, 12, 13, 61, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 90, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Attack Closest player in 90 yards'),
(@ENTRY, 0, 13, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Talk 1 to invoker'),
(@ENTRY, 0, 14, 0, 1, 0, 100, 1, 5000, 5000, 0, 0, 144, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Time = 5 seconds (OOC) - Self: Disable Immune to PC'),
(@ENTRY, 0, 15, 0, 1, 0, 100, 1, 5000, 5000, 0, 0, 145, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Time = 5 seconds (OOC) - Self: Disable Immune to NPC'),
(@ENTRY, 0, 16, 0, 54, 0, 100, 0, 0, 0, 0, 0, 144, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Enable Immune to PC'),
(@ENTRY, 0, 17, 0, 54, 0, 100, 0, 0, 0, 0, 0, 145, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Enable Immune to NPC'),
(@ENTRY, 0, 18, 0, 31, 0, 100, 0, 61170, 0, 2000, 2000, 11, 57635, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'On spell Flash Freeze (61170)  hit a target (wait 2000 - 2000 ms before next event trigger) - Self: Cast spell  Disengage (57635) on Victim');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 31242 AND `SourceId` = 0;
