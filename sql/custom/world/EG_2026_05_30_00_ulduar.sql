--
UPDATE `creature_template` SET `flags_extra` = 8256 WHERE `entry` = 34001;

UPDATE `creature_template` SET `faction` = 2105 WHERE `entry` IN (33060, 33062, 33067, 33109, 33167, 34045);

-- Iron Construct
UPDATE `creature_template` SET `unit_flags` = 33619968, `mechanic_immune_mask` = 617299839, `HealthModifier` = 86, `DamageModifier` = 13 WHERE `entry` = 33121;
UPDATE `creature_template` SET `unit_flags` = 33619968, `mechanic_immune_mask` = 617299839, `HealthModifier` = 300, `DamageModifier` = 22 WHERE `entry` = 33191;

-- Restore 4 RX-214 Repair-o-matic Station spawns
UPDATE `gameobject` SET `position_x` = 130.55328 WHERE `guid` = 55106;
UPDATE `gameobject` SET `position_x` = 131.89485 WHERE `guid` = 55153;
UPDATE `gameobject` SET `position_x` = 138.5165 WHERE `guid` = 55126;
UPDATE `gameobject` SET `position_x` = 139.85677 WHERE `guid` = 55193;
