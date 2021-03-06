#include <map>
#include <iostream>
#include <vector>
#include <set>
#include "parser.h"

using namespace std;

map<string, vector<int>> toColor = {
    {"minecraft:grass_block", {0,255,0}},
    {"minecraft:grass_path", {0,255,0}},
    {"minecraft:mossy_cobblestone", {0,150,0}},
    {"minecraft:snow", {255,255,255}},
    {"minecraft:snow_block", {255,255,255}},
    {"minecraft:ice", {220,255,220}},
    {"minecraft:clay", {220,255,220}},
    {"minecraft:white_concrete", {230,230,230}},
    {"minecraft:white_terracotta", {230,230,230}},
    {"minecraft:obsidian", {10,10,10}},
    {"minecraft:stone", {100,100,100}},
    {"minecraft:granite", {130,130,130}},
    {"minecraft:cobblestone", {70,70,70}},
    {"minecraft:cobblestone_stairs", {70,70,70}},
    {"minecraft:cobblestone_slab", {70,70,70}},
    {"minecraft:andesite", {120,120,120}},
    {"minecraft:gravel", {80,80,80}},
    {"minecraft:water", {0,0,255}},
    {"minecraft:sand",  {255,255,0}},
    {"minecraft:red_sand",  {255,150,0}},
    {"minecraft:sandstone",  {220,220,0}},
    {"minecraft:smooth_sandstone",  {220,220,0}},
    {"minecraft:cut_sandstone",  {220,220,0}},
    {"minecraft:red_sandstone",  {220,180,0}},
    {"minecraft:hay_block",  {220,220,0}},
    {"minecraft:sandstone_slab",  {220,220,0}},
    {"minecraft:smooth_sandstone_slab",  {220,220,0}},
    {"minecraft:diamond_block", {100,100,255}},
    {"minecraft:dirt", {117,104,49}},
    {"minecraft:podzol", {117,104,49}},
    {"minecraft:farmland", {117,104,49}},
    {"minecraft:coarse_dirt", {97,84,29}},
    {"minecraft:birch_leaves", {63,143,73}},
    {"minecraft:spruce_leaves", {63,143,73}},
    {"minecraft:oak_leaves", {63,143,73}},
    {"minecraft:acacia_leaves", {63,143,73}},
    {"minecraft:dark_oak_leaves", {63,143,73}},
    {"minecraft:jungle_leaves", {63,143,73}},
    {"minecraft:oak_log", {82,72,12}}, 
    {"minecraft:dark_oak_log", {82,72,12}}, 
    {"minecraft:birch_log", {82,72,12}},
    {"minecraft:spruce_log", {82,72,12}},
    {"minecraft:jungle_log", {82,72,12}},
    {"minecraft:acacia_log", {80,57,12}}, 
    {"minecraft:oak_wood", {82,72,12}}, 
    {"minecraft:dark_oak_wood", {82,72,12}}, 
    {"minecraft:acacia_wood", {100,72,12}}, 
    {"minecraft:birch_wood", {82,72,12}},
    {"minecraft:spruce_wood", {82,72,12}},
    {"minecraft:jungle_wood", {82,72,12}},
    {"minecraft:stripped_oak_wood", {82,72,12}}, 
    {"minecraft:stripped_dark_oak_wood", {82,72,12}}, 
    {"minecraft:stripped_acacia_wood", {100,72,12}}, 
    {"minecraft:stripped_birch_wood", {82,72,12}},
    {"minecraft:stripped_spruce_wood", {82,72,12}},
    {"minecraft:stripped_jungle_wood", {82,72,12}},
    {"minecraft:oak_planks", {110,90,30}},
    {"minecraft:oak_compostor", {110,90,30}},
    {"minecraft:oak_stairs", {110,90,30}},
    {"minecraft:dark_oak_planks", {110,90,30}},
    {"minecraft:dark_oak_stairs", {110,90,30}},
    {"minecraft:jungle_planks", {110,90,30}},
    {"minecraft:jungle_stairs", {110,90,30}},
    {"minecraft:spruce_planks", {110,90,30}},
    {"minecraft:spruce_stairs", {110,90,30}},
    {"minecraft:birch_planks", {110,110,40}},
    {"minecraft:birch_stairs", {110,110,40}},
    {"minecraft:cactus", {55,143,36}},
    {"minecraft:lava", {207, 106, 52}},
    {"minecraft:diorite", {212,208,205}},
    {"minecraft:andesite", {227,158,120}},
    {"minecraft:black_concrete_powder", {20,20,20}},
    {"minecraft:smooth_quartz", {200,200,200}},
    {"minecraft:coal_ore", {40,40,40}},
    {"minecraft:black_concrete", {20,20,20}},
    {"minecraft:green_terracotta", {43,94,39}},
    {"minecraft:lime_terracotta", {63,114,59}},
    {"minecraft:yellow_terracotta", {189,170,62}},
    {"minecraft:red_terracotta", {166,58,28}},
    {"minecraft:black_terracotta", {20,20,20}},
    {"minecraft:gray_terracotta", {80,80,80}},
    {"minecraft:gray_terracotta_powder", {80,80,80}},
    {"minecraft:gray_concrete", {80,80,80}},
    {"minecraft:gray_concrete_powder", {80,80,80}},
    {"minecraft:black_terracotta_powder", {20,20,20}},
    {"minecraft:brown_terracotta", {128,107,68}},
    {"minecraft:brown_terracotta_powder", {128,107,68}},
    {"minecraft:pink_terracotta", {252,157,212}},
    {"minecraft:light_blue_terracotta", {188,247,247}},
    {"minecraft:blue_terracotta", {148,207,207}},
    {"minecraft:terracotta", {150,102,29}},
    {"minecraft:iron_ore", {130,115,92}},
    
};

set<string> skipBlocks = {
  "minecraft:grass",
  "minecraft:air",
  "minecraft:cave_air",
  "minecraft:rose_bush",
  "minecraft:poppy", 
  "minecraft:lilac",
  "minecraft:dead_bush",
  "minecraft:dandelion",
  "minecraft:sugar_cane",
  "minecraft:allium",
  "minecraft:blue_orchid",
  "minecraft:cornflower",
  "minecraft:tall_grass",
  "minecraft:oak_sapling",
  "minecraft:vine",
  "minecraft:lantern",
  "minecraft:large_fern",
  "minecraft:fern",
  "minecraft:tall_seagrass",
  "minecraft:sunflower",
  "minecraft:spruce_fence",
  "minecraft:orange_tulip",
  "minecraft:kelp",
  "minecraft:lily_pad",
  "minecraft:oak_fence",
  "minecraft:oak_fence_gate",
  "minecraft:birch_fence",
  "minecraft:birch_fence_gate",
  "minecraft:jungle_fence",
  "minecraft:jungle_fence_gate",
  "minecraft:spruce_fence",
  "minecraft:spruce_fence_gate",
  "minecraft:seagrass",
  "minecraft:oxeye_daisy",
  "minecraft:red_tulip",
  "minecraft:pink_tulip",
  "minecraft:peony",
  "minecraft:carrots",
  "minecraft:potatoes",
  "minecraft:wheat",
  "minecraft:beetroots",
  "minecraft:kelp_plant",
  "minecraft:bamboo",
  "minecraft:cocoa",
  "minecraft:white_carpet",
  "minecraft:sea_pickle",
  "minecraft:torch",
  "minecraft:wall_torch",
  "minecraft:bell",
  "minecraft:oak_trapdoor",
  "minecraft:birch_trapdoor",
  "minecraft:spruce_trapdoor",
  "minecraft:jungle_trapdoor",
  "minecraft:bubble_column",
  "minecraft:lily_of_the_valley",
  "minecraft:white_tulip",
  "minecraft:azure_bluet",
  "minecraft:jungle_button",
  "minecraft:melon_stem",
  "minecraft:potted_cactus",
  "minecraft:potted_dead_bush",
  "minecraft:granite_wall",
  "minecraft:sandstone_wall"
};