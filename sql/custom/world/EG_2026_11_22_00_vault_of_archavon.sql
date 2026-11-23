--
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` IN (58666, 60882) AND `spell_effect` = 58672 AND `type` = 1;

DELETE FROM `spell_script_names` WHERE `spell_id` IN (58678, 64218, 66690, 72034, 72096, 72079) AND `ScriptName` IN ('EG_spell_archavon_rock_shards_periodic', 'EG_spell_emalon_overcharge', 'EG_spell_koralon_flaming_cinder', 'EG_spell_toravon_whiteout', 'EG_spell_toravon_frozen_orb_health_periodic');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(58678, 'EG_spell_archavon_rock_shards_periodic'),
(64218, 'EG_spell_emalon_overcharge'),
(66690, 'EG_spell_koralon_flaming_cinder'),
(72034, 'EG_spell_toravon_whiteout'),
(72096, 'EG_spell_toravon_whiteout'),
(72079, 'EG_spell_toravon_frozen_orb_health_periodic');

UPDATE `spell_dbc` SET `Effect1` = 28, `EffectImplicitTargetA1` = 73, `EffectRadiusIndex1` = 13, `EffectMiscValue1` = 34049, `EffectMiscValueB1` = 64 WHERE `Id` = 64232;

DELETE FROM `creature_summon_groups` WHERE `summonerId` = 33993 AND `summonerType` = 0 AND `groupId` = 0;
INSERT INTO `creature_summon_groups` (`summonerId`, `summonerType`, `groupId`, `entry`, `position_x`, `position_y`, `position_z`, `orientation`, `summonType`, `summonTime`, `Comment`) VALUES
(33993, 0, 0, 33998, -205.2377, -281.0328, 91.62176, 1.797689, 5, 0, 'Vault of Archavon - Emalon - Tempest Minion'),
(33993, 0, 0, 33998, -233.2622, -280.3559, 91.82957, 1.343904, 5, 0, 'Vault of Archavon - Emalon - Tempest Minion'),
(33993, 0, 0, 33998, -232.9089, -296.9768, 91.71149, 1.396263, 5, 0, 'Vault of Archavon - Emalon - Tempest Minion'),
(33993, 0, 0, 33998, -204.7726, -297.2969, 91.71247, 1.762783, 5, 0, 'Vault of Archavon - Emalon - Tempest Minion');

UPDATE `creature_template` SET `ScriptName` = 'EG_npc_tempest_minion' WHERE `entry` IN (33998, 34049);

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 13 AND `SourceGroup` = 7 AND `SourceEntry` = 64218;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `ConditionStringValue1`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(13, 7, 64218, 0, 0, 31, 0, 3, 33998, 0, '', 0, 0, 0, '', 'Overcharge - target Tempest Minion');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 13 AND `SourceGroup` = 4 AND `SourceEntry` IN (72034, 72096);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `ConditionStringValue1`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(13, 4, 72034, 0, 0, 31, 0, 3, 38439, 0, '', 0, 0, 0, '', 'Whiteout - target Toravon Stalker'),
(13, 4, 72096, 0, 0, 31, 0, 3, 38439, 0, '', 0, 0, 0, '', 'Whiteout - target Toravon Stalker');

UPDATE `creature_template_addon` SET `auras` = '72122' WHERE `entry` = 38483;

DELETE FROM `spell_proc` WHERE `SpellId` = 72122;
INSERT INTO `spell_proc` (`SpellId`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMask0`, `SpellFamilyMask1`, `SpellFamilyMask2`, `ProcFlags`, `SpellTypeMask`, `SpellPhaseMask`, `HitMask`, `AttributesMask`, `DisableEffectsMask`, `ProcsPerMinute`, `Chance`, `Cooldown`, `Charges`) VALUES
(72122, 0, 0, 0, 0, 0, 4, 0, 0, 12287, 0, 0, 0, 0, 3000, 0);

UPDATE `smart_scripts` SET `event_param1` = 30000, `event_param2` = 42000 WHERE `entryorguid` = 32353 AND `source_type` = 0 AND `id` IN (0, 1);
UPDATE `smart_scripts` SET `event_param3` = 16000, `event_param4` = 18000 WHERE `entryorguid` = 32353 AND `source_type` = 0 AND `id` IN (4, 5);
UPDATE `smart_scripts` SET `event_param3` = 8000, `event_param4` = 18000 WHERE `entryorguid` = 34015 AND `source_type` = 0 AND `id` = 0;

UPDATE `creature_template` SET `VehicleId` = 303 WHERE `entry` = 31722;

UPDATE `creature_template` SET `speed_run` = 0.595238 WHERE `entry` = 34200;

DELETE FROM `achievement_criteria_data` WHERE `criteria_id` IN (11902, 11903, 13107, 13108) AND `type` IN (0, 12);
INSERT INTO `achievement_criteria_data` (`criteria_id`, `type`, `value1`, `value2`, `ScriptName`) VALUES
(11902, 12, 0, 0, ''),
(11903, 12, 1, 0, ''),
(13107, 12, 0, 0, ''),
(13108, 12, 1, 0, '');

UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` IN (31125, 31722, 33993, 33994, 35013, 35360, 38433, 38462);
UPDATE `creature_template` SET `mechanic_immune_mask` = 550189887 WHERE `entry` IN (32353, 32368, 34015, 34016, 35143, 35359, 38482, 38483);
