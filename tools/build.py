import os
import re
import argparse

parser = argparse.ArgumentParser(description='OptiFine 着色器预处理器.')

parser.add_argument("-o", "--output", default=".", help="目标文件夹. 将在该文件夹内建立 shaders 目录")
parser.add_argument("-s", "--source", default="src", help="源码文件夹.")
parser.add_argument("-v", "--verbose", action="store_true", help="打印构建详细过程.")
parser.add_argument("-m", "--sourcemap", default=None, help="在构建结果中添加源码文件和行号信息.")
parser.add_argument("-g", "--gbuffers", action="store_true", default=False, help="使用模板生成 gbuffers_*.*sh, 以使源码文件夹无需包含这些文件.")

args = parser.parse_args()

outputdir = args.output
sourcedir = args.source


def ensureOutputDir():
    if args.verbose:
        print("检查目标文件夹是否存在.")
    if not os.path.isdir(outputdir):
        os.mkdir(outputdir)
        if args.verbose:
            print("    已新建目标文件夹.")
    if not os.path.isdir(f"{outputdir}/shaders"):
        os.mkdir(f"{outputdir}/shaders")
        if args.verbose:
            print("    已新建目标着色器文件夹.")


assets = set()
sources = {}

sourceTemplates = {
    "main": {
        ".vsh": '#include "core/gbuffers/gbuffers_main.vert"',
        ".fsh": '#include "core/gbuffers/gbuffers_main.frag"'
    },
    "entity": {
        ".vsh": '#define GBUFFERS_ENTITIES\n#include "core/gbuffers/gbuffers_main.vert"',
        ".fsh": '#define GBUFFERS_ENTITIES\n#include "core/gbuffers/gbuffers_main.frag"'
    },
    "discard": {
        ".vsh": '#include "core/gbuffers/gbuffers_discard.vert"',
        ".fsh": '#include "core/gbuffers/gbuffers_discard.frag"'
    },
    "shadow": {
        ".vsh": '#include "core/shadow/shadow.vert"',
        ".gsh": '#include "core/shadow/shadow.geom"',
        ".fsh": '#include "core/shadow/shadow.frag"',
    },
    "full_screen": {
        ".vsh": '#include "core/full_screen.vert"'
    }
}
virtualSourceFiles = {
    f"{sourcedir}/shadow": "shadow",
    f"{sourcedir}/gbuffers_basic": "main",
    f"{sourcedir}/gbuffers_skybasic": "discard",
    f"{sourcedir}/gbuffers_skytextured": "discard",
    f"{sourcedir}/gbuffers_clouds": "discard",
    f"{sourcedir}/gbuffers_terrain": "discard",
    f"{sourcedir}/gbuffers_damagedblock": "discard",
    f"{sourcedir}/gbuffers_block": "discard",
    f"{sourcedir}/gbuffers_beaconbeam": "discard",
    f"{sourcedir}/gbuffers_entities": "entity",
    f"{sourcedir}/gbuffers_entities_glowing": "entity",
    f"{sourcedir}/gbuffers_armor_glint": "discard",
    f"{sourcedir}/gbuffers_spidereyes": "discard",
    f"{sourcedir}/gbuffers_weather": "discard",
    f"{sourcedir}/gbuffers_item": "main",
    f"{sourcedir}/gbuffers_textured": "main",
    f"{sourcedir}/gbuffers_textured_lit": "main",
    f"{sourcedir}/gbuffers_terrain": "main",
    f"{sourcedir}/gbuffers_damagedblock": "main",
    f"{sourcedir}/gbuffers_block": "main",
    f"{sourcedir}/gbuffers_hand": "discard",
    f"{sourcedir}/gbuffers_water": "discard",
    f"{sourcedir}/deferred": "full_screen",
    f"{sourcedir}/deferred2": "full_screen",
    f"{sourcedir}/deferred3": "full_screen",
    f"{sourcedir}/deferred4": "full_screen",
    f"{sourcedir}/deferred5": "full_screen",
    f"{sourcedir}/deferred6": "full_screen",
    f"{sourcedir}/deferred7": "full_screen",
    f"{sourcedir}/deferred8": "full_screen",
    f"{sourcedir}/deferred9": "full_screen",
    f"{sourcedir}/deferred10": "full_screen",
    f"{sourcedir}/composite14": "full_screen",
    f"{sourcedir}/composite15": "full_screen",
    f"{sourcedir}/final": "full_screen",
}


def scanSources(filter, root=f"{sourcedir}/"):
    if not root.endswith("/"):
        root += "/"
    paths = os.listdir(root)
    for p in paths:
        path = root+p
        if os.path.isfile(path) and (not path.startswith(".")):
            if filter(path):
                if args.verbose:
                    print(f"    未找到源码文件: {path}")
                with open(path) as f:
                    sources[path] = {
                        "content": f.read(),
                        "parentdir": root,
                        "qid": None
                    }
                    sources[path]["original"] = sources[path]["content"]
            else:
                assets.add(path)
        else:
            scanSources(filter, root=root+p+"/")


def isSource(fn):
    return fn.endswith(".vsh") or \
        fn.endswith(".fsh") or \
        fn.endswith(".gsh") or \
        fn.endswith(".csh") or \
        fn.endswith(".vert") or \
        fn.endswith(".frag") or \
        fn.endswith(".geom") or \
        fn.endswith(".comp") or \
        fn.endswith(".hpp") or \
        fn.endswith(".glsl")


def isObject(fn):
    return fn.endswith(".vsh") or \
        fn.endswith(".fsh") or \
        fn.endswith(".gsh") or \
        fn.endswith(".csh")


def isAsset(fn):
    return not isSource(fn)


def dfsprocess(target, qid, depth=0):
    '''
    target: DFS的起始点
    qid: 染色值, 若遇到染过色的节点则跳过. 如果不包含#pragma once, 则不染色
    depth: 防止深度过大
    '''
    if depth >= 8:
        print(f"文件包含嵌套过深: {target}")
    assert depth <= 16, f"已达到最大包含深度. {target}"
    pattern = re.compile("^#include\s+(.+)")
    if sources[target]["qid"] == qid:
        return
    sources[target]["content"] = sources[target]["original"]
    # 仅用于编辑器代码着色
    # 不会递归地预处理
    if "#pragma none" in sources[target]["content"]:
        if args.verbose:
            print(f"    {target} ignored.")
        sources[target]["content"] = ""
        sources[target]["qid"] = qid
        return
    if sources[target]["content"].startswith("#pragma once"):
        sources[target]["qid"] = qid
    if depth == 0:
        print(f"正在处理 {target}")
    lines = sources[target]["content"].split('\n')
    buffer = []
    hasMatch = False
    for i, l in enumerate(lines):
        match = re.search(pattern, l)
        if match:
            hasMatch = True
            targetAbs = f"{sourcedir}/" + match.groups()[0][1:-1]
            if sources[targetAbs]["qid"] != qid:
                if args.verbose:
                    print("    " * (depth+1) + f"包含 {targetAbs}")
                dfsprocess(targetAbs, qid, depth=depth+1)
                if sources[targetAbs]['content'].startswith("#pragma once"):
                    buffer.append(sources[targetAbs]["content"][len("#pragma once"):])
                else:
                    buffer.append(sources[targetAbs]['content'])
            else:
                buffer.append(f"// {l}")
        else:
            if args.sourcemap and len(l) == 0:
                buffer.append(f'''// file://{args.sourcemap}/{target}#L{i+1}''')
            else:
                buffer.append(l.replace("::", "_N_"))
    if hasMatch:
        def processDrawBuffers(s):
            ts = [g for g in s]
            return ",".join(ts)

        def processRenderTargets(s):
            gs = s.split(",")
            ts = [g.strip() for g in gs]
            assert len([t for t in ts if len(t) == 0]) == 0, f"render targets 声明有误: {s}"
            return ",".join(ts)
        drawbuffers = re.compile("^\s*#pragma\s+drawbuffers\(([0-9]+)\)")
        replaced_content = [drawbuffers.sub(
            (lambda tg:f"/* RENDERTARGETS: {processDrawBuffers(tg.group(1))} */"), l) for l in buffer if l != ""]
        binliterals = re.compile("0b([0-9]+)")
        replaced_content = [binliterals.sub(
            (lambda tg:f"{hex(int(tg.group(1), base=2))}"), l) for l in replaced_content if l != ""]
        rendertargets = re.compile("^\s*#pragma\s+rendertargets\(([0-9,\s]+)\)")
        sources[target]['content'] = "\n".join([rendertargets.sub(
            (lambda tg:f"/* RENDERTARGETS: {processRenderTargets(tg.group(1))} */"), l) for l in replaced_content if l != ""])


def preprocess():
    qid = 0
    try:
        for fn, source in sources.items():
            if isObject(fn):
                dfsprocess(fn, qid, depth=0)
                qid += 1
    except KeyError as e:
        print(f"文件不存在: {e.args[0]}")
        os._exit(1)


def dumpSources(filter, root=f"{sourcedir}/"):
    paths = [p[len(sourcedir)+1:] for p in sources.keys()]
    for p in paths:
        path = root+p
        if filter(path):
            with open(f'{outputdir}/shaders/{p}', "w") as f:
                if p.endswith(".csh"):
                    f.write("#version 430 core\n")
                else:
                    f.write("#version 430 compatibility\n")
                f.write(sources[path]['content'])


def safeOpen(path, *args, **kwargs):
    if os.path.isfile(path):
        return open(path, *args, **kwargs)
    paths = path.split('/')
    for i in range(1, len(paths)):
        temp = "/".join(paths[:i])
        if not os.path.isdir(temp):
            os.mkdir(temp)
    return open(path, *args, **kwargs)


def dumpAssets(filter):
    for asset in assets:
        if filter(asset):
            target = f"{outputdir}/shaders/{asset[len(sourcedir)+1:]}"
            print(f"正在写入 {target}")
            with open(asset, "rb") as f:
                with safeOpen(target, "wb") as g:
                    g.write(f.read())


if __name__ == "__main__":
    ensureOutputDir()
    if args.verbose:
        print("正在扫描源文件.")
    scanSources(isSource)
    if args.gbuffers:
        for name, kind in virtualSourceFiles.items():
            property = sourceTemplates[kind]
            for ext, content in property.items():
                sources[name + ext] = {
                    "content": content,
                    "original": content,
                    "qid": None
                }
    if args.verbose:
        print("正在进行预处理.")
    preprocess()
    if args.verbose:
        print("正在写入目标文件.")
    dumpSources(isObject)
    if args.verbose:
        print("正在写入资源文件.")
    dumpAssets(isAsset)
