-- DK avoidance script link
DELETE FROM `spell_script_names` WHERE `ScriptName` = 'spell_dk_avoidance_passive';
INSERT INTO `spell_script_names` (spell_id, ScriptName) VALUES
(62137, 'spell_dk_avoidance_passive');

-- DK ghoul auras
DELETE FROM `creature_template_addon` WHERE entry IN (26125, 24207);
INSERT INTO `creature_template_addon` (entry, auras) VALUES
(26125, '62137'),
(24207, '62137');
