--
-- Group 1033 (bleed damage taken %): Stampede - Rhino exotic pet ability (57386, rank 1)
-- Group 1096 (spell cast-speed slow): Lava Breath - Core Hound exotic pet ability (58604, rank 1)
DELETE FROM `spell_group` WHERE (`id` = 1033 AND `spell_id` = 57386) OR (`id` = 1096 AND `spell_id` = 58604);
INSERT INTO `spell_group` (`id`, `spell_id`) VALUES
(1033, 57386),
(1096, 58604);

-- fix groups 1089/1090 - they referenced -1085 (Divine Spirit / Prayer of Spirit / Scroll of Spirit, all MOD_STAT Spirit)
-- instead of -1088 (Strength of Earth Totem 8076 / Horn of Winter 57330)
DELETE FROM `spell_group` WHERE `id` IN (1089, 1090) AND `spell_id` IN (-1085, -1088);
INSERT INTO `spell_group` (`id`, `spell_id`) VALUES
(1089, -1088),
(1090, -1088);

-- Group 1102 - Sha'tari Skyguard buff copies (40055/40165/40166/40167) -> EXCLUSIVE
-- Group 1103 - Apexis Vibrations/Emanations/Enlightenment (40623/40625/40626) -> EXCLUSIVE_HIGHEST
DELETE FROM `spell_group_stack_rules` WHERE `group_id` IN (1102, 1103);
INSERT INTO `spell_group_stack_rules` (`group_id`, `stack_rule`) VALUES
(1102, 1),
(1103, 4);
