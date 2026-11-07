--
DELETE FROM `spell_script_names` WHERE `spell_id` IN (62323, 62336) AND `ScriptName` IN ('EG_spell_flame_leviathan_hookshot', 'EG_spell_flame_leviathan_hookshot_aura');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(62323, 'EG_spell_flame_leviathan_hookshot'),
(62336, 'EG_spell_flame_leviathan_hookshot_aura');

UPDATE `creature_template` SET `unit_flags` = `unit_flags` | 32768 WHERE `entry` IN (3654, 5762, 5763);
