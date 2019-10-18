#! /bin/sh

if [[ "$OSTYPE" == "darwin"* ]]; then
    export DYLD_INSERT_LIBRARIES="$1" 
    export DYLD_FORCE_FLAT_NAMESPACE=1
else
    export LD_PRELOAD="$1"
fi

cc -shared -fPIC dev_zero.c -o dev_zero.so
cc reads_dev_zero.c -o reads_dev_zero
export USFS_PATH="/dev/zero:dev_zero.so"

./reads_dev_zero
if [[ $? == 0 ]] ; then
    echo dev_zero passed
else 
    echo dev_zero passed
fi

rm reads_dev_zero dev_zero.so

echo $ld_preload
