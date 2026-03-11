--
DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (30403);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`) VALUES
(30403, 1, 1, 0, 1);

 -- Nergeld smart ai
SET @ENTRY := 30403;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (3040300, 3040301);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 11, 0, 100, 512, 0, 0, 0, 0, 11, 59037, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Cast spell  The Sum is Greater than the Parts: Nergeld\'s Freeze Anim (59037) on Self'),
(@ENTRY, 0, 1, 0, 27, 0, 100, 512, 0, 0, 0, 0, 80, 3040300, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On passenger boarded (vehicle) - Self: Start timed action list id #Nergeld #0 (3040300) (update always) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 512, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Talk 0 to invoker'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 512, 0, 0, 0, 0, 28, 59037, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Remove aura due to spell  The Sum is Greater than the Parts: Nergeld\'s Freeze Anim (59037)'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 512, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set react state to Passive'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 512, 10000, 10000, 0, 0, 107, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'After 10 seconds - Self: Summon creature group 0'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 512, 20000, 20000, 0, 0, 107, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'After 20 seconds - Self: Summon creature group 1'),
(@ENTRY * 100, 9, 5, 0, 0, 0, 100, 512, 20000, 20000, 0, 0, 107, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'After 20 seconds - Self: Summon creature group 2'),
(@ENTRY * 100, 9, 6, 0, 0, 0, 100, 512, 20000, 20000, 0, 0, 107, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'After 20 seconds - Self: Summon creature group 3'),
(@ENTRY, 0, 2, 0, 28, 0, 100, 512, 0, 0, 0, 0, 80, 3040301, 2, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On passenger removed (vehicle) - Self: Start timed action list id #Nergeld #1 (3040301) (update always) override existing // -inline'),
(@ENTRY * 100 + 1, 9, 0, 0, 0, 0, 100, 512, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Talk 1 to invoker'),
(@ENTRY * 100 + 1, 9, 1, 0, 0, 0, 100, 512, 0, 0, 0, 0, 11, 29266, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Cast spell  Permanent Feign Death (29266) on Self'),
(@ENTRY * 100 + 1, 9, 2, 0, 0, 0, 100, 512, 0, 0, 0, 0, 146, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Set uninteractable'),
(@ENTRY * 100 + 1, 9, 3, 0, 0, 0, 100, 512, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'After 0 seconds - Self: Despawn in 10 s'),
(@ENTRY, 0, 3, 0, 60, 0, 100, 512, 2000, 2000, 2000, 2000, 41, 0, 10, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 2 seconds - Self: Despawn instantly respawn in 10 seconds');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30403 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 4, 30403, 0, 0, 23, 0, 4595, 0, 0, 1, 'Action invoker is not in area Hall of Horrors (4595)');

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 30403;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES
(30403, 56699, 1, 0);

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 18) AND (`SourceGroup` IN (30403));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(18, 30403, 56699, 0, 0, 9, 0, 13043, 0, 0, 0, 'Clicker has quest The Sum is Greater than the Parts (13043) active');

 -- Grimmr Hound smart ai
SET @ENTRY := 30432;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 8, 0, 100, 0, 32067, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  The Sum is Greater than the Parts: Dr. Terrible\'s Death Signal (32067) hit - Self: Despawn instantly'),
(@ENTRY, 0, 1, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly'),
(@ENTRY, 0, 2, 0, 1, 0, 100, 1, 2000, 2000, 0, 0, 49, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Time = 2 seconds (OOC) - Self: Attack Owner/Summoner');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30432 AND `SourceId` = 0;

 -- Vargul Assailant smart ai
SET @ENTRY := 30471;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 8, 0, 100, 0, 32067, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  The Sum is Greater than the Parts: Dr. Terrible\'s Death Signal (32067) hit - Self: Despawn instantly'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 4500, 5599, 13400, 15600, 11, 43410, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 13.4 - 15.6 seconds (4.5 - 5.599s initially) (IC) - Self: Cast spell  Chop (43410) on Victim'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 8000, 9000, 35000, 37000, 11, 23262, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 35 - 37 seconds (8 - 9s initially) (IC) - Self: Cast spell  Demoralize (23262) on Self'),
(@ENTRY, 0, 3, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly'),
(@ENTRY, 0, 4, 0, 1, 0, 100, 1, 2000, 2000, 0, 0, 49, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Time = 2 seconds (OOC) - Self: Attack Owner/Summoner');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30471 AND `SourceId` = 0;

 -- Dr. Terrible smart ai
SET @ENTRY := 30404;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Talk 0 to Attacked unit'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 8000, 8000, 15000, 19000, 11, 61143, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 15 - 19 seconds (8 - 8s initially) (IC) - Self: Cast spell  Crazed Chop (61143) on Victim'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 10000, 12000, 28000, 33000, 11, 61146, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 'Every 28 - 33 seconds (10 - 12s initially) (IC) - Self: Cast spell  Slime Stream (61146) on Random hostile'),
(@ENTRY, 0, 3, 0, 2, 0, 100, 1, 0, 45, 0, 0, 11, 31730, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When health between 0%-45%% (once) - Self: Cast spell  Heal (31730) on Self'),
(@ENTRY, 0, 4, 5, 6, 0, 100, 0, 0, 0, 0, 0, 11, 32067, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Cast spell  The Sum is Greater than the Parts: Dr. Terrible\'s Death Signal (32067) with flags triggered on Self'),
(@ENTRY, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Talk 1 to invoker'),
(@ENTRY, 0, 6, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly'),
(@ENTRY, 0, 7, 0, 1, 0, 100, 1, 2000, 2000, 0, 0, 49, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'Time = 2 seconds (OOC) - Self: Attack Owner/Summoner');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30404 AND `SourceId` = 0;

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (56746));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 56746, 0, 0, 31, 0, 3, 30471, 0, 0, 'Punch can target Vargul Assailant'),
(13, 1, 56746, 0, 1, 31, 0, 3, 30432, 0, 0, 'Punch can target Grimmr Hound'),
(13, 1, 56746, 0, 2, 31, 0, 3, 30404, 0, 0, 'Punch can target Dr. Terrible');
