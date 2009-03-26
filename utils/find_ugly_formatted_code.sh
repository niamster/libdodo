#!/bin/bash

#gaps after PP directives
pcregrep -Mn '#(if|else)[^\n]*\n\s*\n' trunk/src/* trunk/include/libdodo/*

#gaps before PP directived
pcregrep -Mn '\n\s*\n#(endif|else)' trunk/src/* trunk/include/libdodo/*

#big gaps
pcregrep -Mn '\n\s*\n\s*\n' trunk/src/* trunk/include/libdodo/*

pcregrep -Mn '^\/\/-------------------------------------------------------------------\n[^\n]' trunk/src/*
pcregrep -Mn '[^\n]\n\/\/-------------------------------------------------------------------' trunk/src/*
