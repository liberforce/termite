#! /bin/sh
this_script_dir=$(cd $(dirname $0) && pwd)
there="${this_script_dir}/tools"

${there}/playgame.py                                        \
	--player_seed 42                                    \
	--end_wait=0.25                                     \
	--nolaunch                                          \
	--verbose                                           \
	-e                                                  \
	--log_dir ${there}/logs                             \
	--turns 30                                          \
	--map_file ${there}/maps/example/tutorial1.map      \
	"$@"                                                \
	termite                                             \
	"python ${there}/sample_bots/python/HunterBot.py" 
