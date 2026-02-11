 -- Tur Ragepaw smart ai
SET @ENTRY := 27328;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2732803);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 11, 0, 100, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Reset faction'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 2, 0, 0, 0, 0, 9, 24921, 2, 20, 0, 0, 0, 0, 'On respawn - Creature Cosmetic Trigger - LAB (24921) in 2 - 20 yards: Set creature data #0 to 2'),
(@ENTRY, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 90, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Set stand state to KNEEL'),
(@ENTRY, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 82, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Add npc flags GOSSIP'),
(@ENTRY, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 8, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Set react state to Defensive'),
(@ENTRY, 0, 5, 6, 62, 0, 100, 0, 9497, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 0 from menu 9497 selected - Set event phase to phase 1'),
(@ENTRY, 0, 6, 7, 61, 0, 100, 0, 0, 0, 0, 0, 80, 2732800, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 0 from menu 9497 selected - Self: Start timed action list id #Tur Ragepaw #0 (2732800) (update out of combat)'),
(@ENTRY, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 67, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 0 from menu 9497 selected - Trigger timed event timedEvent[1] in 0 - 0 ms'),
(@ENTRY, 0, 8, 9, 62, 0, 100, 0, 9497, 1, 0, 0, 22, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 1 from menu 9497 selected - Set event phase to phase 2'),
(@ENTRY, 0, 9, 10, 61, 0, 100, 0, 0, 0, 0, 0, 80, 2732801, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 1 from menu 9497 selected - Self: Start timed action list id #Tur Ragepaw #1 (2732801) (update out of combat)'),
(@ENTRY, 0, 10, 0, 61, 0, 100, 0, 0, 0, 0, 0, 67, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 1 from menu 9497 selected - Trigger timed event timedEvent[1] in 0 - 0 ms'),
(@ENTRY, 0, 11, 12, 62, 0, 100, 0, 9497, 2, 0, 0, 22, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 2 from menu 9497 selected - Set event phase to phase 3'),
(@ENTRY, 0, 12, 13, 61, 0, 100, 0, 0, 0, 0, 0, 80, 2732802, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 2 from menu 9497 selected - Self: Start timed action list id #Tur Ragepaw #2 (2732802) (update out of combat)'),
(@ENTRY, 0, 13, 0, 61, 0, 100, 0, 0, 0, 0, 0, 67, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 2 from menu 9497 selected - Trigger timed event timedEvent[1] in 0 - 0 ms'),
(@ENTRY, 0, 14, 15, 59, 0, 100, 0, 1, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 17, 0, 10, 0, 0, 0, 0, 0, 'On timed event timedEvent[1] triggered - Players in 0 - 10 yards: Close gossip'),
(@ENTRY, 0, 15, 16, 61, 0, 100, 0, 0, 0, 0, 0, 83, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On timed event timedEvent[1] triggered - Self: Remove npc flags GOSSIP'),
(@ENTRY, 0, 16, 0, 61, 0, 100, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 0, 0, 9, 24921, 2, 20, 0, 0, 0, 0, 'On timed event timedEvent[1] triggered - Creature Cosmetic Trigger - LAB (24921) in 2 - 20 yards: Set creature data #0 to 1'),
(@ENTRY, 0, 17, 0, 40, 0, 100, 0, 9, 218625, 0, 0, 80, 2732803, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 9 of path 218625 reached - Self: Start timed action list id #Tur Ragepaw #3 (2732803) (update out of combat) // -inline'),
(@ENTRY * 100 + 3, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 101, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set home position to current position'),
(@ENTRY * 100 + 3, 9, 1, 0, 0, 0, 100, 0, 6000, 6000, 0, 0, 1, 3, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 6 seconds - Self: Talk 3 to invoker'),
(@ENTRY * 100 + 3, 9, 2, 0, 0, 0, 100, 0, 300, 300, 0, 0, 12, 26633, 6, 60000, 0, 0, 0, 8, 0, 0, 0, 4893.27, -3842.42, 337.648, 3.12414, 'After 0.3 seconds - Self: Summon creature Ursoc (26633) at (4893.27, -3842.42, 337.648, 3.12414) as summon type corpse timed despawn with duration 60 seconds'),
(@ENTRY, 0, 18, 19, 21, 1, 100, 0, 0, 0, 0, 0, 28, 52507, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On home reached - Self: Remove aura due to spell  Ragepaw\'s Presence (52507)'),
(@ENTRY, 0, 19, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 48368, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On home reached - Self: Cast spell  Bear Form (48368) on Self'),
(@ENTRY, 0, 20, 0, 4, 1, 100, 0, 0, 0, 0, 0, 11, 52507, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Cast spell  Ragepaw\'s Presence (52507) on Self'),
(@ENTRY, 0, 21, 0, 9, 1, 100, 0, 0, 5, 2500, 4000, 11, 52504, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 5 yards (cooldown 2500 - 4000 ms) - Self: Cast spell  Lacerate (52504) on Victim'),
(@ENTRY, 0, 22, 0, 9, 1, 100, 0, 0, 5, 6000, 8000, 11, 52506, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 5 yards (cooldown 6000 - 8000 ms) - Self: Cast spell  Maul (52506) on Victim'),
(@ENTRY, 0, 23, 0, 0, 1, 100, 0, 5000, 9000, 20000, 20000, 11, 6795, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 20 - 20 seconds (5 - 9s initially) (IC) - Self: Cast spell  Growl (6795) on Victim'),
(@ENTRY, 0, 24, 0, 21, 2, 100, 0, 0, 0, 0, 0, 11, 48369, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On home reached - Self: Cast spell  Moonkin Form (48369) on Self'),
(@ENTRY, 0, 25, 0, 9, 2, 100, 0, 0, 40, 2500, 4000, 11, 52501, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 40 yards (cooldown 2500 - 4000 ms) - Self: Cast spell  Wrath (52501) with flags combat move on Victim'),
(@ENTRY, 0, 26, 0, 9, 2, 100, 0, 0, 40, 13000, 14000, 11, 52502, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 40 yards (cooldown 13000 - 14000 ms) - Self: Cast spell  Moonfire (52502) on Victim'),
(@ENTRY, 0, 27, 28, 21, 4, 100, 0, 0, 0, 0, 0, 11, 48371, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On home reached - Self: Cast spell  Tree of Life (48371) on Self'),
(@ENTRY, 0, 28, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 52553, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On home reached - Self: Cast spell  Empowered Tree of Life (52553) on Self'),
(@ENTRY, 0, 29, 0, 0, 4, 100, 1, 2000, 2000, 0, 0, 79, 15, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Time = 2 seconds (IC) - Self: Chase its victim in 15 yards (angle 0)'),
(@ENTRY, 0, 30, 0, 0, 4, 100, 0, 0, 1000, 1500, 1500, 11, 52554, 0, 0, 0, 0, 0, 31, 40, 1, 0, 0, 0, 0, 0, 'Every 1.5 - 1.5 seconds (0 - 1s initially) (IC) - Self: Cast spell  Nourish (52554) on Lowest HP friendly player in 40 yards'),
(@ENTRY, 0, 31, 0, 0, 4, 100, 0, 0, 1000, 1000, 2000, 11, 52551, 0, 0, 0, 0, 0, 31, 40, 1, 1, 0, 0, 0, 0, 'Every 1 - 2 seconds (0 - 1s initially) (IC) - Self: Cast spell  Lifebloom (52551) on Lowest HP friendly player in 40 yards including self'),
(@ENTRY, 0, 32, 0, 75, 4, 100, 0, 0, 26633, 40, 2000, 49, 0, 0, 0, 0, 0, 0, 7, 26633, 0, 0, 0, 0, 0, 0, 'When creature Ursoc (26633) in range 40 (cooldown 2 sec) - Self: Attack Found creature'),
(@ENTRY, 0, 33, 0, 38, 0, 100, 0, 0, 1, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[0] set to 1 - Self: Despawn instantly');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 27328 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 33, 27328, 0, 0, 21, 0, 32, 0, 0, 0, 'Action invoker has unit state UNIT_STATE_CHASE'),
(22, 33, 27328, 0, 0, 36, 1, 0, 0, 0, 0, 'Object is alive'),
(22, 33, 27328, 0, 0, 21, 1, 32, 0, 0, 1, 'Object doesn\'t have unit state UNIT_STATE_CHASE');

 -- Timed list 2732800 smart ai
SET @ENTRY := 2732800;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 48368, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Cast spell  Bear Form (48368) on Self'),
(@ENTRY, 9, 1, 0, 0, 0, 100, 0, 300, 300, 0, 0, 2, 250, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0.3 seconds - Self: Set faction to Escortee (250)'),
(@ENTRY, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 91, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Remove stand state KNEEL'),
(@ENTRY, 9, 3, 0, 0, 0, 100, 0, 2500, 2500, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'After 2.5 seconds - Last action invoker: Talk 0 to invoker'),
(@ENTRY, 9, 4, 0, 0, 0, 100, 0, 4700, 4700, 0, 0, 53, 1, 218625, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 4.7 seconds - Self: Start path #218625, run, do not repeat, Passive'),
(@ENTRY, 9, 5, 0, 0, 0, 100, 0, 0, 0, 0, 0, 8, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set react state to Aggressive');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 2732800 AND `SourceId` = 9;

 -- Timed list 2732801 smart ai
SET @ENTRY := 2732801;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 48369, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Cast spell  Moonkin Form (48369) on Self'),
(@ENTRY, 9, 1, 0, 0, 0, 100, 0, 300, 300, 0, 0, 2, 250, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0.3 seconds - Self: Set faction to Escortee (250)'),
(@ENTRY, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 91, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Remove stand state KNEEL'),
(@ENTRY, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 24905, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Cast spell  Moonkin Form (Passive) (24905) on Self'),
(@ENTRY, 9, 4, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 24907, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Cast spell  Moonkin Aura (24907) on Self'),
(@ENTRY, 9, 5, 0, 0, 0, 100, 0, 400, 400, 0, 0, 11, 52503, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0.4 seconds - Self: Cast spell  Empowered Moonkin Aura (52503) on Self'),
(@ENTRY, 9, 6, 0, 0, 0, 100, 0, 2500, 2500, 0, 0, 1, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'After 2.5 seconds - Last action invoker: Talk 1 to invoker'),
(@ENTRY, 9, 7, 0, 0, 0, 100, 0, 4700, 4700, 0, 0, 53, 1, 218625, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 4.7 seconds - Self: Start path #218625, run, do not repeat, Passive'),
(@ENTRY, 9, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 8, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set react state to Aggressive');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 2732801 AND `SourceId` = 9;

 -- Timed list 2732802 smart ai
SET @ENTRY := 2732802;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 11, 48371, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Cast spell  Tree of Life (48371) on Self'),
(@ENTRY, 9, 1, 0, 0, 0, 100, 0, 100, 100, 0, 0, 11, 52553, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0.1 seconds - Self: Cast spell  Empowered Tree of Life (52553) on Self'),
(@ENTRY, 9, 2, 0, 0, 0, 100, 0, 0, 0, 0, 0, 2, 113, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set faction to Escortee (113)'),
(@ENTRY, 9, 3, 0, 0, 0, 100, 0, 0, 0, 0, 0, 91, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Remove stand state KNEEL'),
(@ENTRY, 9, 4, 0, 0, 0, 100, 0, 2600, 2600, 0, 0, 1, 2, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'After 2.6 seconds - Last action invoker: Talk 2 to invoker'),
(@ENTRY, 9, 5, 0, 0, 0, 100, 0, 4700, 4700, 0, 0, 53, 1, 218625, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 4.7 seconds - Self: Start path #218625, run, do not repeat, Passive'),
(@ENTRY, 9, 6, 0, 0, 0, 100, 0, 0, 0, 0, 0, 8, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set react state to Aggressive');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 2732802 AND `SourceId` = 9;
