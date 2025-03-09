#!/bin/bash

# Function to generate a random alphanumeric character
generate_random_char() {
    echo $(tr -dc 'a-zA-Z0-9' < /dev/urandom | head -c1)
}

# Specify the total number of characters and characters per line
num_characters=600000
characters_per_line=120000

# Generate the random sequence
random_sequence=""
for ((i=0; i<num_characters; i++)); do
    random_sequence="${random_sequence}$(generate_random_char)"
done

# Split the random sequence into lines
split_sequence=$(echo "$random_sequence" | fold -w $characters_per_line)

# Save the random sequence to a file
echo "$split_sequence" > test25.txt

echo "Random sequence generated and saved to random_sequence.txt"

