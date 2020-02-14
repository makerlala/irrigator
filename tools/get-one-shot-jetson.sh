#!/bin/bash
#
# Copyright (c) 2018-2020 - Dumi Loghin (dumi@makerlala.com)
#
# This file is part of irrigator - an open source smart robot.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
#
# Get one shot on Nvidia Jetson TX1/2, Nano
#
rm nvcamtest_*.jpg
nvgstcapture -m 1 --image-res=4 --gainrange=1 --orientation=2 -A --capture-auto 
# > /dev/null 2>&1
mv nvcamtest_*.jpg one-shot.jpg

