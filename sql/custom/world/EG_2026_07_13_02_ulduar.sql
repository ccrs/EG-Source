--
UPDATE `creature_template` SET `speed_run` = 1.14286, `AIName` = '' WHERE `entry` = 34192;
UPDATE `creature` SET `MovementType` = 2, `wander_distance` = 0 WHERE `guid` IN (135783, 137560, 137632, 137633, 137634, 137635, 137636, 137637, 137638, 137639, 137640, 137641, 137642, 137643, 137644, 137645);
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryorguid` IN (-136578, -136579, -136580, -136581, -136582, -136583, -136584, -136585);
