 -- Mine Car smart ai
SET @ENTRY := 28817;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 3, 0, 25703, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Morph to model 25703'),
(@ENTRY, 0, 1, 0, 27, 0, 100, 0, 0, 0, 0, 0, 11, 52464, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On passenger boarded (vehicle) - Self: Cast spell  52464 on Self'),
(@ENTRY, 0, 2, 3, 8, 0, 100, 1, 52595, 0, 0, 0, 59, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell  52595 hit - Self: Set run'),
(@ENTRY, 0, 3, 0, 61, 0, 100, 0, 0, 0, 0, 0, 29, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On spell  52595 hit - Self: Follow Caster by distance 0, angle 0'),
(@ENTRY, 0, 4, 0, 28, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 19, 28841, 9, 0, 0, 0, 0, 0, 'On passenger removed (vehicle) - Closest alive creature Scarlet Miner (28841) in 9 yards: Despawn instantly');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 28817 AND `SourceId` = 0;

DELETE FROM `creature_template_movement` WHERE `CreatureId`= @ENTRY;
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`, `InteractionPauseTimer`) VALUES
(@ENTRY, 1, 0, 0, 0, 0, 0, 0);
