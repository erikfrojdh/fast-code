import sys; sys.path.append("/Users/erik/software/aareinstall")

from aare import RawFile
import time

N = 10000
t0 = time.perf_counter()
with RawFile("/Users/erik/data/Moench03new/cu_half_speed_master_4.json") as f:
    _, data = f.read_n(N)
t1 = time.perf_counter()
print(f"Reading {N} frames took {t1-t0:.3f} seconds")
pd = data.mean(axis=0)
t2 = time.perf_counter()
print(f"Calculating pedestal took {t2-t1:.3f} seconds")
GB = data.itemsize * data.size / 1e9
tt = t2-t0
print(f"Total time: {tt:.3f} seconds {GB/tt:.3f} GB/s")