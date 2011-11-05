#! /bin/sh
this_script_dir=$(cd $(dirname $0) && pwd)
there="${this_script_dir}/../tools"
game_id=$1
shift

${there}/playgame.py                                        \
	--player_seed 42                                    \
	--end_wait=0.25                                     \
	--nolaunch                                          \
	-g $game_id                                         \
	-R                                                  \
	-I                                                  \
	-O                                                  \
	-E                                                  \
	--turntime 9999900                                  \
	--loadtime 9999900                                  \
	--log_dir ${this_script_dir}/logs                   \
	--turns 300                                         \
	--map_file ${there}/maps/example/tutorial1.map      \
	--serial                                            \
	"$@"                                                \
	termite                                             \
	"python ${there}/sample_bots/python/HunterBot.py" 
