import csv
import random

# Constants for generating instances
NUM_INSTANCES_PER_SIZE = 4  # Number of instances to generate per problem size
MAX_ITEMS = 31              # Maximum number of items in the largest problem
MIN_ITEMS = 5                # Minimum number of items in the smallest problem
ITEM_STEP = 2               # Step size for increasing number of items
MAX_WEIGHT = 5
MAX_VALUE = 5
MAX_CAPACITY = 30

# Define the CSV filename
csv_filename = 'instances.csv'

# Generate instances
instances = []
for num_items in range(MIN_ITEMS, MAX_ITEMS + 1, ITEM_STEP):
    for _ in range(NUM_INSTANCES_PER_SIZE):
        min_weight=random.randint(1,MAX_WEIGHT-1)
        capacity = random.randint(MAX_CAPACITY // 2, MAX_CAPACITY)
        weights = [random.randint(1, MAX_WEIGHT) for _ in range(num_items)]
        values = [random.randint(1, MAX_VALUE) for _ in range(num_items)]
        instance = [capacity , num_items, min_weight] + [val for pair in zip(weights, values) for val in pair]
        #instance = [capacity , num_items] + [val for pair in zip(weights, values) for val in pair]
        instances.append(instance)

# Write instances to CSV file
with open(csv_filename, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerows(instances)
