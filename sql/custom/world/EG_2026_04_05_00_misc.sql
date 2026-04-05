-- Spellscript Entry
DELETE FROM `spell_script_names` WHERE `ScriptName` = 'spell_zuldrak_vargul_explosion';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(55569, 'spell_zuldrak_vargul_explosion');

 -- Gymer smart ai
SET @ENTRY := 29884;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 1, 54, 0, 100, 512, 0, 0, 0, 0, 86, 55430, 2, 7, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Summoner: Cast spell  Gymer\'s Buddy (55430) with flags triggered at Self'),
(@ENTRY, 0, 1, 0, 61, 0, 100, 512, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Set react state to Passive'),
(@ENTRY, 0, 2, 3, 27, 0, 100, 513, 0, 0, 0, 0, 44, 256, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On passenger boarded (vehicle) - Self: Set phasemask Phase 9'),
(@ENTRY, 0, 3, 4, 61, 0, 100, 512, 0, 0, 0, 0, 1, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On passenger boarded (vehicle) - Self: Talk 8 to invoker'),
(@ENTRY, 0, 4, 0, 61, 0, 100, 512, 0, 0, 0, 0, 11, 55461, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On passenger boarded (vehicle) - Self: Cast spell  Storm\'s Fury (55461) on Self'),
(@ENTRY, 0, 5, 6, 28, 0, 100, 513, 0, 0, 0, 0, 44, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On passenger removed (vehicle) - Self: Set phasemask Phase 1'),
(@ENTRY, 0, 6, 7, 61, 0, 100, 512, 0, 0, 0, 0, 1, 10, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On passenger removed (vehicle) - Self: Talk 10 to invoker'),
(@ENTRY, 0, 7, 0, 61, 0, 100, 512, 0, 0, 0, 0, 41, 5000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On passenger removed (vehicle) - Self: Despawn in 5 s'),
(@ENTRY, 0, 8, 0, 75, 0, 100, 512, 0, 29897, 15, 5000, 11, 55517, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When creature Reanimated Corpse (29897) in range 15 (cooldown 5 sec) - Self: Cast spell  Ghoul Crush (55517) with flags triggered on Self'),
(@ENTRY, 0, 9, 0, 60, 0, 100, 512, 5000, 5000, 5000, 5000, 28, 55517, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 5 seconds - Self: Remove aura due to spell  Ghoul Crush (55517)');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 29884 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(22, 6, 29884, 0, 0, 31, 0, 4, 0, 0, 0, 'Action invoker is player'),
(22, 9, 29884, 0, 0, 1, 1, 55517, 0, 0, 1, 'Object has not aura of spell Ghoul Crush (55517), effect EFFECT_0'),
(22, 10, 29884, 0, 0, 29, 1, 29897, 15, 0, 1, 'There is no creature Reanimated Corpse (29897) within range 15 yards to Object');

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 13) AND (`SourceEntry` IN (55519));
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `Comment`) VALUES 
(13, 1, 55519, 0, 0, 31, 0, 3, 29897, 0, 0, 'Potential target of the spell is creature, entry is Reanimated Corpse (29897)');

UPDATE `creature` SET `spawntimesecs` = 5 WHERE `Id` = 29897 AND `phaseMask` = 256;
