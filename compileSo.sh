
#source=$1 
#if [ -f $1 ]; then rm $1
#fi

gcc  -c -fpic  $1.c

gcc -shared -lc  -o $1.so  $1.o

cp $1.so /opt/lib
