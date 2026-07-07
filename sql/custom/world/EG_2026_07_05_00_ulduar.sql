--
DELETE FROM `spell_script_names` WHERE `spell_id` = 62221;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(62221, 'EG_spell_brightleaf_unstable_sun_beam_forced');

UPDATE `creature_template` SET `flags_extra` = 8384 WHERE `entry` = 33140;
UPDATE `creature_template` SET `flags_extra` = 8384 WHERE `entry` = 33141;

UPDATE `creature_template` SET `flags_extra` = 8256 WHERE `entry` = 34143;
