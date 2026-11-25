--
DELETE FROM `spell_script_names` WHERE `spell_id` IN (37920, 38006, 38055, 38488) AND `ScriptName` = 'spell_gen_charmed_unit_spell_cooldown';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(37920, 'spell_gen_charmed_unit_spell_cooldown'),
(38006, 'spell_gen_charmed_unit_spell_cooldown'),
(38055, 'spell_gen_charmed_unit_spell_cooldown'),
(38488, 'spell_gen_charmed_unit_spell_cooldown');
