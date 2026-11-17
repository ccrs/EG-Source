--
DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (10619, 10620) AND `type` = 12;
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES
(10619, 12, 0, 0, ''),
(10620, 12, 1, 0, '');
