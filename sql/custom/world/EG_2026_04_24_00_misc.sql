-- DK avoidance script link
DELETE FROM `spell_script_names` WHERE `ScriptName` = 'spell_dk_avoidance_passive';
INSERT INTO `spell_script_names` (spell_id, ScriptName) VALUES
(62137, 'spell_dk_avoidance_passive');

-- Pet hit/crit/expertise inheritance (spell_gen_pet_calculate)
-- 19591 = hunter tamed pet passive 06 (melee + spell crit from owner)
-- 35695 = warlock pet passive crit (spell + melee crit from owner)
-- 61013 = warlock pet scaling 05 (melee hit + spell hit + expertise from owner)
-- 61017 = hunter pet scaling 04 (melee hit + spell hit + expertise from owner)
-- 61697 = DK pet scaling 03 (melee hit + spell hit from owner)
DELETE FROM `spell_script_names` WHERE `spell_id` IN (19591, 35695, 61013, 61017, 61697) AND `ScriptName` = 'spell_gen_pet_calculate';
INSERT INTO `spell_script_names` (spell_id, ScriptName) VALUES
(19591, 'spell_gen_pet_calculate'),
(35695, 'spell_gen_pet_calculate'),
(61013, 'spell_gen_pet_calculate'),
(61017, 'spell_gen_pet_calculate'),
(61697, 'spell_gen_pet_calculate');

-- DK ghoul auras (no creature family, not covered by LearnPetPassives)
-- 62137 = DK avoidance passive (Night of the Dead talent scaling)
-- 61697 = DK pet scaling 03 (hit/spell hit from owner)
DELETE FROM `creature_template_addon` WHERE entry IN (26125, 24207);
INSERT INTO `creature_template_addon` (entry, auras) VALUES
(26125, '62137 61697'),
(24207, '62137 61697');
