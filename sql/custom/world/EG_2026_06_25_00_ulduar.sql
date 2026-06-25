--
UPDATE `creature_template` SET `ScriptName` = 'EG_npc_kologarn_focused_eyebeam' WHERE `entry` IN (33632, 33802);

DELETE FROM `spell_script_names` WHERE `ScriptName` = 'EG_spell_kologarn_focused_eyebeam_spawn';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(63343, 'EG_spell_kologarn_focused_eyebeam_spawn'),
(63701, 'EG_spell_kologarn_focused_eyebeam_spawn');

UPDATE `creature_template` SET `ScriptName` = 'EG_npc_kologarn_arm' WHERE `entry` IN (32933, 32934);
