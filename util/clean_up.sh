#!/bin/bash
HOME_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
rm -r $HOME_DIR/log/

`mysql -u zhen -pZhenjiang#1 capstone --execute='delete from command;'`
`mysql -u zhen -pZhenjiang#1 capstone --execute='delete from xyz;'`
`mysql -u zhen -pZhenjiang#1 capstone --execute='delete from ui_xyz;'`
