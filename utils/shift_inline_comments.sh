#!/bin/bash

sed -i -r 's/[ \t]{2,}(\/\/.*)/ \1/' include/libdodo/*h
