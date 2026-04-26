-- Arachnid Quarter
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask` | 32 WHERE `entry` IN
(15956, 29249,  -- Anub'Rekhan
 15953, 29268,  -- Grand Widow Faerlina
 15952, 29278); -- Maexxna

-- Plague Quarter
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask` | 32 WHERE `entry` IN
(15954, 29615,  -- Noth the Plaguebringer
 28919, 29113,  -- Noth the Plaguebringer (phase variants, included for safety)
 15936, 29701,  -- Heigan the Unclean
 16011, 29718); -- Loatheb

-- Construct Quarter
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask` | 32 WHERE `entry` IN
(16028, 29324,  -- Patchwerk
 15931, 29373,  -- Grobbulus
 15932, 29417,  -- Gluth
 15928, 29448); -- Thaddius

-- Military Quarter
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask` | 32 WHERE `entry` IN
(16061, 29940,  -- Instructor Razuvious
 16060, 29955,  -- Gothik the Harvester
 16064, 30603,  -- Thane Korth'azz
 16065, 30601,  -- Lady Blaumeux
 30549, 30600,  -- Baron Rivendare
 16063, 30602); -- Sir Zeliek

-- Frostwyrm Lair
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask` | 32 WHERE `entry` IN
(15989, 29991,  -- Sapphiron
 15990, 30061); -- Kel'Thuzad
