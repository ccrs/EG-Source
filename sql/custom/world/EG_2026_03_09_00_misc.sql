--
UPDATE `creature_model_info` SET `BoundingRadius` = 0.50915, `CombatReach` = 1.0625 WHERE `DisplayID` IN (4267,2568,4535);
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'EG_npc_plague_slime' WHERE `entry` = 16243;
DELETE FROM `smart_scripts` WHERE `entryorguid` = 16243 AND `source_type` = 0;
UPDATE `creature_template` SET `ScriptName` = 'EG_npc_evolving_ectoplasm' WHERE `entry` = 3640;
