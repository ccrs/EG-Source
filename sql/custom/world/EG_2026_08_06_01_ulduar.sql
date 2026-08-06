--
DELETE FROM `spell_script_names` WHERE `spell_id` = 62692;
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(62692, 'EG_spell_general_vezax_aura_of_despair');

UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` IN (33524, 34152);
