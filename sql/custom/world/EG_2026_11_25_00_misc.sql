--
UPDATE `gameobject_template` SET `ScriptName` = '' WHERE `entry` = 184926;

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 17 AND `SourceGroup` = 0 AND `SourceEntry` = 37390 AND `SourceId` = 0 AND `ElseGroup` = 0 AND `ConditionTypeOrReference` = 1 AND `ConditionTarget` = 0 AND `ConditionValue1` = 37407 AND `ConditionValue2` = 0 AND `ConditionValue3` = 0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(17, 0, 37390, 0, 0, 1, 0, 37407, 0, 0, 1, 60, 0, '', 'Oscillating Frequency Scanner - caster does not have aura Oscillating Frequency Scanner (another scanner within 25 yards)');

DELETE FROM `spell_area` WHERE `spell` = 32407 AND `area` = 3674 AND `quest_start` = 0 AND `aura_spell` = 0 AND `racemask` = 0 AND `gender` = 2;
