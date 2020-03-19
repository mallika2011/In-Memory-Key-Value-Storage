import secrets
import string  
import numpy as np
from functools import partial

def produce_amount_keys(amount_of_keys, _randint=np.random.randint):
    keys = set()
    pickchar = partial(secrets.choice, string.ascii_lowercase)
    while len(keys) < amount_of_keys:
        keys |= {''.join([pickchar() for _ in range(_randint(1, 10))]) for _ in range(amount_of_keys - len(keys))}
    return keys

def produce_amount_values(amount_of_values, _randint=np.random.randint):
    values = set()
    pickchar = partial(secrets.choice, string.ascii_lowercase)
    while len(values) < amount_of_values:
        values |= {''.join([pickchar() for _ in range(_randint(1, 20))]) for _ in range(amount_of_values - len(values))}
    return values

n = 1000
keys = list(produce_amount_keys(n))
# values = list(produce_amount_values(n))


for i in range(len(keys)):
    # print(keys[i] ," ", values[i])
    print(keys[i])

