--
DELETE FROM `spelldifficulty_dbc` WHERE `id` IN (8374, 62240, 62269, 62283, 62285, 62310, 62325, 62437, 62438, 62483, 62484, 62485, 62572, 62584, 62589, 62598, 62623, 62648, 62649, 62653, 62654, 62663, 62664);
INSERT INTO `spelldifficulty_dbc` (`id`, `spellid0`, `spellid1`, `spellid2`, `spellid3`) VALUES
(8374, 8374, 38761, 0, 0), -- Arcing Smash
(62240, 62240, 64087, 0, 0), -- Solar Flare
(62269, 62269, 63490, 0, 0), -- Rune of Death
(62283, 62283, 62930, 0, 0), -- Iron Roots (Freya)
(62285, 62285, 62931, 0, 0), -- Thorn Swarm
(62310, 62310, 62928, 0, 0), -- Impale
(62325, 62325, 62932, 0, 0), -- Ground Tremor (Elder Stonebark)
(62437, 62437, 62859, 0, 0), -- Ground Tremor (Freya)
(62438, 62438, 62861, 0, 0), -- Iron Roots (Elder Ironbranch)
(62483, 62483, 65589, 0, 0), -- Stonebark's Essence Channel
(62484, 62484, 65588, 0, 0), -- Ironbranch's Essence Channel
(62485, 62485, 65587, 0, 0), -- Brightleaf's Essence Channel
(62572, 62572, 62870, 0, 0), -- Lifebinder's Gift Trigger Missile
(62584, 62584, 64185, 0, 0), -- Lifebinder's Gift
(62589, 62589, 63571, 0, 0), -- Nature's Fury
(62598, 62598, 62937, 0, 0), -- Detonate
(62623, 62623, 62872, 0, 0), -- Sunbeam
(62648, 62648, 62939, 0, 0), -- Lightning Lash
(62649, 62649, 62938, 0, 0), -- Stormbolt
(62653, 62653, 62935, 0, 0), -- Tidal Wave
(62654, 62654, 62936, 0, 0), -- Tidal Wave (effect)
(62663, 62663, 64190, 0, 0), -- Hardened Bark (aura)
(62664, 62664, 64191, 0, 0); -- Hardened Bark

DELETE FROM `spell_script_names` WHERE `spell_id` IN (50810, 50811, 64876, 66351);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(50810, 'spell_krystallus_shatter'), -- Shatter
(50811, 'spell_krystallus_shatter_effect'), -- Shatter
(64876, 'EG_spell_energy_sap'), -- Energy Sap
(66351, 'spell_mimiron_proximity_explosion'); -- Explosion
