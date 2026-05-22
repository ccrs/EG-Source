DELETE FROM `spell_script_names` WHERE `ScriptName` IN ('EG_spell_Cosmetic___Divine_Shield_Blue', 'EG_spell_cosmetic___divine_shield_blue');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(45775, 'EG_spell_cosmetic___divine_shield_blue');

 -- Inquisitor Salrand smart ai
SET @ENTRY := 25584;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 9 AND `entryOrGuid` IN (2558400);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 38, 0, 100, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Self: Talk 0 to invoker'),
(@ENTRY, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 1 - Set event phase to phase 2'),
(@ENTRY, 0, 2, 3, 38, 0, 100, 0, 1, 2, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Self: Talk 1 to invoker'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 80, 2558400, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On data[1] set to 2 - Self: Start timed action list id #Inquisitor Salrand #0 (2558400) (update out of combat) // -inline'),
(@ENTRY * 100, 9, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 69, 10, 0, 0, 0, 0, 0, 8, 0, 0, 0, 3392.85, 6161.09, 79.8309, 0, 'After 0 seconds - Self: Move to position (3392.85, 6161.09, 79.8309, 0) (point id 10)'),
(@ENTRY, 0, 4, 5, 6, 0, 100, 0, 0, 0, 0, 0, 11, 45809, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Cast spell  Summon Salrand\'s Lockbox (45809) on Self'),
(@ENTRY, 0, 5, 0, 61, 0, 100, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Talk 2 to invoker'),
(@ENTRY, 0, 6, 7, 25, 0, 100, 0, 0, 0, 0, 0, 11, 45775, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Cast spell  Cosmetic - Divine Shield Blue (45775) on Self'),
(@ENTRY, 0, 7, 8, 61, 0, 100, 0, 0, 0, 0, 0, 144, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Enable Immune to PC'),
(@ENTRY, 0, 8, 9, 61, 0, 100, 0, 0, 0, 0, 0, 145, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Enable Immune to NPC'),
(@ENTRY, 0, 9, 10, 61, 0, 100, 0, 0, 0, 0, 0, 60, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Disable gravity'),
(@ENTRY, 0, 10, 0, 61, 0, 100, 0, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On reset - Set event phase to phase 1'),
(@ENTRY, 0, 11, 0, 1, 1, 100, 0, 5000, 5000, 10000, 10000, 11, 45777, 0, 0, 0, 0, 0, 10, 119586, 25594, 0, 0, 0, 0, 0, 'Every 10 seconds (5s initially) (OOC) - Self: Cast spell  Salrand\'s Beam (45777) on Creature Beryl Point InvisMan (25594) with guid 119586 (fetching)'),
(@ENTRY, 0, 12, 13, 34, 0, 100, 0, 8, 10, 0, 0, 60, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type POINT_MOTION_TYPE inform, point 10 - Self: Enable gravity'),
(@ENTRY, 0, 13, 14, 61, 0, 100, 0, 0, 0, 0, 0, 144, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type POINT_MOTION_TYPE inform, point 10 - Self: Disable Immune to PC'),
(@ENTRY, 0, 14, 15, 61, 0, 100, 0, 0, 0, 0, 0, 145, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type POINT_MOTION_TYPE inform, point 10 - Self: Disable Immune to NPC'),
(@ENTRY, 0, 15, 16, 61, 0, 100, 0, 0, 0, 0, 0, 28, 45775, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On movement of type POINT_MOTION_TYPE inform, point 10 - Self: Remove aura due to spell  Cosmetic - Divine Shield Blue (45775)'),
(@ENTRY, 0, 16, 0, 61, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 21, 30, 0, 0, 0, 0, 0, 0, 'On movement of type POINT_MOTION_TYPE inform, point 10 - Self: Attack Closest player in 30 yards'),
(@ENTRY, 0, 17, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 5, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly respawn in 5 seconds');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 25584 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 1, 25584, 0, 0, 1, 1, 45775, 0, 0, 0, 'Object has aura of spell Cosmetic - Divine Shield Blue (45775), effect EFFECT_0'),
(22, 3, 25584, 0, 0, 1, 1, 45775, 0, 0, 0, 'Object has aura of spell Cosmetic - Divine Shield Blue (45775), effect EFFECT_0');

DELETE FROM `creature_template_movement` WHERE `CreatureId`= @ENTRY;
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`, `InteractionPauseTimer`) VALUES
(@ENTRY, 1, 0, 1, 0, 0, 0, 0);
