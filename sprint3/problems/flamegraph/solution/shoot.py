import argparse
import subprocess
import time
import random
import shlex

RANDOM_LIMIT = 1000
SEED = 123456789
random.seed(SEED)

AMMUNITION = [
    'localhost:8080/api/v1/maps/map1',
    'localhost:8080/api/v1/maps'
]

SHOOT_COUNT = 100
COOLDOWN = 0.1


def start_server():
    parser = argparse.ArgumentParser()
    parser.add_argument('server', type=str)
    return parser.parse_args().server


def run(command, output=None):
    #print("run:", command)
    process = subprocess.Popen(shlex.split(command), stdout=output, stderr=subprocess.DEVNULL)
    return process


def stop(process, wait=False):
    if process.poll() is None and wait:
        process.wait()
    process.terminate()


def shoot(ammo):
    hit = run('curl ' + ammo, output=subprocess.DEVNULL)
    time.sleep(COOLDOWN)
    stop(hit, wait=True)


def make_shots():
    for _ in range(SHOOT_COUNT):
        ammo_number = random.randrange(RANDOM_LIMIT) % len(AMMUNITION)
        shoot(AMMUNITION[ammo_number])
    print('Shooting complete')


server = run(start_server())
#print(server.pid)

perf_record = run("perf record -F 100000 -p " + str(server.pid) + " -o perf.data -ag")
#print("perf end")
make_shots()
stop(server)
time.sleep(1)
stop(perf_record)
time.sleep(1)

perf_script = subprocess.Popen(shlex.split("perf script -i perf.data"), stdout=subprocess.PIPE)
stackcollapse = subprocess.Popen(shlex.split("./FlameGraph/stackcollapse-perf.pl"), stdin=perf_script.stdout, stdout = subprocess.PIPE)
with open('graph.svg', "w") as outfile:
    flamegraps = subprocess.Popen(shlex.split("./FlameGraph/flamegraph.pl"), stdin=stackcollapse.stdout, stdout=outfile)
    flamegraps.wait()

print('Job done')
