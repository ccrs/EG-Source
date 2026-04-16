-- Pool of Tar
UPDATE `creature_template` SET `flags_extra`=`flags_extra`|1073741824 WHERE `entry`=33090;

-- Freya's adds
UPDATE `creature_template` SET `mechanic_immune_mask`= 
1 | 2 | 4 | 8 | 16 | 32 | 64 | 256 | 512 | 1024 | 2048 | 4096 | 8192 | 65536 | 131072 | 8388608 | 67108864 | 536870912 WHERE `entry` IN
(33385, 33397, 33396, 33088, 33168, 33228);

--
UPDATE `creature_template` SET `AIName`='', `ScriptName`='EG_npc_arachnopod_destroyer' WHERE `entry`=34183;
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid` IN (34183);

DELETE FROM `creature_template_spell` WHERE `CreatureID` = 34183;
INSERT INTO `creature_template_spell` (`CreatureID`, `Index`, `Spell`, `VerifiedBuild`) VALUES
(34183, 0, 64717, 0),
(34183, 1, 64779, 0),
(34183, 2, 64776, 0);

DELETE FROM `vehicle_template_accessory` WHERE `entry`=34183;
INSERT INTO `vehicle_template_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`, `summontype`, `summontimer`) VALUES 
(34183, 34184, 1, 0, 'Arachnopod - Clockwork Mechanic', 6, 500);

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry`=34183;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`) VALUES 
(34183, 63313, 0, 1);

--
DELETE FROM `spelldifficulty_dbc` WHERE `id` IN (46763);
INSERT INTO `spelldifficulty_dbc` (`id`, `spellid0`, `spellid1`, `spellid2`, `spellid3`) VALUES 
(46763, 46763, 64062, 0, 0);

-- Enslaved Fire Elemental
UPDATE `creature` SET `spawntimesecs`=604800 WHERE `id`=33838 AND `guid` IN (137557, 137558, 137559);
