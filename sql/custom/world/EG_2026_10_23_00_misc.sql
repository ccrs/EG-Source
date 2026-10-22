--
DELETE FROM `spell_proc` WHERE `SpellId` IN (54309, 59352);
INSERT INTO `spell_proc` (`SpellId`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `ProcFlags`, `SpellTypeMask`, `SpellPhaseMask`, `HitMask`, `AttributesMask`, `DisableEffectsMask`, `ProcsPerMinute`, `Chance`, `Cooldown`, `Charges`) VALUES
(54309, 0, 0, 0, 0, 0, 34816, 2, 0, 0, 2, 1, 0, 100, 0, 0), -- Mark of Darkness - SpellTypeMask 2: heals only, not every positive spell
(59352, 0, 0, 0, 0, 0, 34816, 2, 0, 0, 2, 1, 0, 100, 0, 0); -- Mark of Darkness (Heroic) - SpellTypeMask 2: heals only, not every positive spell

DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryorguid` = 30643;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(30643, 0, 0, 0, 54, 0, 100, 0, 0, 0, 0, 0, 38, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Set in combat with current zone'),
(30643, 0, 1, 0, 54, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Despawn instantly');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 30643 AND `SourceId` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(22, 1, 30643, 0, 0, 13, 1, 0, 1, 2, 0, 0, 0, '', 'Lava Blaze - Set in combat with zone only while Sartharion is in progress'),
(22, 2, 30643, 0, 0, 13, 1, 0, 1, 2, 1, 0, 0, '', 'Lava Blaze - Despawn if Sartharion is not in progress');
