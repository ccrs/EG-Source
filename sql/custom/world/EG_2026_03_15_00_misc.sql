--
DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (3678, 3679, 3636, 5048, 5762, 5763, 3654);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`) VALUES
(3678, 1, 1, 0, 0),
(3679, 1, 1, 1, 0),
(3636, 1, 1, 0, 0),
(5048, 1, 1, 0, 0),
(5762, 1, 1, 0, 0),
(5763, 1, 1, 0, 0),
(3654, 1, 1, 0, 0);

 -- Bjorn Halgurdsson smart ai
SET @ENTRY := 24238;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = '' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 1, 0, 100, 0, 10000, 15000, 45000, 60000, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Every 45 - 60 seconds (10 - 15s initially) (OOC) - Self: Talk 2 to invoker'),
(@ENTRY, 0, 1, 2, 8, 0, 100, 1, 43315, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On spell  Vrykul Insult (43315) hit - Caster: Talk 0'),
(@ENTRY, 0, 2, 3, 61, 0, 100, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Vrykul Insult (43315) hit - Self: Talk 1 to invoker'),
(@ENTRY, 0, 3, 4, 61, 0, 100, 0, 0, 0, 0, 0, 8, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Vrykul Insult (43315) hit - Self: Set react state to Aggressive'),
(@ENTRY, 0, 4, 5, 61, 0, 100, 0, 0, 0, 0, 0, 144, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Vrykul Insult (43315) hit - Self: Disable Immune to PC'),
(@ENTRY, 0, 5, 6, 61, 0, 100, 0, 0, 0, 0, 0, 145, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Vrykul Insult (43315) hit - Self: Disable Immune to NPC'),
(@ENTRY, 0, 6, 7, 61, 0, 100, 0, 0, 0, 0, 0, 159, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On spell  Vrykul Insult (43315) hit - Self: Set attackable'),
(@ENTRY, 0, 7, 0, 61, 0, 100, 0, 0, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On spell  Vrykul Insult (43315) hit - Self: Attack Caster'),
(@ENTRY, 0, 8, 9, 9, 0, 100, 1, 0, 10, 0, 0, 43, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 10 yards (once) - Self: Dismount'),
(@ENTRY, 0, 9, 0, 61, 0, 100, 0, 0, 0, 0, 0, 60, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'When victim in range 0 - 10 yards (once) - Self: Enable gravity'),
(@ENTRY, 0, 10, 0, 8, 0, 100, 0, 6660, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On spell  Shoot (6660) hit - Self: Attack Caster'),
(@ENTRY, 0, 11, 0, 8, 0, 100, 0, 11971, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On spell  Sunder Armor (11971) hit - Self: Attack Caster'),
(@ENTRY, 0, 12, 0, 8, 0, 100, 0, 18802, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On spell  Frost Shot (18802) hit - Self: Attack Caster'),
(@ENTRY, 0, 13, 0, 8, 0, 100, 0, 43895, 0, 0, 0, 49, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On spell  Lydell\'s Poison (43895) hit - Self: Attack Caster'),
(@ENTRY, 0, 14, 0, 6, 0, 100, 0, 0, 0, 0, 0, 11, 43371, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On death - Self: Cast spell  Bjorn Kill Credit (43371) on Self'),
(@ENTRY, 0, 15, 0, 0, 0, 100, 0, 5000, 10000, 10000, 15000, 11, 32736, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 10 - 15 seconds (5 - 10s initially) (IC) - Self: Cast spell  Mortal Strike (32736) on Victim'),
(@ENTRY, 0, 16, 0, 0, 0, 100, 0, 0, 5000, 10000, 15000, 11, 33661, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Every 10 - 15 seconds (0 - 5s initially) (IC) - Self: Cast spell  Crush Armor (33661) on Victim'),
(@ENTRY, 0, 17, 18, 11, 0, 100, 0, 0, 0, 0, 0, 43, 0, 22657, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Mount to model 22657'),
(@ENTRY, 0, 18, 19, 61, 0, 100, 0, 0, 0, 0, 0, 144, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Enable Immune to PC'),
(@ENTRY, 0, 19, 20, 61, 0, 100, 0, 0, 0, 0, 0, 145, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Enable Immune to NPC'),
(@ENTRY, 0, 20, 21, 61, 0, 100, 0, 0, 0, 0, 0, 159, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Set not attackable'),
(@ENTRY, 0, 21, 0, 61, 0, 100, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On respawn - Self: Set react state to Passive'),
(@ENTRY, 0, 22, 0, 7, 0, 100, 0, 0, 0, 0, 0, 41, 0, 15, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On evade - Self: Despawn instantly respawn in 15 seconds');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 24238 AND `SourceId` = 0;

UPDATE `creature_template` SET `speed_run` = 1.14286 WHERE `entry` = 24238;
