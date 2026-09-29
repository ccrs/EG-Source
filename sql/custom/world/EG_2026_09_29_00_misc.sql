--
UPDATE `creature_template` SET `ScriptName` = 'EG_npc_brewfest_dark_iron_attack_generator' WHERE `entry` = 23703;
UPDATE `creature_template` SET `ScriptName` = 'EG_npc_brewfest_dark_iron_guzzler' WHERE `entry` = 23709;
UPDATE `gameobject_template` SET `ScriptName` = 'EG_go_brewfest_dark_iron_mole_machine' WHERE `entry` = 186685;

DELETE FROM `spell_script_names` WHERE `spell_id` IN (42436, 42300);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(42436, 'EG_spell_brewfest_toss_mug'),
(42300, 'EG_spell_brewfest_add_mug');

DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 13 AND `SourceEntry` = 42393;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionValue1`, `ConditionValue2`, `Comment`) VALUES
(13, 1, 42393, 0, 31, 3, 23700, 'Brewfest - Attack Keg - target Barleybrew Festive Keg'),
(13, 1, 42393, 1, 31, 3, 23702, 'Brewfest - Attack Keg - target Thunderbrew Festive Keg'),
(13, 1, 42393, 2, 31, 3, 23706, 'Brewfest - Attack Keg - target Gordok Festive Keg'),
(13, 1, 42393, 3, 31, 3, 24372, 'Brewfest - Attack Keg - target Drohn''s Distillery Festive Keg'),
(13, 1, 42393, 4, 31, 3, 24373, 'Brewfest - Attack Keg - target T''chali''s Voodoo Brew Festive Keg');

DELETE FROM `creature_text` WHERE `CreatureID` IN (24536, 24484, 23709);
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(24536, 0, 0, 'Oh, we''re from Blackrock Mountain,\nWe''ve come ta drink yer brew!\nDark Iron dwarves, they do not lie,\nAnd so yeh know it''s true!', 12, 0, 100, 0, 0, 0, 22441, 0, 'Dark Iron Herald - Dark Iron Attack - verse 1'),
(24536, 1, 0, 'Yeh will not try our bitter,\nYeh will not serve our ale!\nBut have Brewfest without our lot?\nJust try it, and ye''ll fail!', 12, 0, 100, 0, 0, 0, 22442, 0, 'Dark Iron Herald - Dark Iron Attack - verse 2'),
(24536, 2, 0, 'So lift a mug to Coren,\nAnd Hurley Blackbreath too!\nThis drink is weak, without much kick,\nBut oi!  At least it''s brew!', 12, 0, 100, 0, 0, 0, 22443, 0, 'Dark Iron Herald - Dark Iron Attack - verse 3'),
(24536, 3, 0, 'We''ll drink yer stout and lager,\nDrain all the pints and kegs!\nWe''ll drink and brawl and brawl and drink,\n''til we can''t feel our legs!', 12, 0, 100, 0, 0, 0, 22444, 0, 'Dark Iron Herald - Dark Iron Attack - verse 4'),
(24536, 4, 0, 'And when the brew''s all missin''\nTa Shadowforge we''ll hop,\nA bitter toast ta Ragnaros...\n... but bring him not a drop!', 12, 0, 100, 0, 0, 0, 22445, 0, 'Dark Iron Herald - Dark Iron Attack - verse 5'),
(24536, 5, 0, 'RETREAT!!  We''ve already lost $3096W and we can''t afford to lose any more!!', 14, 0, 100, 0, 0, 0, 22602, 0, 'Dark Iron Herald - Dark Iron Attack - camp defended'),
(24536, 6, 0, 'We did it boys!  Now back to the Grim Guzzler and we''ll drink to the $3096W that were injuredl!', 14, 0, 100, 0, 0, 0, 22601, 0, 'Dark Iron Herald - Dark Iron Attack - kegs destroyed'),
(24484, 0, 0, 'Dark Iron dwarves!!!', 12, 0, 100, 0, 0, 0, 22672, 0, 'Brewfest Reveler - Dark Iron Attack - flee'),
(24484, 0, 1, 'They''re after the beer!', 12, 0, 100, 0, 0, 0, 22673, 0, 'Brewfest Reveler - Dark Iron Attack - flee'),
(24484, 0, 2, 'Someone has to save the beer!', 12, 0, 100, 0, 0, 0, 22674, 0, 'Brewfest Reveler - Dark Iron Attack - flee'),
(24484, 0, 3, 'If you value your beer, run for it!', 12, 0, 100, 0, 0, 0, 22675, 0, 'Brewfest Reveler - Dark Iron Attack - flee'),
(24484, 0, 4, 'Run!  It''s the Dark Iron dwarves!', 12, 0, 100, 0, 0, 0, 22676, 0, 'Brewfest Reveler - Dark Iron Attack - flee'),
(23709, 0, 0, 'No one expects the Dark Iron dwarves!', 12, 0, 100, 0, 0, 0, 22163, 0, 'Dark Iron Guzzler - Dark Iron Attack - arrival'),
(23709, 0, 1, 'Drink it all boys!', 12, 0, 100, 0, 0, 0, 22316, 0, 'Dark Iron Guzzler - Dark Iron Attack - arrival'),
(23709, 0, 2, 'It''s not a party without some crashers!', 12, 0, 100, 0, 0, 0, 22317, 0, 'Dark Iron Guzzler - Dark Iron Attack - arrival'),
(23709, 0, 3, 'Did someone say, "Free Brew"?', 12, 0, 100, 0, 0, 0, 22318, 0, 'Dark Iron Guzzler - Dark Iron Attack - arrival'),
(23709, 0, 4, 'DRINK! BRAWL! DRINK! BRAWL!', 12, 0, 100, 0, 0, 0, 22833, 0, 'Dark Iron Guzzler - Dark Iron Attack - arrival');

DELETE FROM `creature_text` WHERE `CreatureID` IN (23683, 23684, 23685, 24492, 24493) AND `GroupID` IN (1, 2);
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(23683, 1, 0, 'Chug and chuck!  Chug and chuck!', 12, 0, 100, 0, 0, 0, 23621, 0, 'Maeve Barleybrew - Dark Iron Attack - mug reminder'),
(23683, 1, 1, 'Down the free brew and pelt the Guzzlers with your mug!', 12, 0, 100, 0, 0, 0, 23620, 0, 'Maeve Barleybrew - Dark Iron Attack - mug reminder'),
(23683, 2, 0, 'SOMEONE TRY THIS SUPER BREW!!', 14, 0, 100, 0, 0, 0, 22547, 0, 'Maeve Barleybrew - Dark Iron Attack - super brew'),
(23684, 1, 0, 'Chug and chuck!  Chug and chuck!', 12, 0, 100, 0, 0, 0, 23621, 0, 'Ita Thunderbrew - Dark Iron Attack - mug reminder'),
(23684, 1, 1, 'Down the free brew and pelt the Guzzlers with your mug!', 12, 0, 100, 0, 0, 0, 23620, 0, 'Ita Thunderbrew - Dark Iron Attack - mug reminder'),
(23684, 2, 0, 'SOMEONE TRY THIS SUPER BREW!!', 14, 0, 100, 0, 0, 0, 22547, 0, 'Ita Thunderbrew - Dark Iron Attack - super brew'),
(23685, 1, 0, 'Chug and chuck!  Chug and chuck!', 12, 0, 100, 0, 0, 0, 23621, 0, 'Gordok Brew Barker - Dark Iron Attack - mug reminder'),
(23685, 1, 1, 'Down the free brew and pelt the Guzzlers with your mug!', 12, 0, 100, 0, 0, 0, 23620, 0, 'Gordok Brew Barker - Dark Iron Attack - mug reminder'),
(23685, 2, 0, 'SOMEONE TRY THIS SUPER BREW!!', 14, 0, 100, 0, 0, 0, 22547, 0, 'Gordok Brew Barker - Dark Iron Attack - super brew'),
(24492, 1, 0, 'Chug and chuck!  Chug and chuck!', 12, 0, 100, 0, 0, 0, 23621, 0, 'Drohn''s Distillery Barker - Dark Iron Attack - mug reminder'),
(24492, 1, 1, 'Down the free brew and pelt the Guzzlers with your mug!', 12, 0, 100, 0, 0, 0, 23620, 0, 'Drohn''s Distillery Barker - Dark Iron Attack - mug reminder'),
(24492, 2, 0, 'SOMEONE TRY THIS SUPER BREW!!', 14, 0, 100, 0, 0, 0, 22547, 0, 'Drohn''s Distillery Barker - Dark Iron Attack - super brew'),
(24493, 1, 0, 'Chug and chuck!  Chug and chuck!', 12, 0, 100, 0, 0, 0, 23621, 0, 'T''chali''s Voodoo Brewery Barker - Dark Iron Attack - mug reminder'),
(24493, 1, 1, 'Down the free brew and pelt the Guzzlers with your mug!', 12, 0, 100, 0, 0, 0, 23620, 0, 'T''chali''s Voodoo Brewery Barker - Dark Iron Attack - mug reminder'),
(24493, 2, 0, 'SOMEONE TRY THIS SUPER BREW!!', 14, 0, 100, 0, 0, 0, 22547, 0, 'T''chali''s Voodoo Brewery Barker - Dark Iron Attack - super brew');
