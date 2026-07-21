--
DELETE FROM `spell_dbc` WHERE `Id` IN (37392, 37503);
INSERT INTO `spell_dbc` (`Id`, `Attributes`, `AttributesEx`, `AttributesEx2`, `CastingTimeIndex`, `ProcChance`, `DurationIndex`, `RangeIndex`, `EquippedItemClass`, `Effect1`, `EffectImplicitTargetA1`, `EffectRadiusIndex1`, `EffectMiscValue1`, `EffectMiscValueB1`, `DmgMultiplier1`, `DmgMultiplier2`, `DmgMultiplier3`, `SchoolMask`, `SpellName`) VALUES
(37392, 256, 268435456, 4, 1, 101, 552, 1, -1, 28, 18, 0, 21759, 64, 1, 1, 1, 1, 'Summon Top Bunny Caster'),
(37503, 384, 0, 4, 1, 101, 21, 7, -1, 28, 44, 13, 21796, 64, 1, 1, 1, 1, 'Summon Wyrm from Beyond');
