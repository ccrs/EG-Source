--
UPDATE `creature_model_info` SET `BoundingRadius` = 0.31, `CombatReach` = 20 WHERE `DisplayID` = 28945;

DELETE FROM `spell_area` WHERE `spell` = 63988 AND `area` = 4273;
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`, `quest_start_status`, `quest_end_status`) VALUES
(63988, 4273, 0, 0, 0, 0, 2, 0, 64, 11);

UPDATE `creature_template` SET `flags_extra` = `flags_extra` | 1073741824 WHERE `entry` IN (33433, 33567, 33716, 33717, 33719, 33720, 33943, 33959);
