#!/bin/bash

../bootgen -arch versal -read partition.pdi > log_content
../bootgen -arch versal -image bootimage.bif -w -o partition_new.pdi
../bootgen -arch versal -read partition_new.pdi > log_content_new
