--
DELETE FROM `creature_template_movement` WHERE `CreatureId`= 26858;
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`, `InteractionPauseTimer`) VALUES
(26858, 1, 0, 1, 0, 0, 0, 0);

 -- Sarathstra smart ai
SET @ENTRY := 26858;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 54, 0, 100, 0, 0, 0, 0, 0, 60, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Disable gravity'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 53, 1, 26858, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Start path #26858, run, do not repeat, Passive'),
(@ENTRY, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Set event phase to phase 1'),
(@ENTRY, 0, 3, 0, 40, 0, 100, 0, 3, 26858, 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On wapoint 3 of path 26858 reached - Self: Enable gravity'),
(@ENTRY, 0, 4, 0, 1, 1, 100, 1, 240000, 240000, 0, 0, 41, 0, 10, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Time = 240 seconds (OOC) - Self: Despawn instantly respawn in 10 seconds');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 26858 AND `SourceId` = 0;

-- WP
DELETE FROM `waypoint_data` WHERE `id`=26858;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_type`, `action`, `action_chance`, `wpguid`) VALUES
(26858, 1, 4400.307, 913.1443, 141.6648, NULL, 0, 0, 0, 100, 0),
(26858, 2, 4400.307, 913.1443, 86.03818, NULL, 0, 0, 0, 100, 0),
(26858, 3, 4396.26,  917.477,  83.1363,  NULL, 0, 0, 0, 100, 0),
(26858, 4, 4384.42,  931.647,  81.0272,  NULL, 0, 0, 0, 100, 0);

SET @GOSSIP=9434;

-- Creature Text
DELETE FROM `creature_text` WHERE `CreatureID`=26859;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(26859, 0, 0, "Don't ya worry, mon. Just leave it to Rokhan. She be comin'.", 12, 0, 100, 1, 0, 0, 26047, 0, 'Rokhan to Player');

-- Gossip
DELETE FROM `gossip_menu_option` WHERE `MenuID`=@GOSSIP AND `OptionID`=0;
INSERT INTO `gossip_menu_option` (`MenuID`, `OptionID`, `OptionIcon`, `OptionText`, `OptionBroadcastTextID`, `OptionType`, `OptionNpcFlag`, `ActionMenuID`, `ActionPoiID`, `BoxCoded`, `BoxMoney`, `BoxText`, `BoxBroadcastTextID`, `VerifiedBuild`) VALUES
(@GOSSIP, 0, 0, "Get that frostwyrm on the ground and I'll finish her.", 26046, 1, 1, 0, 0, 0, 0, '', 0, 0);

-- Rokhan gossip conditions
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=15 AND `SourceGroup`=@GOSSIP AND `SourceEntry`=0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(15, @GOSSIP, 0, 0, 1, 47, 0, 12097, 8, 0, 0, 0, 0, '', "Rokhan gossip menu only show if player has state_mask 8 in Sarathstra, Scourge of the North"),
(15, @GOSSIP, 0, 0, 1, 2, 0, 36793, 1, 0, 1, 0, 0, '', "Rokhan gossip menu only show if the player doesn't have the Sarathstra's Frozen Heart item");

 -- Rokhan smart ai
SET @ENTRY := 26859;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 62, 1, 100, 0, 9434, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 0 from menu 9434 selected - Gossip player: Close gossip'),
(@ENTRY, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 83, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 0 from menu 9434 selected - Self: Remove npc flags GOSSIP'),
(@ENTRY, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 0 from menu 9434 selected - Self: Talk 0 to invoker'),
(@ENTRY, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 12, 26858, 6, 60000, 0, 0, 1, 8, 0, 0, 0, 4412.329, 857.795, 170, 2.28, 'On gossip action 0 from menu 9434 selected - Self: Summon creature Sarathstra (26858) at (4412.329, 857.795, 170, 2.28) as summon type corpse timed despawn with duration 60 seconds'),
(@ENTRY, 0, 4, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 0 from menu 9434 selected - Set event phase to phase 2'),
(@ENTRY, 0, 5, 6, 35, 2, 100, 0, 26858, 0, 0, 0, 81, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature Sarathstra (26858) despawn - Self: Set npc flags GOSSIP'),
(@ENTRY, 0, 6, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On summoned creature Sarathstra (26858) despawn - Set event phase to phase 1'),
(@ENTRY, 0, 7, 0, 11, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Set event phase to phase 1');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 26859 AND `SourceId` = 0;
