--
DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (30403);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`) VALUES
(30403, 1, 1, 0, 0);
