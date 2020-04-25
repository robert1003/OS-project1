import sys
import os

fname = sys.argv[1]
time_unit = sys.argv[2]

theory_time = 0
with open(os.path.join('output_stderr', fname + '_stderr.txt'), 'r') as f:
    for line in f:
        a, b, c = list(map(int, line.split()))
        if a == 2:
            theory_time += b
try:
    time_unit = float(time_unit)
    theory_time *= time_unit
except:
    pass

name_to_pid = {}
with open(os.path.join('output', fname + '_stdout.txt'), 'r') as f:
    for line in f:
        name, pid = line.split()
        name_to_pid[name] = pid

pid_to_real_time = {}
times = []
with open(os.path.join('output', fname + '_dmesg.txt'), 'r') as f:
    for line in f:
        pid, st, ed = line.split()[2:]
        pid_to_real_time[pid] = float(ed) - float(st)
        times += [float(ed), float(st)]

times = sorted(times)
print(times[0], times[-1])
tot_time = times[-1] - times[0]
sum_time = 0
for name in sorted(name_to_pid):
    pid = name_to_pid[name]
    real_time = pid_to_real_time[pid]
    print(name, pid, real_time)
    sum_time += real_time

print('sum: {:.5f}'.format(sum_time))
print('theory time: {:.5f}, total time: {:.5f}, error: {:.5}%'.format(theory_time, tot_time, ((tot_time - theory_time) / theory_time) * 100))
