#! /bin/sh
this_script_dir=$(cd $(dirname $0) && pwd)
there="${this_script_dir}/../aichallenge/ants"
game_id=$1
shift

${there}/playgame.py                                        \
	--verbose                                    \
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
	--log_dir /tmp/logs                                 \
	--turns 300                                         \
	--map_file ${there}/maps/example/tutorial1.map      \
	--serial                                            \
	"$@"                                                \
	"./termite"                                         \
	"python ${there}/dist/sample_bots/python/HunterBot.py"
