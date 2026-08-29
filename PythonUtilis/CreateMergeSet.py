import os
import glob
import re

def create_merge_set(directory_path):
    output_filename = directory_path + "CandidateGapSequences_Merge.txt"
    
    # Locate all files matching the pattern in the target directory
    search_pattern = os.path.join(directory_path, "CandidateGapSequences_*.txt")
    file_list = glob.glob(search_pattern)
    
    total_files_processed = 0
    total_lines_merged = 0

    with open(output_filename, 'w') as outfile:
        for filepath in file_list:
            filename = os.path.basename(filepath)
            
            # Skip the output file if it happens to be generated in the same directory
            if filename == output_filename:
                continue
                
            # Extract 'X' from the filename (e.g., '100' from 'CandidateGapSequences_100.txt')
            match = re.match(r"CandidateGapSequences_(.+)\.txt", filename)
            if match:
                x_value = match.group(1)
                
                with open(filepath, 'r') as infile:
                    for line in infile:
                        clean_line = line.strip()
                        if clean_line:
                            # Prepend X- to the completely unchanged original line
                            outfile.write(f"{x_value}-{clean_line}\n")
                            total_lines_merged += 1
                
                total_files_processed += 1

    print(f"Merge complete. Created: {output_filename}")
    print(f"Files processed: {total_files_processed}")
    print(f"Total sequences merged: {total_lines_merged}")

if __name__ == "__main__":
    directory_path = "../Results/FinalSets/Criterion-Comparisons/"
    create_merge_set(directory_path)