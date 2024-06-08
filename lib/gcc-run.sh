#/bin/sh
if [ "$#" -ne 1 ]; then
    echo "Usage:  filename.c"
    exit 1
fi

filename=$1
out=$1$RANDOM

gcc $filename -o $out

if [ $? -eq 0 ]; then
    chmod +x ./$out
    ./$out
    rm ./$out
else
    echo "Compilation failed"
fi
