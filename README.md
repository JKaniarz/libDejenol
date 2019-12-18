# libDejenol
A library for working with the data files in "Mordor: The Depths of Dejenol"

## Key Files
### MTypes.h
It has WIP structs for all the major data files.
Known complete structs are Monster, Automap, GuildLog, dungeon Floor, Library,
and HallOfRecords
### MLoader (.cpp/.h)
It has functions for loading the types out of the data files.
### MSaver (.cpp/.h)
It will eventually have functions for saving all the types back to the data 
files. Right now it can only save monsters and the automap.
### RecordReader.h
Reads files in VB3's record style.
### RecordWriter.h
Writes files in VB3's record style.
## Example Programs
### superseer
A program the generates the map overlays for "The Super Seer" tool.
### pathfinder
A program that finds the shortest path between two points in the dungeon.
ForwardSolver calculates, for all squares, the distance away from a starting point.
ReverseSolver calculates, for all squares, the distance to an ending point.
(Command line support not implemented. You have to edit the coordinates and 
recompile.)
### revealmap
A program that looks through your automap and reveales inaccessible squares but
only after you've explored enough to prove that they are inaccessible. (Useful
for Type A people who don't want to make sacrifical characters to teleport into
rocks for the sake of having them marked on the map.)

If you provide a number as a command line parameter it will fully reveal that
floor. (For people who like spoilers.)
### monstersheet
A program that converts the monster data to/from csv format.

Usage:
	monstersheet infile outfile

Examples:
	monstersheet mdata5.mdr mdata5.csv
	monstersheet mdata5.csv mdata5.mdr

If no outfile is provided it will use the filename of the input.
If no parameters are provided it will convert mdata5.mdr to csv.