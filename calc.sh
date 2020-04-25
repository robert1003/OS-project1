
unit_time=$1

echo FIFO
for i in {1..5};
do
    echo $i
    python3 calc.py FIFO_${i} $unit_time
done

echo SJF
for i in {1..5};
do
    echo $i
    python3 calc.py SJF_${i} $unit_time
done

echo PSJF
for i in {1..5};
do
    echo $i
    python3 calc.py PSJF_${i} $unit_time
done

echo RR
for i in {1..5};
do
    echo $i
    python3 calc.py RR_${i} $unit_time
done
