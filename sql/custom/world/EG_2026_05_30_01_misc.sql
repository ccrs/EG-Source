--
UPDATE `creature_template` SET `flags_extra` = 8192 WHERE `entry` = 30234;
UPDATE `creature_template` SET `flags_extra` = 8192 WHERE `entry` = 31748;

DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (30234,31748);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`) VALUES
(30234, 1, 0, 1, 0, 0, 0),
(31748, 1, 0, 1, 0, 0, 0);
