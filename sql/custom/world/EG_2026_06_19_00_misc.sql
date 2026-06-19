--
DELETE FROM `spell_proc` WHERE `SpellId` = 60510;
INSERT INTO `spell_proc` (`SpellId`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `ProcFlags`, `SpellTypeMask`, `SpellPhaseMask`, `HitMask`, `AttributesMask`, `DisableEffectsMask`, `ProcsPerMinute`, `Chance`, `Cooldown`, `Charges`) VALUES
(60510, 0, 0, 0, 0, 0, 16384, 2, 1, 0, 0, 0, 0, 2, 0, 0);
