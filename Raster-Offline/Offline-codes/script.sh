# rm out.bmp

g++ -std=c++17 1605024.cpp -o 1605024

for i in 4
do 
    echo 'running '$i':'
    ./1605024  ./tests/$i/scene.txt  ./tests/$i/config.txt

    echo 'check-1 '
    diff -Z stage1.txt ./tests/$i/stage1.txt
    echo 'check-2 '
    diff -Z stage2.txt ./tests/$i/stage2.txt
    echo 'check-3 '
    diff -Z stage3.txt ./tests/$i/stage3.txt
    # echo 'check-z_buffer '
    # diff -Z z_out.txt ./tests/$i/z_buffer.txt > diff.txt

    code out.bmp
done
