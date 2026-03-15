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
