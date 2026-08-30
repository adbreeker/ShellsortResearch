def remove_duplicates_inplace(filepath):
    seen_sequences = set()
    unique_lines = []
    
    total_lines = 0
    duplicate_count = 0

    # Read the file and collect only unique, non-blank lines
    with open(filepath, 'r') as file:
        for line in file:
            clean_line = line.strip()
            
            # Ignore any existing empty lines so they aren't written back
            if not clean_line:
                continue
                
            total_lines += 1
            
            # Isolate the sequence for duplicate checking
            if ':' in clean_line:
                name, sequence = clean_line.split(':', 1)
                normalized_sequence = sequence.strip()
                
                if normalized_sequence in seen_sequences:
                    duplicate_count += 1
                else:
                    seen_sequences.add(normalized_sequence)
                    unique_lines.append(clean_line)

    # Overwrite the original file entirely with the unique lines
    # This prevents any "empty spaces" from being left behind.
    with open(filepath, 'w') as out_file:
        for item in unique_lines:
            out_file.write(item + '\n')

    # Calculate and display statistics
    if total_lines > 0:
        duplicate_percentage = (duplicate_count / total_lines) * 100
    else:
        duplicate_percentage = 0.0

    print(f"File processed: {filepath}")
    print(f"Total sequences processed: {total_lines}")
    print(f"Unique sequences retained: {len(unique_lines)}")
    print(f"Duplicates removed: {duplicate_count} ({duplicate_percentage:.2f}%)")

if __name__ == "__main__":
    path = "../Results/FinalSets/CompEval-WinningSequences.txt"
    remove_duplicates_inplace(path)