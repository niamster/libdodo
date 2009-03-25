#!/bin/bash

#gaps after PP directives
pcregrep -Mn 'endif\n#' trunk/src/* trunk/include/libdodo/*

#gaps before PP directived
pcregrep -Mn '^\n\n#' trunk/src/* trunk/include/libdodo/*

#big gaps
pcregrep -Mn '\n\n\n' trunk/src/* trunk/include/libdodo/*

pcregrep -Mn '^\/\/-------------------------------------------------------------------\n[^\n]' trunk/src/*
pcregrep -Mn '[^\n]\n\/\/-------------------------------------------------------------------' trunk/src/*
