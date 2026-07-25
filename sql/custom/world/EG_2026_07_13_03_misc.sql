--
DELETE FROM `creature_template_addon` WHERE `entry` = 27829; -- Ebon Gargoyle
INSERT INTO `creature_template_addon` (`entry`, `auras`) VALUES
(27829, '62137'); -- Ebon Gargoyle: Avoidance (Night of the Dead)

UPDATE `creature_template_addon` SET `auras` = '28305 32233' WHERE `entry` = 19668; -- Shadowfiend: Mana Leech + Avoidance

DELETE FROM `creature_template_addon` WHERE `entry` IN (510, 37994); -- Water Elemental (temporary + glyphed permanent)
INSERT INTO `creature_template_addon` (`entry`, `auras`) VALUES
(510, '32233'), -- Water Elemental: Avoidance
(37994, '32233'); -- Water Elemental (Glyph of Eternal Water): Avoidance

DELETE FROM `spell_script_names` WHERE `spell_id` = 58686 AND `ScriptName` = 'EG_spell_dk_glyph_of_the_ghoul';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(58686, 'EG_spell_dk_glyph_of_the_ghoul'); -- Glyph of the Ghoul
