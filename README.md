# HumongousInt
The class for writing unbounded integers into memory

Unlike several other approaches, this one is intended to be used as a substitute for int, long, or long long (and also for double or long double). Meaning that the intention is that it should be able to perform all the same actions, even the ones that are unsafe for something that takes more than 50% of your memory.
