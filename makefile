.PHONY : all clean pack patch autogen

all : autogen patch
	@mkdir -p shaders
	@python3 tools/build.py -g
	@tools/validate.sh

tools/blocks.txt :
	curl -L https://github.com/Radvylf/minecraft-lists/raw/master/1.17/1.17.1/blocks.txt -q -o tools/blocks.txt

src/block.properties : tools/blocks.txt tools/genblocks.py
	@tools/genblocks.py

shaders/final.fsh : tools/patch.py
	@python3 tools/patch.py

autogen : src/block.properties

patch : shaders/final.fsh

pack :
	zip -r ../HowToOptiFine.zip shaders

clean :
	rm -fr shaders/
