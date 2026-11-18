--
UPDATE `smart_scripts` SET `target_param2` = 1 WHERE `entryorguid` IN (17994, 20034, 20883, 21299, 27972, 28201, 29411, 37023) AND `source_type` = 0 AND `id` = 0;
UPDATE `smart_scripts` SET `target_param2` = 1 WHERE `entryorguid` IN (-55065, -55064, -55062, -54999, -54996, -54994, -48400, -48394, -45571, -45570, -45537, 7999, 16146, 17871, 25363, 25370, 27966, 28201) AND `source_type` = 0 AND `id` = 1;
UPDATE `smart_scripts` SET `target_param2` = 1 WHERE `entryorguid` IN (16146, 17960, 18503, 27971, 28547, 28581, 36516) AND `source_type` = 0 AND `id` = 2;
UPDATE `smart_scripts` SET `target_param2` = 1 WHERE `entryorguid` IN (7999, 25370, 27971, 34198) AND `source_type` = 0 AND `id` = 3;
UPDATE `smart_scripts` SET `target_param2` = 1 WHERE `entryorguid` IN (-91249, -91248, -91247, -90987, -90986, -90985, 2784, 18829, 20864, 34198) AND `source_type` = 0 AND `id` = 4;
UPDATE `smart_scripts` SET `target_param2` = 1 WHERE `entryorguid` = 28998 AND `source_type` = 0 AND `id` = 5;
UPDATE `smart_scripts` SET `target_param2` = 1 WHERE `entryorguid` IN (29826, 30954, 31160) AND `source_type` = 0 AND `id` = 6;
UPDATE `smart_scripts` SET `target_param2` = 1 WHERE `entryorguid` = 29621 AND `source_type` = 0 AND `id` = 19;

UPDATE `smart_scripts` SET `target_type` = 5, `target_param2` = 1 WHERE `entryorguid` = 15392 AND `source_type` = 0 AND `id` = 1;
UPDATE `smart_scripts` SET `target_type` = 2 WHERE `entryorguid` = 19926 AND `source_type` = 0 AND `id` = 3;
UPDATE `smart_scripts` SET `target_type` = 2 WHERE `entryorguid` = 27712 AND `source_type` = 0 AND `id` = 0;
UPDATE `smart_scripts` SET `target_type` = 7 WHERE `entryorguid` IN (27414, 27425) AND `source_type` = 0 AND `id` = 1;

DELETE FROM `spell_script_names` WHERE `spell_id` IN (25471, 35831);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(25471, 'EG_spell_ruins_of_ahnqiraj_attack_order'),
(35831, 'EG_spell_netherstorm_ring_of_flame');
