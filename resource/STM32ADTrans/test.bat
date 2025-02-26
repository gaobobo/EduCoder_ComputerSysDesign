@echo off
rem python oled_simer.py --clk 0 --din 1 --res 0 --dc 1 --cs 1

python oled_simer.py --clk 0 --din 1 --res 1 --dc 1 --cs 1
python oled_simer.py --clk 0 --din 1 --res 1 --dc 1 --cs 0

rem 0
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 0 --din 1 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 1 --din 1 --res 1 --dc 1 --cs 0
rem 1
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 1 --din 0 --res 1 --dc 1 --cs 0
rem 2
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 1 --din 0 --res 1 --dc 1 --cs 0
rem 3
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 1 --din 0 --res 1 --dc 1 --cs 0
rem 4
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 1 --din 0 --res 1 --dc 1 --cs 0
rem 5
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 1 --din 0 --res 1 --dc 1 --cs 0
rem 6
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 1 --din 0 --res 1 --dc 1 --cs 0
rem 7
python oled_simer.py --clk 0 --din 0 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 0 --din 1 --res 1 --dc 1 --cs 0
python oled_simer.py --clk 1 --din 1 --res 1 --dc 1 --cs 0

python oled_simer.py --clk 1 --din 1 --res 1 --dc 1 --cs 1

