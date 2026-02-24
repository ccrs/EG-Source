 -- Reanimated Captain smart ai
SET @ENTRY := 30986;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 32674, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On aggro - Self: Cast spell  Avenger\'s Shield (32674) on Attacked unit'),
(@ENTRY, 0, 1, 0, 0, 0, 100, 0, 5000, 5000, 17800, 19800, 11, 58154, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 17.8 - 19.8 seconds (5 - 5s initially) (IC) - Self: Cast spell  Hammer of Injustice (58154) on Victim'),
(@ENTRY, 0, 2, 0, 0, 0, 100, 0, 5000, 15000, 17800, 19800, 11, 58153, 0, 0, 0, 0, 0, 31, 40, 0, 1, 0, 0, 0, 0, 'Every 17.8 - 19.8 seconds (5 - 15s initially) (IC) - Self: Cast spell  Unholy Light (58153) on Lowest HP friendly in 40 yards including self');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30986 AND `SourceId` = 0;

-- Vrykul Harpoon Gun
SET @VHG1 := 27992;
SET @VHG2 := 27993;

DELETE FROM `creature_text` WHERE `creatureid` = @VHG1;
INSERT INTO `creature_text` (`creatureid`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `TextRange`, `comment`, `BroadcastTextId`) VALUES
(@VHG1, 0, 0, 'The longhouse is destroyed! Alarms can be heard in the distance.', 41, 0, 100, 0, 0, 0, 2, 'It Goes to 11...', 23623),
(@VHG1, 1, 0, 'Direct hit on the Dockhouse! Dragonflyer Defenders have been alerted!', 41, 0, 100, 0, 0, 0, 2, 'It Goes to 11...', 23625),
(@VHG1, 2, 0, 'The Vrykul Storage facility is up in flames! Dragonflyer Defenders have been alerted!', 41, 0, 100, 0, 0, 0, 2, 'It Goes to 11...', 23637);

DELETE FROM `creature_text` WHERE `creatureid` = @VHG2;
INSERT INTO `creature_text` (`creatureid`, `groupid`, `id`, `text`, `type`, `language`, `probability`, `emote`, `duration`, `sound`, `TextRange`, `comment`, `BroadcastTextId`) VALUES
(@VHG2, 0, 0, 'The longhouse is destroyed! Alarms can be heard in the distance.', 41, 0, 100, 0, 0, 0, 2, 'It Goes to 11...', 23623),
(@VHG2, 1, 0, 'Direct hit on the Dockhouse! Dragonflyer Defenders have been alerted!', 41, 0, 100, 0, 0, 0, 2, 'It Goes to 11...', 23625),
(@VHG2, 2, 0, 'The Vrykul Storage facility is up in flames! Dragonflyer Defenders have been alerted!', 41, 0, 100, 0, 0, 0, 2, 'It Goes to 11...', 23637);

-- UPDATE `creature` SET `phasemask` = 169 WHERE `guid` IN (1976229, 1976228);

UPDATE `creature_template` SET `RegenHealth` = 0 WHERE `entry` IN (@VHG1, @VHG2);

-- Spells
SET @FH1 := 43986;
SET @FH2 := 43990;
SET @FiL := 43997;

DELETE FROM `creature_template_spell` WHERE `CreatureID` IN (@VHG1, @VHG2);
INSERT INTO `creature_template_spell` (`CreatureID`, `Index`, `Spell`, `VerifiedBuild`) VALUES
(@VHG1, 0, @FH1, 0),
(@VHG1, 1, @FiL, 0),
(@VHG2, 0, @FH1, 0),
(@VHG2, 1, @FiL, 0);

SET @DI1 := 24538;
SET @DI2 := 24646;
SET @DI3 := 24647;
SET @VHC := 50331;

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`= 13 AND `SourceEntry` IN (@FH1, @FH2, @FiL, @VHC);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`SourceId`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionTarget`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`NegativeCondition`,`ErrorType`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(13, 1, @FH1, 0, 0, 31, 0, 3, @DI1, 0, 0, 0, 0, '', 'Fiery Harpoon 1 targets Quest Trigger'),
(13, 1, @FH1, 0, 1, 31, 0, 3, @DI2, 0, 0, 0, 0, '', 'Fiery Harpoon 1 targets Quest Trigger'),
(13, 1, @FH1, 0, 2, 31, 0, 3, @DI3, 0, 0, 0, 0, '', 'Fiery Harpoon 1 targets Quest Trigger'),
(13, 1, @FH2, 0, 0, 31, 0, 3, @DI1, 0, 0, 0, 0, '', 'Fiery Harpoon 2 targets Quest Trigger'),
(13, 1, @FH2, 0, 1, 31, 0, 3, @DI2, 0, 0, 0, 0, '', 'Fiery Harpoon 2 targets Quest Trigger'),
(13, 1, @FH2, 0, 2, 31, 0, 3, @DI3, 0, 0, 0, 0, '', 'Fiery Harpoon 2 targets Quest Trigger'),
(13, 1, @VHC, 0, 0, 31, 0, 3, @VHG1, 0, 0, 0, 0, '', 'Vrykul Harpoon Control targets Valkyr Harpoon Gun'),
(13, 1, @VHC, 0, 1, 31, 0, 3, @VHG2, 0, 0, 0, 0, '', 'Vrykul Harpoon Control targets Valkyr Harpoon Gun');

DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (@VHG1, @VHG2);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`) VALUES
(@VHG1, 0, 0, 0, 1),
(@VHG2, 0, 0, 0, 1);

 -- Vrykul Harpoon Gun smart ai
SET @ENTRY := 27992;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2799200);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 25, 0, 100, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Disable auto attack'),
(@ENTRY, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Disable combat based movement'),
(@ENTRY, 0, 2, 3, 38, 0, 100, 512, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 512, 0, 0, 0, 0, 67, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Trigger timed event timedEvent[1] in 0 - 0 ms'),
(@ENTRY, 0, 4, 5, 38, 0, 100, 512, 1, 2, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Self: Talk 1 to invoker'),
(@ENTRY, 0, 5, 0, 61, 0, 100, 512, 0, 0, 0, 0, 67, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Trigger timed event timedEvent[1] in 0 - 0 ms'),
(@ENTRY, 0, 6, 7, 38, 0, 100, 512, 1, 3, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Self: Talk 2 to invoker'),
(@ENTRY, 0, 7, 0, 61, 0, 100, 512, 0, 0, 0, 0, 67, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Trigger timed event timedEvent[1] in 0 - 0 ms'),
(@ENTRY, 0, 8, 0, 59, 0, 100, 512, 1, 0, 0, 0, 80, 2799200, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On timed event timedEvent[1] triggered - Self: Start timed action list id #Vrykul Harpoon Gun #0 (2799200) (update always) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 970, -5250, 195, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (970, -5250, 195, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 978, -5272, 204, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (978, -5272, 204, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 956, -5267, 198, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (956, -5267, 198, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 965, -5241, 189, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (965, -5241, 189, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 948, -5247, 200, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (948, -5247, 200, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 5, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 920, -5251, 193, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (920, -5251, 193, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 6, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 928, -5269, 206, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (928, -5269, 206, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 7, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 906, -5263, 196, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (906, -5263, 196, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 8, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 915, -5238, 187, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (915, -5238, 187, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 9, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 898, -5245, 202, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (898, -5245, 202, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY, 0, 9, 0, 31, 0, 100, 512, 43997, 0, 0, 0, 11, 43998, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell Fiery Lance (43997)  hit a target - Self: Cast spell  Fiery Lance (43998) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 27992 AND `SourceId` = 0;

 -- Vrykul Harpoon Gun smart ai
SET @ENTRY := 27993;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2799300);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 25, 0, 100, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Disable auto attack'),
(@ENTRY, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Disable combat based movement'),
(@ENTRY, 0, 2, 3, 38, 0, 100, 512, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 512, 0, 0, 0, 0, 67, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Trigger timed event timedEvent[1] in 0 - 0 ms'),
(@ENTRY, 0, 4, 5, 38, 0, 100, 512, 1, 2, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Self: Talk 1 to invoker'),
(@ENTRY, 0, 5, 0, 61, 0, 100, 512, 0, 0, 0, 0, 67, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Trigger timed event timedEvent[1] in 0 - 0 ms'),
(@ENTRY, 0, 6, 7, 38, 0, 100, 512, 1, 3, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Self: Talk 2 to invoker'),
(@ENTRY, 0, 7, 0, 61, 0, 100, 512, 0, 0, 0, 0, 67, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 3 - Trigger timed event timedEvent[1] in 0 - 0 ms'),
(@ENTRY, 0, 8, 0, 59, 0, 100, 512, 1, 0, 0, 0, 80, 2799300, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On timed event timedEvent[1] triggered - Self: Start timed action list id #Vrykul Harpoon Gun #0 (2799300) (update always) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 970, -5250, 195, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (970, -5250, 195, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 1, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 978, -5272, 204, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (978, -5272, 204, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 2, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 956, -5267, 198, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (956, -5267, 198, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 3, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 965, -5241, 189, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (965, -5241, 189, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 4, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 948, -5247, 200, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (948, -5247, 200, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 5, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 920, -5251, 193, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (920, -5251, 193, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 6, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 928, -5269, 206, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (928, -5269, 206, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 7, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 906, -5263, 196, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (906, -5263, 196, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 8, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 915, -5238, 187, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (915, -5238, 187, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY * 100, 9, 9, 0, 0, 0, 100, 512, 3000, 4000, 0, 0, 12, 24533, 4, 60000, 0, 0, 0, 8, 0, 0, 0, 898, -5245, 202, 4.5, 'After 4 seconds - Self: Summon creature Dragonflayer Defender (24533) at (898, -5245, 202, 4.5) as summon type timed despawn out of combat with duration 60 seconds'),
(@ENTRY, 0, 9, 0, 31, 0, 100, 512, 43997, 0, 0, 0, 11, 43998, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell Fiery Lance (43997)  hit a target - Self: Cast spell  Fiery Lance (43998) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 27993 AND `SourceId` = 0;

 -- Dragonflayer Defender smart ai
SET @ENTRY := 24533;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 60, 0, 100, 0, 1000, 1000, 1500, 2500, 11, 44188, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 1.5 - 2.5 seconds (1 - 1s initially) - Self: Cast spell  Harpoon Toss (44188) on Victim'),
(@ENTRY, 0, 1, 2, 8, 0, 100, 0, 43997, 0, 0, 0, 134, 43998, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Fiery Lance (43997) hit - Caster: Cast spell Fiery Lance (43998) on Self'),
(@ENTRY, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 37, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Fiery Lance (43997) hit - Self: Die'),
(@ENTRY, 0, 3, 4, 54, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Attack Owner/Summoner'),
(@ENTRY, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 79, 30, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Chase its victim in 30 yards (angle 0)'),
(@ENTRY, 0, 5, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 24533 AND `SourceId` = 0;

-- DELETE FROM `creature_template_addon` WHERE `entry` = 24533;
-- INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
-- (24533, 0, 22657, 50331648, 1, 0, '');

DELETE FROM `creature_template_movement` WHERE `CreatureId` = 24533;
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`) VALUES
(24533, 0, 0, 1, 0);

 -- Dragonflayer Installation I smart ai
SET @ENTRY := 24538;
UPDATE `creature_template` SET `flags_extra`= 130, `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 8, 0, 100, 0, 43990, 0, 0, 0, 134, 44092, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Fiery Harpoon (43990) hit - Caster: Cast spell Vrykul Harpoon QC (44092) on Self'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 1, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On spell  Fiery Harpoon (43990) hit - Caster: Set creature data #1 to 1'),
(@ENTRY, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 57931, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Fiery Harpoon (43990) hit - Self: Cast spell  Cosmetic - Low Poly Fire (57931) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 24538 AND `SourceId` = 0;

 -- Dragonflayer Installation II smart ai
SET @ENTRY := 24646;
UPDATE `creature_template` SET `flags_extra`= 130, `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 8, 0, 100, 0, 43990, 0, 0, 0, 134, 44128, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Fiery Harpoon (43990) hit - Caster: Cast spell Vrykul Harpoon QC (44128) on Self'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 2, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On spell  Fiery Harpoon (43990) hit - Caster: Set creature data #1 to 2'),
(@ENTRY, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 57931, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Fiery Harpoon (43990) hit - Self: Cast spell  Cosmetic - Low Poly Fire (57931) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 24646 AND `SourceId` = 0;

 -- Dragonflayer Installation III smart ai
SET @ENTRY := 24647;
UPDATE `creature_template` SET `flags_extra`= 130, `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 8, 0, 100, 0, 43990, 0, 0, 0, 134, 44129, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Fiery Harpoon (43990) hit - Caster: Cast spell Vrykul Harpoon QC (44129) on Self'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 45, 1, 3, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On spell  Fiery Harpoon (43990) hit - Caster: Set creature data #1 to 3'),
(@ENTRY, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 11, 57931, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Fiery Harpoon (43990) hit - Self: Cast spell  Cosmetic - Low Poly Fire (57931) on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 24647 AND `SourceId` = 0;

-- Spellscript Entry
DELETE FROM `spell_script_names` WHERE `ScriptName` = 'EG_spell_fiery_lance';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(43997, 'EG_spell_fiery_lance');

DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (24418);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`) VALUES
(24418, 0, 0, 1, 0);

 -- Steel Gate Flying Machine smart ai
SET @ENTRY := 24418;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 60, 0, 100, 512, 5000, 5000, 5000, 5000, 41, 0, 10, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 5 seconds - Self: Despawn instantly respawn in 10 seconds'),
(@ENTRY, 0, 1, 0, 28, 0, 100, 512, 0, 0, 0, 0, 41, 0, 10, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On passenger removed (vehicle) - Self: Despawn instantly respawn in 10 seconds');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 24418 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 1, 24418, 0, 0, 23, 0, 3999, 0, 0, 1, 'Action invoker is not in area Steel Gate (3999)');

UPDATE `creature_template_addon` SET `auras` = '43775 43889 60921' WHERE `entry` = 24418;
