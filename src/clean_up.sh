#!/bin/bash

rm -r log/

`mysql -u zhen -pZhenjiang#1 capstone --execute='delete from command;'`
`mysql -u zhen -pZhenjiang#1 capstone --execute='delete from xyz;'`
