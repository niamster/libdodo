#!/bin/bash

sed -i -r 's/[ \t]{2,}(\/\/.*)/ \1/' trunk/include/libdodo/*h
