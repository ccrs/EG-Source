--
DELETE FROM `creature_template_movement` WHERE `CreatureId` = 32193;
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`, `InteractionPauseTimer`) VALUES
(32193, NULL, NULL, 1, NULL, NULL, NULL, NULL);
