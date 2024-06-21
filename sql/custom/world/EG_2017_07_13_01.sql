DELETE FROM `creature_template` WHERE (`name`="Raid Target Dummy" AND `subname`="You hate me, so punch me!") OR (`name`="Raid Dummy Controller" AND `subname`="Come talk to me!");
SET @centry:= 60002;
INSERT INTO `creature_template` (`entry`,`modelid1`,`name`,`subname`,`minlevel`,`maxlevel`,`faction`,`npcflag`,`scale`,`rank`,`unit_class`,`type`,`type_flags`,`MovementType`,`ArmorModifier`,`mechanic_immune_mask`,`flags_extra`,`ScriptName`) VALUES
(@centry+00, 28048, "Raid Target Dummy",     "You hate me, so punch me!", 83, 83, 168, 0, 2,   1, 6,  (4|8|16|64), 0, 9, 1, 617299839, (2|64|256|262144), "EG_npc_damage_test_dummy"),
(@centry+01, 24103, "Raid Dummy Controller", "Come talk to me!",          83, 83, 35,  1, 1.5, 1, 15, (4|16),      0, 9, 1, 0,         (2|64|262144),     "EG_npc_damage_test_controller");

DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (@centry+00, @centry+01);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`) VALUES
(@centry+00, '1', '0', '0', '1', '0', '0'),
(@centry+01, '1', '0', '0', '1', '0', '0');