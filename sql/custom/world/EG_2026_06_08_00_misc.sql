--
UPDATE `quest_template_addon` SET `PrevQuestID` = 0 WHERE `ID` IN (13108, 13109);
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 19 AND `SourceEntry` IN (13108, 13109);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(19, 0, 13108, 0, 0, 8, 0, 13047, 0, 0, 0, 0, 0, '', 'Whatever it Takes! requires only The Reckoning (13047)'),
(19, 0, 13109, 0, 0, 8, 0, 13047, 0, 0, 0, 0, 0, '', 'Diametrically Opposed requires only The Reckoning (13047)');

DELETE FROM `creature_loot_template` WHERE `Entry`=33293 AND `Item` IN (45442, 45443, 45444, 45445, 45446);
