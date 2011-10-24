#! /bin/sh
this_script_dir=$(cd $(dirname $0) && pwd)
there="${this_script_dir}/tools"

${there}/playgame.py --strict -t 30 -m ${there}/maps/example/tutorial1.map $*
