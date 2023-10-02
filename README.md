# GOPH a Gopher Server written in C

I was wanting a gopher server for my FreeBSD box that took some simple defaults and just runs. I decided to write one myself after watching a Youtube video on Unix socket progamming (something I haven't touched for years).

## Building from source

Being written on FreeBSD I am using the default clang compiler. Building on Linux shouldn't be problematic though the makefile should be adjusted accordingly.

## Binaries created 

Make creates the following binary: 'gopherd'

## Other functions

I hope to add other functions into the server including:

1.	Ability to create a sample gophermap prepopulated with a list of link types
1.	A tool to build .cache files from the contents of a directory
1.	A tool to walk through the gopher server checking links both local and remote


