# compile
make clean
make debug

# clean message
dmesg -C

# configs
input_dir=testdata
output_dir=output
my_dir=output_stderr

# TIME_MEASUREMENT.txt
echo TIME_MEASUREMENT
./main < $input_dir/TIME_MEASUREMENT.txt > $output_dir/TIME_MEASUREMENT_stdout.txt 2> $my_dir/TIME_MEASUREMENT_stderr.txt
dmesg -c | grep Project1 > $output_dir/TIME_MEASUREMENT_dmesg.txt
sleep 1

# run FIFO
echo FIFO
for i in {1..5};
do
    echo $i
    ./main < $input_dir/FIFO_${i}.txt > $output_dir/FIFO_${i}_stdout.txt 2> $my_dir/FIFO_${i}_stderr.txt
    dmesg -c | grep Project1 > $output_dir/FIFO_${i}_dmesg.txt
    sleep 1
done
    
# run SJF
echo SJF
for i in {1..5};
do
    echo $i
    ./main < $input_dir/SJF_${i}.txt > $output_dir/SJF_${i}_stdout.txt 2> $my_dir/SJF_${i}_stderr.txt
    dmesg -c | grep Project1 > $output_dir/SJF_${i}_dmesg.txt
    sleep 1
done

# run PSJF
echo PSJF
for i in {1..5};
do
    echo $i
    ./main < $input_dir/PSJF_${i}.txt > $output_dir/PSJF_${i}_stdout.txt 2> $my_dir/PSJF_${i}_stderr.txt
    dmesg -c | grep Project1 > $output_dir/PSJF_${i}_dmesg.txt
    sleep 1
done

# run RR
echo RR
for i in {1..5};
do
    echo $i
    ./main < $input_dir/RR_${i}.txt > $output_dir/RR_${i}_stdout.txt 2> $my_dir/RR_${i}_stderr.txt
    dmesg -c | grep Project1 > $output_dir/RR_${i}_dmesg.txt
    sleep 1
done
