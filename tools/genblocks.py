#!/usr/bin/env python3

from audioop import cross
from turtle import shape


with open("tools/blocks.txt") as f:
    blocks = [b[:-1] for b in f.readlines() if b.startswith("minecraft:")]


def is_bubble_column(b: str):
    return b.endswith("bubble_column")
def is_fence(b: str):
    return b.endswith("fence")
def is_fence_gate(b: str):
    return b.endswith("fence_gate")
def is_nether_portal(b: str):
    return b.endswith("nether_portal")
def is_campfire(b: str):
    return b.endswith("campfire")
def is_fire(b: str):
    return b.endswith("fire") and not b.endswith("campfire")
def is_glass_pane(b: str):
    return b.endswith("glass_pane")
def is_glass(b: str):
    return b.endswith("glass") and not b.endswith("glass_pane")
def is_iron_bars(b: str):
    return b.endswith("iron_bars")
def is_wall_banner(b: str):
    return b.endswith("wall_banner")
def is_banner(b: str):
    return b.endswith("banner") and not b.endswith("wall_banner")
def is_stairs(b: str):
    return b.endswith("stairs")
def is_bed(b: str):
    return b.endswith("bed")
def is_end_rod(b: str):
    return b.endswith("end_rod")
def is_dragon_egg(b: str):
    return b.endswith("dragon_egg")
def is_dragon_head(b: str):
    return b.endswith("dragon_head")
def is_dragon_wall_head(b: str):
    return b.endswith("dragon_wall_head")
def is_end_portal(b: str):
    return b.endswith("end_portal")
def is_end_gateway(b: str):
    return b.endswith("end_gateway")
def is_shulker_box(b: str):
    return b.endswith("shulker_box")
def is_stonecutter(b: str):
    return b.endswith("stonecutter")
def is_chest(b: str):
    return b.endswith("chest") and not b.endswith("ender_chest")
def is_ender_chest(b: str):
    return b.endswith("ender_chest")
def is_brewing_stand(b: str):
    return b.endswith("brewing_stand")
def is_enchanting_table(b: str):
    return b.endswith("enchanting_table")
def is_cauldron(b: str):
    return b.endswith("cauldron")
def is_anvil(b: str):
    return b.endswith("anvil")
def is_grindstone(b: str):
    return b.endswith("grindstone")
def is_conduit(b: str):
    return b.endswith("conduit")
def is_bell(b: str):
    return b.endswith("bell")
def is_lectern(b: str):
    return b.endswith("lectern")
def is_composter(b: str):
    return b.endswith("composter")
def is_cake(b: str):
    return b.endswith("cake") and not b.endswith("candle_cake")
def is_candle_cake(b: str):
    return b.endswith("candle_cake")
def is_candle(b: str):
    return b.endswith("candle")
def is_scaffolding(b: str):
    return b.endswith("scaffolding")
def is_slab(b: str):
    return b.endswith("slab")
def is_hopper(b: str):
    return b.endswith("hopper")
def is_piston(b: str):
    return b.endswith("piston") and not b.endswith("moving_piston")
def is_piston_head(b: str):
    return b.endswith("piston_head")
def is_moving_piston(b: str):
    return b.endswith("moving_piston")
def is_daylight_detector(b: str):
    return b.endswith("daylight_detector")
def is_pressure_plate(b: str):
    return b.endswith("pressure_plate")
def is_redstone_wire(b: str):
    return b.endswith("redstone_wire")
def is_tripwire_hook(b: str):
    return b.endswith("tripwire_hook")
def is_tripwire(b: str):
    return b.endswith("tripwire")
def is_lever(b: str):
    return b.endswith("lever")
def is_button(b: str):
    return b.endswith("button")
def is_wall_torch(b: str):
    return b.endswith("wall_torch")
def is_torch(b: str):
    return b.endswith("torch") and not b.endswith("wall_torch")
def is_redstone_repeater(b: str):
    return b.endswith("redstone_repeater")
def is_redstone_comparator(b: str):
    return b.endswith("redstone_comparator")
def is_trapdoor(b: str):
    return b.endswith("trapdoor")
def is_door(b: str):
    return b.endswith("door") and not b.endswith("trapdoor")
def is_wall(b: str):
    return b.endswith("wall")
def is_rail(b: str):
    return b.endswith("rail")
def is_lantern(b: str):
    return b.endswith("lantern") and not b.endswith("o_lantern") and not b.endswith("sea_lantern")
def is_ladder(b: str):
    return b.endswith("ladder")
def is_sea_pickle(b: str):
    return b.endswith("sea_pickle")
def is_flower_pot(b: str): # 空花盆
    return b.endswith("flower_pot")
def is_chain(b: str):
    return b.endswith("chain")
def is_turtle_egg(b: str):
    return b.endswith("turtle_egg")
def is_wall_sign(b: str):
    return b.endswith("wall_sign")
def is_sign(b: str):
    return b.endswith("sign") and not b.endswith("wall_sign")
def is_head(b: str):
    return b.endswith("head") and not b.endswith("wall_head") and not is_dragon_head(b) and not is_piston_head(b)
def is_wall_head(b: str):
    return b.endswith("wall_head") and not is_dragon_wall_head(b)
def is_coral_fan(b: str):
    return b.endswith("coral_fan")
def is_coral_wall_fan(b: str):
    return b.endswith("coral_wall_fan")
def is_tinted_flower_pot_cross(b):
    '''models/block/tinted_flower_pot_cross.json'''
    return b.endswith("potted_fern")
def is_flower_pot_cross(b):
    '''models/block/flower_pot_cross.json'''
    return b in [
        "minecraft:potted_acacia_sapling",
        "minecraft:potted_allium",
        "minecraft:potted_azure_bluet",
        "minecraft:potted_birch_sapling",
        "minecraft:potted_blue_orchid",
        "minecraft:potted_brown_mushroom",
        "minecraft:potted_cornflower",
        "minecraft:potted_crimson_fungus",
        "minecraft:potted_crimson_roots",
        "minecraft:potted_dandelion",
        "minecraft:potted_dark_oak_sapling",
        "minecraft:potted_dead_bush",
        "minecraft:potted_jungle_sapling",
        "minecraft:potted_lily_of_the_valley",
        "minecraft:potted_oak_sapling",
        "minecraft:potted_orange_tulip",
        "minecraft:potted_oxeye_daisy",
        "minecraft:potted_pink_tulip",
        "minecraft:potted_poppy",
        "minecraft:potted_red_mushroom",
        "minecraft:potted_red_tulip",
        "minecraft:potted_spruce_sapling",
        "minecraft:potted_warped_fungus",
        "minecraft:potted_warped_roots",
        "minecraft:potted_white_tulip",
        "minecraft:potted_wither_rose",
    ]
def is_stem_growth(b: str):
    return b in [
        "minecraft:melon_stem",
        "minecraft:pumpkin_stem"
    ]
def is_attached_stem(b: str):
    return b in [
        "minecraft:attached_melon_stem",
        "minecraft:attached_pumpkin_stem"
    ]
def is_crop(b: str):
    '''models/block/crop.json'''
    return b.endswith("wheat") or b.endswith("beetroots") or b.endswith("nether_wart") or b.endswith("potatoes") or b.endswith("carrots")
def is_tinted_cross(b: str):
    '''models/block/tinted_cross.json'''
    return b in [
        "minecraft:bamboo_sapling",
        "minecraft:fern",
        "minecraft:grass",
        "minecraft:kelp",
        "minecraft:kelp_plant",
        "minecraft:large_fern_bottom",
        "minecraft:large_fern_top",
        "minecraft:sugar_cane",
        "minecraft:tall_grass_bottom",
        "minecraft:tall_grass_top",
    ]
def is_cross(b: str):
    '''models/block/cross.json'''
    return b in [
        "minecraft:acacia_sapling",
        "minecraft:allium",
        "minecraft:azure_bluet",
        "minecraft:birch_sapling",
        "minecraft:blue_orchid",
        "minecraft:brain_coral",
        "minecraft:brown_mushroom",
        "minecraft:bubble_coral",
        "minecraft:cobweb",
        "minecraft:cornflower",
        "minecraft:crimson_fungus",
        "minecraft:crimson_roots",
        "minecraft:dandelion",
        "minecraft:dark_oak_sapling",
        "minecraft:dead_brain_coral",
        "minecraft:dead_bubble_coral",
        "minecraft:dead_bush",
        "minecraft:dead_fire_coral",
        "minecraft:dead_horn_coral",
        "minecraft:dead_tube_coral",
        "minecraft:fire_coral",
        "minecraft:horn_coral",
        "minecraft:jungle_sapling",
        "minecraft:lilac_bottom",
        "minecraft:lilac_top",
        "minecraft:lily_of_the_valley",
        "minecraft:nether_sprouts",
        "minecraft:oak_sapling",
        "minecraft:orange_tulip",
        "minecraft:oxeye_daisy",
        "minecraft:peony_bottom",
        "minecraft:peony_top",
        "minecraft:pink_tulip",
        "minecraft:poppy",
        "minecraft:red_mushroom",
        "minecraft:red_tulip",
        "minecraft:rose_bush_bottom",
        "minecraft:rose_bush_top",
        "minecraft:spruce_sapling",
        "minecraft:sunflower_bottom",
        "minecraft:sweet_berry_bush",
        "minecraft:tube_coral",
        "minecraft:twisting_vines",
        "minecraft:twisting_vines_plant",
        "minecraft:warped_fungus",
        "minecraft:warped_roots",
        "minecraft:weeping_vines",
        "minecraft:weeping_vines_plant",
        "minecraft:white_tulip",
        "minecraft:wither_rose",
    ]
def is_carpet(b: str):
    return b.endswith("carpet")
def is_cocoa(b: str):
    return b.endswith("cocoa")
def is_beacon(b: str):
    return b.endswith("beacon")
def is_farmland(b: str):
    return b.endswith("farmland")
def is_lava(b: str):
    return b.endswith("lava")
def is_water(b: str):
    return b.endswith("water")
def is_snow(b: str):
    return b.endswith("snow")
def is_furnace(b: str):
    return b.endswith("furnace")
def is_smoker(b: str):
    return b.endswith("smoker")
def is_redstone_ore(b: str):
    return b.endswith("redstone_ore")
def is_redstone_torch(b: str):
    return b.endswith("redstone_torch")
def is_redstone_lamp(b: str):
    return b.endswith("redstone_lamp")
def is_cube(b: str):
    return b[len("minecraft:"):] in "acacia_log acacia_planks andesite barrel bedrock bee_hive bee_nest acacia_wood birch_log birch_planks birch_wood black_concrete black_concrete_powder black_glazed_terracotta black_wool blast_furnace blue_concrete blue_concrete_powder blue_glazed_terracotta blue_ice blue_terracotta blue_wool bone_block bookshelf brain_coral_block bricks brown_concrete brown_concrete_powder brown_glazed_terracotta brown_mushroom_block brown_terracotta brown_wool bubble_coral_block cartography_table carved_pumpkin chain_command_block chiseled_quartz_block chiseled_red_sandstone chiseled_sandstone chiseled_stone_bricks clay coal_block coal_ore coarse_dirt cobblestone command_block cracked_stone_bricks crafting_table cut_red_sandstone cut_sandstone cyan_concrete cyan_concrete_powder cyan_glazed_terracotta cyan_terracotta cyan_wool dark_oak_log dark_oak_planks dark_oak_wood dark_prismarine dead_brain_coral_block dead_bubble_coral_block dead_fire_coral_block dead_horn_coral_block dead_tube_coral_block diamond_block diamond_ore diorite dirt dispenser dried_kelp_block dropper emerald_block emerald_ore end_stone end_stone_bricks fire_coral_block fletching_table furnace gold_block gold_ore granite grass_block grass_path gravel gray_concrete gray_concrete_powder gray_glazed_terracotta gray_terracotta gray_wool green_concrete green_concrete_powder green_glazed_terracotta green_terracotta green_wool hay_block honey_block honeycomb_block horn_coral_block infested_chiseled_stone_bricks infested_cobblestone infested_cracked_stone_bricks infested_mossy_stone_bricks infested_stone infested_stone_bricks iron_block iron_ore jukebox jungle_log jungle_planks jungle_wood lapis_block lapis_ore light_blue_concrete light_blue_concrete_powder light_blue_glazed_terracotta light_blue_terracotta light_blue_wool light_gray_concrete light_gray_concrete_powder light_gray_glazed_terracotta light_gray_terracotta light_gray_wool lime_concrete lime_concrete_powder lime_glazed_terracotta lime_terracotta lime_wool loom magenta_terracotta magenta_wool melon mossy_cobblestone mossy_stone_bricks mushroom_stem mycelium nether_bricks nether_quartz_ore nether_wart_block netherrack note_block oak_log oak_planks oak_wood observer obsidian orange_concrete orange_concrete_powder orange_glazed_terracotta orange_terracotta orange_wool packed_ice pink_concrete pink_concrete_powder pink_glazed_terracotta pink_terracotta pink_wool podzol polished_andesite polished_diorite prismarine prismarine_bricks pumpkin purple_concrete purple_concrete_powder purple_glazed_terracotta purple_terracotta purple_wool purpur_block purpur_pillar quartz_block quartz_pillar red_concrete red_concrete_powder red_glazed_terracotta red_mushroom_block red_nether_bricks red_sand red_sandstone red_terracotta red_wool redstone_block redstone_lamp redstone_ore repeating_command_block sand sandstone smithing_table smoker smooth_quartz smooth_red_sandstone smooth_sandstone smooth_stone snow_block soul_sand sponge spruce_log spruce_planks spruce_wood stone stone_bricks stripped_acacia_log stripped_acacia_wood stripped_birch_log stripped_birch_wood stripped_dark_oak_log stripped_dark_oak_wood stripped_jungle_log stripped_jungle_wood stripped_oak_log stripped_oak_wood stripped_spruce_log stripped_spruce_wood structure_block terracotta tnt tube_coral_block wet_sponge white_concrete white_concrete_powder white_glazed_terracotta white_terracotta white_wool yellow_concrete yellow_concrete_powder yellow_glazed_terracotta yellow_terracotta yellow_wool acacia_leaves birch_leaves dark_oak_leaves jungle_leaves oak_leaves spawner spruce_leaves black_stained_glass blue_stained_glass brown_stained_glass cyan_stained_glass glass gray_stained_glass green_stained_glass ice light_blue_stained_glass light_gray_stained_glass lime_stained_glass magenta_stained_glass orange_stained_glass pink_stained_glass purple_stained_glass red_stained_glass white_stained_glass yellow_stained_glass tinted_glass beacon glowstone sea_lantern".split() or b.endswith("block") or b.endswith("stone") or b.endswith("ore")

def emissive_bit(block: str):
    b = block[len("minecraft:"):]
    return is_beacon(b) or is_fire(b) or is_campfire(b) or b=="glowstone" or b=="jack_o_lantern" or b=="lava" or b=="lava_cauldron" or b=="redstone_lamp" or b=="sea_lantern" or b=="sea_pickle" or is_conduit(b) or is_lantern(b) or b=="shroomlight" or b=="respawn_anchor" or b=="froglight" or is_end_rod(b) or is_torch(b) or is_wall_torch(b) or b=="cave_vines" or b.endswith("furnace") or b=="smoker" or is_candle(b) or is_end_gateway(b) or is_end_portal(b) or b.endswith("redstone_ore") or b=="magma_block"

def translucent_bit(block: str):
    b = block[len("minecraft:"):]
    return b.endswith("glass") or is_glass_pane(b) or b.endswith("water")

def cube_shape(b: str):
    '''
    标准的方块
    '''
    return is_cube(b)
def facing_cube_shape(b: str):
    '''
    有朝向的方块
    '''
    return is_cube(b) & (is_furnace(b) | is_smoker(b))
def low_cube_shape(b: str):
    '''
    有 0-7 个高度的方块
    '''
    return is_snow(b) | is_lava(b) | is_farmland(b) | is_cake(b) | is_candle_cake(b) | is_water(b) | is_end_portal(b)
def pane_shape(b: str):
    '''
    过方块中心的平面
    '''
    return is_glass_pane(b) | is_iron_bars(b)
def subvoxel_shape(b: str):
    '''
    使用 8 个体素描述的方块. 不存在朝向
    '''
    return is_slab(b) | is_stairs(b) | is_stonecutter(b) | is_daylight_detector(b)
def torch_shape(b: str):
    '''
    火把
    '''
    return is_torch(b)
def wall_torch_shape(b: str):
    '''
    墙上的火把
    '''
    return is_wall_torch(b)
def lantern_shape(b: str):
    '''
    灯笼
    '''
    return is_lantern(b)
def end_rod_shape(b: str):
    '''
    末地烛
    '''
    return is_end_rod(b)
def fence_shape(b):
    return is_fence(b)
def fire_shape(b):
    return is_fire(b) | is_campfire(b)
def cross_shape(b):
    return is_cross(b)
def crop_shape(b):
    return is_crop(b)
def unknow_shape(b):
    '''
    不影响全局光的形状
    '''
    g = dict(globals())
    for k, v in g.items():
        if k!="unknow_shape" & k.endswith("_shape"):
            if k(b):
                return False
    return True

def has_facing(b):
    return facing_cube_shape(b) or wall_torch_shape(b) or end_rod_shape(b)
def has_lit(b):
    return is_furnace(b) or is_smoker(b) or is_redstone_lamp(b) or is_campfire(b) or is_redstone_ore(b) or is_redstone_torch(b)
def has_nsew(b):
    return fence_shape(b) or pane_shape(b)
def has_hanging(b):
    return lantern_shape(b)

if __name__ == "__main__":
    '''
    High XXXXXXXX YYWZZZZZ Low
    ZZZZZ: shapeCode
    YY: ebit, tbit
    W: reserved
    XXXXXXXX: facing selector bits
    '''
    ids = {}

    for block in blocks:
        ebit = 1 if emissive_bit(block) else 0
        tbit = 1 if translucent_bit(block) else 0
        shapeCode = 0
        if cube_shape(block):
            shapeCode = 1
        if facing_cube_shape(block):
            shapeCode = 2
        if low_cube_shape(block):
            shapeCode = 4
        if pane_shape(block):
            shapeCode = 5
        if subvoxel_shape(block):
            shapeCode = 6
        if torch_shape(block):
            shapeCode = 7
        if wall_torch_shape(block):
            shapeCode = 8
        if lantern_shape(block):
            shapeCode = 9
        if end_rod_shape(block):
            shapeCode = 10
        if fence_shape(block):
            shapeCode = 11
        if fire_shape(block):
            shapeCode = 13
        if cross_shape(block):
            shapeCode = 14
        if crop_shape(block):
            shapeCode = 15
        facing_selector = {"": 0}
        if has_facing(block):
            facing_selector = {
                ":facing=north".replace(" ", ""): 0,
                ":facing=south".replace(" ", ""): 1,
                ":facing=east ".replace(" ", ""): 2,
                ":facing=west ".replace(" ", ""): 3,
                ":facing=up   ".replace(" ", ""): 4,
                ":facing=down ".replace(" ", ""): 5,
            }
        nsew_selector = {"": 0}
        if has_nsew(block):
            nsew_selector = {
                ":east=false:north=false:south=false:west=false".replace(" ", ""): 0,
                ":east=false:north=false:south=false:west=true ".replace(" ", ""): 1,
                ":east=false:north=false:south=true :west=false".replace(" ", ""): 2,
                ":east=false:north=false:south=true :west=true ".replace(" ", ""): 3,
                ":east=false:north=true :south=false:west=false".replace(" ", ""): 4,
                ":east=false:north=true :south=false:west=true ".replace(" ", ""): 5,
                ":east=false:north=true :south=true :west=false".replace(" ", ""): 6,
                ":east=false:north=true :south=true :west=true ".replace(" ", ""): 7,
                ":east=true :north=false:south=false:west=false".replace(" ", ""): 8,
                ":east=true :north=false:south=false:west=true ".replace(" ", ""): 9,
                ":east=true :north=false:south=true :west=false".replace(" ", ""): 10,
                ":east=true :north=false:south=true :west=true ".replace(" ", ""): 11,
                ":east=true :north=true :south=false:west=false".replace(" ", ""): 12,
                ":east=true :north=true :south=false:west=true ".replace(" ", ""): 13,
                ":east=true :north=true :south=true :west=false".replace(" ", ""): 14,
                ":east=true :north=true :south=true :west=true ".replace(" ", ""): 15
            }
        lit_selector = {"": ebit}
        if has_lit(block):
            lit_selector = {
                ":lit=true": 1,
                ":lit=false": 0,
            }
        hanging_selector = {"": 0}
        if has_hanging(block):
            hanging_selector = {
                ":hanging=true": 1,
                ":hanging=false": 0,
            }
        for f in facing_selector:
            for nsew in nsew_selector:
                for lit in lit_selector:
                    for hang in hanging_selector:
                        blockID = shapeCode + (lit_selector[lit] << 6) + (tbit << 7) + (nsew_selector[nsew] << 8) + (facing_selector[f] << 8) + (hanging_selector[hang] << 8)
                        selector = block + lit + f + nsew
                        if blockID in ids:
                            ids[blockID].append(selector)
                        else:
                            ids[blockID] = [selector]
    print("正在写入 src/block.properties")
    with open("src/block.properties", "w") as f:
        for id in sorted(ids.keys()):
            f.write(f"block.{id}={' '.join(ids[id])}\n")
