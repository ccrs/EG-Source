--
DELETE FROM `creature_template_spell` WHERE `CreatureID` IN (33062, 34045) AND `Spell` IN (67372, 67393);
INSERT INTO `creature_template_spell` (`CreatureID`, `Index`, `Spell`, `VerifiedBuild`) VALUES
(33062, 4, 67372, 0),
(33062, 5, 67393, 0),
(34045, 4, 67372, 0),
(34045, 5, 67393, 0);
