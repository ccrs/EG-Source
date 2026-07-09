--
DELETE FROM `spell_script_names` WHERE `spell_id` IN (62337, 62933);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(62337, 'EG_spell_stonebark_petrified_bark'),
(62933, 'EG_spell_stonebark_petrified_bark');

DELETE FROM `spell_proc` WHERE `SpellId` IN (62337, 62933);
INSERT INTO `spell_proc` (`SpellId`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `ProcFlags`, `SpellTypeMask`, `SpellPhaseMask`, `HitMask`, `AttributesMask`, `DisableEffectsMask`, `ProcsPerMinute`, `Chance`, `Cooldown`, `Charges`) VALUES
(62337, 0, 0, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(62933, 0, 0, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0);

DELETE FROM `spelldifficulty_dbc` WHERE `id` = 62337;
INSERT INTO `spelldifficulty_dbc` (`id`, `spellid0`, `spellid1`, `spellid2`, `spellid3`) VALUES
(62337, 62337, 62933, 0, 0);
